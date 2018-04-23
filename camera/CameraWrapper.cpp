/*
 * Copyright (C) 2012-2016, The CyanogenMod Project
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
* @file CameraWrapper.cpp
*
* This file wraps a vendor camera module.
*/

#define LOG_TAG "CameraWrapper"

#include <camera/CameraParameters.h>
#include <camera/Camera.h>
#include <cutils/log.h>
#include <hardware/camera.h>
#include <hardware/hardware.h>
#include <utils/String8.h>
#include <utils/threads.h>

static const char KEY_DIS[] = "dis";
static const char DIS_DISABLE[] = "disable";
static const char KEY_ZSL[] = "zsl";
static const char ON[] = "on";
static const char OFF[] = "off";
static const char KEY_VIDEO_HFR_VALUES[] = "video-hfr-values";
static const char KEY_VIDEO_SNAPSHOT_SUPPORTED[] = "video-snapshot-supported";
static const char KEY_FLASH_MODE[] = "flash-mode";
static const char FLASH_MODE_TORCH[] = "mode-torch";
static const char KEY_RECORDING_HINT[] = "recording-hint";
static const char KEY_PREVIEW_FPS_RANGE[] = "preview-fps-range";
static const char KEY_PHASE_AF[] = "phase-af";
static const char KEY_DYNAMIC_RANGE_CONTROL[] = "dynamic-range-control";
static const char KEY_RT_HDR[] = "rt-hdr";
static const char KEY_SUPPORTED_RT_HDR[] = "rt-hdr-values";
static const char KEY_QC_VIDEO_HDR[] = "video-hdr";
static const char KEY_QC_VIDEO_HDR_VALUES[] = "video-hdr-values";

#define BACK_CAMERA_ID 0
#define FRONT_CAMERA_ID 1

using namespace android;

typedef struct wrapper_camera_device {
    camera_device_t base;
    int id;
    camera_device_t *vendor;
} wrapper_camera_device_t;

#define VENDOR_CALL(device, func, ...) ({ \
    wrapper_camera_device_t *__wrapper_dev = (wrapper_camera_device_t*) device; \
    __wrapper_dev->vendor->ops->func(__wrapper_dev->vendor, ##__VA_ARGS__); \
})

#define CAMERA_ID(device) (((wrapper_camera_device_t *)device)->id)

static Mutex gCameraWrapperLock;
static camera_module_t *gVendorModule = 0;

static camera_notify_callback gUserNotifyCb = NULL;
static camera_data_callback gUserDataCb = NULL;
static camera_data_timestamp_callback gUserDataCbTimestamp = NULL;
static camera_request_memory gUserGetMemory = NULL;
static void *gUserCameraDevice = NULL;

static int num_cameras = 0;
static char **fixed_set_params = NULL;

static int check_vendor_module()
{
    int rv;
    ALOGV("%s", __FUNCTION__);

    if (gVendorModule)
        return 0;

    rv = hw_get_module_by_class("camera", "vendor",
            (const hw_module_t**)&gVendorModule);
    if (rv)
        ALOGE("Failed to open vendor camera module %d", rv);

    return rv;
}

// nv12-venus is needed for blobs, but
// framework has no idea what it is
#define PIXEL_FORMAT_NV12_VENUS "nv12-venus"

static char *camera_fixup_getparams(int id, const char *settings)
{
    CameraParameters params;
    params.unflatten(String8(settings));

    ALOGV("%s: cam: %d, original parameters:", __FUNCTION__, id);
    params.dump();

    const char *recordHint = params.get(KEY_RECORDING_HINT);
    bool videoMode = recordHint ? !strcmp(recordHint, "true") : false;

    //Hide nv12-venus from Android.
    if (strcmp (params.getPreviewFormat(), PIXEL_FORMAT_NV12_VENUS) == 0)
          params.setPreviewFormat(params.PIXEL_FORMAT_YUV420SP);

    /* If the vendor has HFR values but doesn't also expose that
     * this can be turned off, fixup the params to tell the Camera
     * that it really is okay to turn it off.
     */
    const char *hfrModeValues = params.get(KEY_VIDEO_HFR_VALUES);
    if (hfrModeValues && !strstr(hfrModeValues, OFF)) {
        char hfrModes[strlen(OFF) + 1 + strlen(hfrModeValues) + 1];
        snprintf(hfrModes, sizeof hfrModes, "%s,%s", OFF, hfrModeValues);
        params.set(KEY_VIDEO_HFR_VALUES, hfrModes);
    }

    /* Enforce video-snapshot-supported to true */
    if (videoMode) {
        params.set(KEY_VIDEO_SNAPSHOT_SUPPORTED, "true");
    }

    /* advertise video HDR mode support from rt HDR mode */
    const char *vhdr_vals = params.get(KEY_SUPPORTED_RT_HDR);
    if (vhdr_vals)
        params.set(KEY_QC_VIDEO_HDR_VALUES, vhdr_vals);

    ALOGV("%s: Fixed parameters:", __FUNCTION__);
    params.dump();

    String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

    return ret;
}

static char *camera_fixup_setparams(int id, const char *settings)
{
    CameraParameters params;
    params.unflatten(String8(settings));

    ALOGV("%s: cam: %d, original parameters:", __FUNCTION__, id);
    params.dump();

    bool wasTorch = false;
    if (fixed_set_params[id]) {
        /* When torch mode is switched off, it is important not to set ZSL, to
           avoid a segmentation violation in libcameraservice.so. Hence, check
           if the last call to setparams enabled torch mode */
        CameraParameters old_params;
        old_params.unflatten(String8(fixed_set_params[id]));

        const char *old_flashMode = old_params.get(KEY_FLASH_MODE);
        wasTorch = old_flashMode && !strcmp(old_flashMode, FLASH_MODE_TORCH);
    }

    const char *recordingHint = params.get(KEY_RECORDING_HINT);
    bool isVideo = recordingHint && !strcmp(recordingHint, "true");
    const char *flashMode = params.get(KEY_FLASH_MODE);
    bool isTorch = flashMode && !strcmp(flashMode, FLASH_MODE_TORCH);

    if (!isTorch && !wasTorch) {
        if (isVideo) {
            params.set(KEY_DIS, DIS_DISABLE);
            params.set(KEY_ZSL, OFF);
        } else {
            params.set(KEY_ZSL, ON);
        }
    }

    if (id == BACK_CAMERA_ID) {
        params.set(KEY_PHASE_AF, ON);
        params.set(KEY_DYNAMIC_RANGE_CONTROL, ON);
    }
    if (isVideo) {
        /* need to translate video-hdr to rt-hdr */
        const char *vhdr = params.get(KEY_QC_VIDEO_HDR);
        params.set(KEY_RT_HDR, vhdr ?: OFF);
    }

    /* Fix for Camera2.  Without this, the cam 1 preview is much too dark. */
    if (id == FRONT_CAMERA_ID)
        params.set(KEY_PREVIEW_FPS_RANGE, "7000,30000");

    ALOGV("%s: Fixed parameters:", __FUNCTION__);
    params.dump();

    String8 strParams = params.flatten();
    if (fixed_set_params[id])
        free(fixed_set_params[id]);
    fixed_set_params[id] = strdup(strParams.string());
    char *ret = fixed_set_params[id];

    return ret;
}

/*******************************************************************
 * Implementation of camera_device_ops functions
 *******************************************************************/
static char *camera_get_parameters(struct camera_device *device);

static int camera_set_preview_window(struct camera_device *device,
        struct preview_stream_ops *window)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, set_preview_window, window);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

void camera_notify_cb(int32_t msg_type, int32_t ext1, int32_t ext2,
        void * __attribute__((unused)) user) {
    gUserNotifyCb(msg_type, ext1, ext2, gUserCameraDevice);
}

void camera_data_cb(int32_t msg_type, const camera_memory_t *data,
        unsigned int index, camera_frame_metadata_t *metadata,
        void * __attribute__((unused)) user) {
    gUserDataCb(msg_type, data, index, metadata, gUserCameraDevice);
}

void camera_data_cb_timestamp(nsecs_t timestamp, int32_t msg_type,
        const camera_memory_t *data, unsigned index,
        void * __attribute__((unused)) user) {
    gUserDataCbTimestamp(timestamp, msg_type, data, index, gUserCameraDevice);
}

camera_memory_t* camera_get_memory(int fd, size_t buf_size, uint_t num_bufs,
        void * __attribute__((unused)) user) {
    return gUserGetMemory(fd, buf_size, num_bufs, gUserCameraDevice);
}

static void camera_set_callbacks(struct camera_device *device,
        camera_notify_callback notify_cb,
        camera_data_callback data_cb,
        camera_data_timestamp_callback data_cb_timestamp,
        camera_request_memory get_memory,
        void *user)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    gUserNotifyCb = notify_cb;
    gUserDataCb = data_cb;
    gUserDataCbTimestamp = data_cb_timestamp;
    gUserGetMemory = get_memory;
    gUserCameraDevice = user;

    VENDOR_CALL(device, set_callbacks, camera_notify_cb, camera_data_cb,
            camera_data_cb_timestamp, camera_get_memory, user);
}

static void camera_enable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d, msg_type %d", __FUNCTION__, CAMERA_ID(device),
        msg_type);

    VENDOR_CALL(device, enable_msg_type, msg_type);
}

static void camera_disable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d, msg_type %d", __FUNCTION__, CAMERA_ID(device),
        msg_type);

    VENDOR_CALL(device, disable_msg_type, msg_type);
}

static int camera_msg_type_enabled(struct camera_device *device,
        int32_t msg_type)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return 0;
    }

    ALOGV("%s: cam %d, msg_type %d", __FUNCTION__, CAMERA_ID(device),
        msg_type);

    int ret = VENDOR_CALL(device, msg_type_enabled, msg_type);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_start_preview(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, start_preview);

    ALOGV("%s: ret %d", __FUNCTION__, ret);
    return ret;
}

static void camera_stop_preview(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    VENDOR_CALL(device, stop_preview);
}

static int camera_preview_enabled(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, preview_enabled);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_store_meta_data_in_buffers(struct camera_device *device,
        int enable)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d, enable %d", __FUNCTION__, CAMERA_ID(device), enable);

    int ret = VENDOR_CALL(device, store_meta_data_in_buffers, enable);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_start_recording(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    CameraParameters parameters;
    parameters.unflatten(String8(camera_get_parameters(device)));

    int ret = VENDOR_CALL(device, start_recording);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static void camera_stop_recording(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    VENDOR_CALL(device, stop_recording);
}

static int camera_recording_enabled(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, recording_enabled);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static void camera_release_recording_frame(struct camera_device *device,
        const void *opaque)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d, opaque %p", __FUNCTION__, CAMERA_ID(device), opaque);

    VENDOR_CALL(device, release_recording_frame, opaque);
}

static int camera_auto_focus(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, auto_focus);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_cancel_auto_focus(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, cancel_auto_focus);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_take_picture(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, take_picture);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_cancel_picture(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, cancel_picture);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static int camera_set_parameters(struct camera_device *device,
        const char *params)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    char *fixed_params = camera_fixup_setparams(CAMERA_ID(device), params);

    int ret = VENDOR_CALL(device, set_parameters, fixed_params);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static char *camera_get_parameters(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return NULL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    char *params = VENDOR_CALL(device, get_parameters);

    char *fixed_params = camera_fixup_getparams(CAMERA_ID(device), params);

    VENDOR_CALL(device, put_parameters, params);

    return fixed_params;
}

static void camera_put_parameters(__unused struct camera_device *device,
        char *params)
{
    if (params)
        free(params);
}

static int camera_send_command(struct camera_device *device,
        int32_t cmd, int32_t arg1, int32_t arg2)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d, cmd %d, arg1 %d, arg2 %d", __FUNCTION__,
        CAMERA_ID(device), cmd, arg1, arg2);

    int ret = VENDOR_CALL(device, send_command, cmd, arg1, arg2);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

static void camera_release(struct camera_device *device)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    VENDOR_CALL(device, release);
}

static int camera_dump(struct camera_device *device, int fd)
{
    if (!device) {
        ALOGE("%s: device NULL", __FUNCTION__);
        return -EINVAL;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(device));

    int ret = VENDOR_CALL(device, dump, fd);

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

extern "C" void heaptracker_free_leaked_memory(void);

static int camera_device_close(hw_device_t *device)
{
    int ret = 0;
    wrapper_camera_device_t *wrapper_dev = (wrapper_camera_device_t *) device;

    Mutex::Autolock lock(gCameraWrapperLock);

    if (!wrapper_dev) {
        ALOGE("%s: device NULL", __FUNCTION__);
        ret = -EINVAL;
        goto done;
    }

    ALOGV("%s: cam %d", __FUNCTION__, CAMERA_ID(wrapper_dev));

    if (fixed_set_params && fixed_set_params[wrapper_dev->id]) {
        free(fixed_set_params[CAMERA_ID(wrapper_dev)]);
        fixed_set_params[CAMERA_ID(wrapper_dev)] = NULL;
    }

    wrapper_dev->vendor->common.close((hw_device_t *) wrapper_dev->vendor);
    if (wrapper_dev->base.ops)
        free(wrapper_dev->base.ops);
    free(wrapper_dev);

done:
#ifdef HEAPTRACKER
    heaptracker_free_leaked_memory();
#endif

    ALOGV("%s: ret %d", __FUNCTION__, ret);

    return ret;
}

/*******************************************************************
 * Implementation of camera_module functions
 *******************************************************************/

/*
 * Open device handle to one of the cameras
 *
 * Assume camera service will keep singleton of each camera
 * so this function will always only be called once per camera instance
 */

static int camera_device_open(const hw_module_t *module, const char *name,
        hw_device_t **device)
{
    int rv = 0;
    int camera_id;
    wrapper_camera_device_t *camera_device = NULL;
    camera_device_ops_t *camera_ops = NULL;

    Mutex::Autolock lock(gCameraWrapperLock);

    ALOGV("%s: cam %s", __FUNCTION__, name);

    if (name != NULL) {
        if (check_vendor_module())
            return -EINVAL;

        if (num_cameras == 0) {
            num_cameras = gVendorModule->get_number_of_cameras();
            ALOGV("%s: %d cameras", __FUNCTION__, num_cameras);
        }

        /* On first invocation, prepare fixed_set_params block for all
           available cameras. */
        if (!fixed_set_params) {
            fixed_set_params = (char **) calloc(num_cameras, sizeof(char *));
            if (!fixed_set_params) {
                ALOGE("Global parameter memory allocation fail");
                rv = -ENOMEM;
                goto fail;
            }
        }

        camera_id = atoi(name);
        if (camera_id < 0 || camera_id > num_cameras) {
            ALOGE("Camera service provided camera_id out of bounds, "
                    "camera_id = %d, num supported = %d",
                    camera_id, num_cameras);
            rv = -EINVAL;
            goto fail;
        }

        camera_device = (wrapper_camera_device_t *)
                calloc(1, sizeof(*camera_device));
        if (!camera_device) {
            ALOGE("camera_device allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        camera_device->id = camera_id;

        rv = gVendorModule->common.methods->open(
                (const hw_module_t *) gVendorModule, name,
                (hw_device_t **) &(camera_device->vendor));
        if (rv) {
            ALOGE("Vendor camera open fail");
            goto fail;
        }
        ALOGV("%s: Got vendor camera device 0x%08X",
                __FUNCTION__, (uintptr_t)(camera_device->vendor));

        camera_ops = (camera_device_ops_t *) calloc(1, sizeof(*camera_ops));
        if (!camera_ops) {
            ALOGE("camera_ops allocation fail");
            rv = -ENOMEM;
            goto fail;
        }

        camera_device->base.common.tag = HARDWARE_DEVICE_TAG;
        camera_device->base.common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
        camera_device->base.common.module = (hw_module_t *)(module);
        camera_device->base.common.close = camera_device_close;
        camera_device->base.ops = camera_ops;

        camera_ops->set_preview_window = camera_set_preview_window;
        camera_ops->set_callbacks = camera_set_callbacks;
        camera_ops->enable_msg_type = camera_enable_msg_type;
        camera_ops->disable_msg_type = camera_disable_msg_type;
        camera_ops->msg_type_enabled = camera_msg_type_enabled;
        camera_ops->start_preview = camera_start_preview;
        camera_ops->stop_preview = camera_stop_preview;
        camera_ops->preview_enabled = camera_preview_enabled;
        camera_ops->store_meta_data_in_buffers = camera_store_meta_data_in_buffers;
        camera_ops->start_recording = camera_start_recording;
        camera_ops->stop_recording = camera_stop_recording;
        camera_ops->recording_enabled = camera_recording_enabled;
        camera_ops->release_recording_frame = camera_release_recording_frame;
        camera_ops->auto_focus = camera_auto_focus;
        camera_ops->cancel_auto_focus = camera_cancel_auto_focus;
        camera_ops->take_picture = camera_take_picture;
        camera_ops->cancel_picture = camera_cancel_picture;
        camera_ops->set_parameters = camera_set_parameters;
        camera_ops->get_parameters = camera_get_parameters;
        camera_ops->put_parameters = camera_put_parameters;
        camera_ops->send_command = camera_send_command;
        camera_ops->release = camera_release;
        camera_ops->dump = camera_dump;

        *device = &camera_device->base.common;
    }

    ALOGV("%s: ret %d", __FUNCTION__, rv);

    return rv;

fail:
    if (camera_device) {
        free(camera_device);
        camera_device = NULL;
    }
    if (camera_ops) {
        free(camera_ops);
        camera_ops = NULL;
    }
    *device = NULL;

    ALOGV("%s: ret %d", __FUNCTION__, rv);

    return rv;
}

static int camera_get_number_of_cameras(void)
{
    if (num_cameras == 0 && !check_vendor_module())
        num_cameras = gVendorModule->get_number_of_cameras();
    ALOGV("%s: %d", __FUNCTION__, num_cameras);
    return num_cameras;
}

static int camera_get_camera_info(int camera_id, struct camera_info *info)
{
    ALOGV("%s, cam %d", __FUNCTION__, camera_id);
    if (check_vendor_module())
        return 0;
    int ret = gVendorModule->get_camera_info(camera_id, info);
    ALOGV("%s: cam %d, facing %d, orient %d, version %d, ret %d",
        __FUNCTION__, camera_id, info->facing, info->orientation,
        info->device_version, ret);
    return ret;
}

static struct hw_module_methods_t camera_module_methods = {
    .open = camera_device_open,
};

camera_module_t HAL_MODULE_INFO_SYM = {
    .common = {
         .tag = HARDWARE_MODULE_TAG,
         .module_api_version = CAMERA_MODULE_API_VERSION_1_0,
         .hal_api_version = HARDWARE_HAL_API_VERSION,
         .id = CAMERA_HARDWARE_MODULE_ID,
         .name = "APQ8084 Camera Wrapper",
         .author = "The LineageOS Project",
         .methods = &camera_module_methods,
         .dso = NULL,
         .reserved = {0},
    },
    .get_number_of_cameras = camera_get_number_of_cameras,
    .get_camera_info = camera_get_camera_info,
    .set_callbacks = NULL,
    .get_vendor_tag_ops = NULL,
    .open_legacy = NULL,
    .set_torch_mode = NULL,
    .init = NULL,
    .reserved = {0},
};
