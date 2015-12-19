/*
 * Copyright (C) 2014, The CyanogenMod Project
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
*
*/

#define LOG_NDEBUG 0

#define LOG_TAG "CameraWrapper"

#include "CameraHardwareInterface.h"
#include <cutils/log.h>
#include <utils/threads.h>
#include <utils/String8.h>
#include <hardware/hardware.h>
#include <hardware/camera.h>
#include <camera/Camera.h>
#include <camera/CameraParameters.h>
#include <dlfcn.h>
#include <stdio.h>

static android::Mutex gCameraWrapperLock;
static camera_module_t *gVendorModule = 0;

static char **fixed_set_params = NULL;

static int camera_device_open(const hw_module_t *module, const char *name,
                hw_device_t **device);
static int camera_device_close(hw_device_t *device);
static int camera_get_number_of_cameras(void);
static int camera_get_camera_info(int camera_id, struct camera_info *info);

static camera_data_callback org_data_cb;

static void*  g_hNSMemMgr = 0;
static void*  membuffer = 0;
static void*  g_hEnhancer = 0;
static void*  JPEGbuffer;

struct PicInfo{
   int format;
   int width;
   int height;
   void *pYUVdata;
   void *pYuv2ndpart; //=pYUVdata+width*height.
   int nothing;
   int nothing2;
   int width2;
   int width3;
   int nothing3;
   int nothing4;
};

struct tPicSummary{
   int numberofpics;
   int minusone;
   void *Picinfo0;
   void *Picinfo1;
   void *Picinfo2;
   void *Picinfo3;
} PicSummary;

struct tJPEGinfo{
   void *pYUVdata;
   int YUVsize;
   int nothing;
   void *somecode;
   int width;
   int height;
   int JPEGformat;
   void *destbuf;
   int YUVsize2;
   int quality;
   int nothing1;
   int JPEGsize;
   int nothing3;
   int nothing4;
} JPEGinfo;

static struct camera_device *current_camera_device;

static PicInfo sourcepic[6]; 
static void *destbuffer;
static PicInfo destpic;
static int AMNC_params[12]; 
static int lightcondition = 0;
static bool lowlightphotoinprogress = false;
typedef void* MHandle;
typedef void* (*pfMMemMgrCreate)(void*, long);
typedef void (*pMMemMgrDestroy)(MHandle);
typedef int (*pAMNC_Init)(MHandle, MHandle*);
typedef int (*pAMNC_Uninit)(MHandle*);
typedef int (*pAMNC_GetDefaultParam)(void*);
typedef int (*pAMNC_Enhancement) (void*, void*, void*, void*, int, int);
typedef int (*pjpegencoder) (void *, void*);
typedef void* (*pjpegencoderInit)(void*, int);

static pfMMemMgrCreate MMemMgrCreate;
static pMMemMgrDestroy MMemMgrDestroy;
static pAMNC_Init AMNC_Init;
static pAMNC_Uninit AMNC_Uninit;
static pAMNC_GetDefaultParam AMNC_GetDefaultParam;
static pAMNC_Enhancement AMNC_Enhancement;
static pjpegencoderInit jpegencoderInit;
static pjpegencoder jpegencoder;

static int devrotation;
static int picwidth;
static int picheight;
static int idx = 0;
static int CurrentCamId;
static void *arcsoftlib;
static void * secjpeginterface;
static bool CMcamapp = false;

static struct hw_module_methods_t camera_module_methods = {
    .open = camera_device_open
};

camera_module_t HAL_MODULE_INFO_SYM = {
    .common = {
         .tag = HARDWARE_MODULE_TAG,
         .module_api_version = CAMERA_MODULE_API_VERSION_1_0,
         .hal_api_version = HARDWARE_HAL_API_VERSION,
         .id = CAMERA_HARDWARE_MODULE_ID,
         .name = "Lentislte Camera Wrapper",
         .author = "The CyanogenMod Project",
         .methods = &camera_module_methods,
         .dso = NULL, /* remove compilation warnings */
         .reserved = {0}, /* remove compilation warnings */
    },
    .get_number_of_cameras = camera_get_number_of_cameras,
    .get_camera_info = camera_get_camera_info,
    .set_callbacks = NULL, /* remove compilation warnings */
    .get_vendor_tag_ops = NULL, /* remove compilation warnings */
    .open_legacy = NULL, /* remove compilation warnings */
    .set_torch_mode = NULL, /* remove compilation warnings */
    .init = NULL, /* remove compilation warnings */
    .reserved = {0}, /* remove compilation warnings */
};

typedef struct wrapper_camera_device {
    camera_device_t base;
    int id;
    camera_device_t *vendor;
} wrapper_camera_device_t;

#define VENDOR_CALL(device, func, ...) ({ \
    wrapper_camera_device_t *__wrapper_dev = (wrapper_camera_device_t*) device; \
    __wrapper_dev->vendor->ops->func(__wrapper_dev->vendor, ##__VA_ARGS__); \
})

#define CAMERA_ID(device) (((wrapper_camera_device_t *)(device))->id)

static int check_vendor_module()
{
    int rv = 0;
    ALOGV("%s", __FUNCTION__);

    if (gVendorModule)
        return 0;

    rv = hw_get_module_by_class("camera", "vendor",
            (const hw_module_t **)&gVendorModule);

    if (rv)
        ALOGE("failed to open vendor camera module");
    return rv;
}

static bool is4kVideo(android::CameraParameters &params) {
    int video_width, video_height;
    params.getVideoSize(&video_width, &video_height);
    ALOGV("%s : VideoSize is %x", __FUNCTION__, video_width*video_height);
    return video_width*video_height > 1920*1080;
}

static bool is480Preview(android::CameraParameters &params) {
    int video_width, video_height;
    params.getPreviewSize(&video_width, &video_height);
    ALOGV("%s : PreviewSize is %x", __FUNCTION__, video_width*video_height);
    return video_width*video_height == 720*480;
}

static char *camera_fixup_getparams(int __attribute__((unused)) id,
    const char *settings)
{
    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#if !LOG_NDEBUG
    ALOGV("%s: Original parameters:", __FUNCTION__);
    params.dump();
#endif

    //Hide nv12-venus from Android.
    if (strcmp (params.getPreviewFormat(), "nv12-venus") == 0)
          params.set("preview-format", "yuv420sp");

    int minfps, maxfps;
    params.getPreviewFpsRange(&minfps, &maxfps);
    if (minfps >= 60000)
    {
	params.set("preview-frame-rate-values", "15,24,30,60,120");
        if (minfps >= 120000)
	{
	     params.set("fast-fps-mode", "2");
             params.set("preview-frame-rate", "120");
	}
	else
	{
	     params.set("fast-fps-mode", "1");
	     params.set("preview-frame-rate", "60");
	}
    }

    android::String8 strParams = params.flatten();
    char *ret = strdup(strParams.string());

#if !LOG_NDEBUG
    ALOGV("%s: fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    return ret;
}

static int camera_send_command(struct camera_device *device,
        int32_t cmd, int32_t arg1, int32_t arg2)
{
    ALOGV("%s->%08X->%08X command :%d ", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor), cmd);

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, send_command, cmd, arg1, arg2);
}


static char *camera_fixup_setparams(int id, const char *settings, camera_device *device)
{
    android::CameraParameters params;
    params.unflatten(android::String8(settings));

#if !LOG_NDEBUG
    ALOGV("%s: original parameters:", __FUNCTION__);
    params.dump();
#endif

    if (is480Preview(params)) {
        ALOGV("%s: 480p preview detected, switching preview format to yuv420p", __FUNCTION__);
        params.set("preview-format", "yuv420p");
    }

    int minfps, maxfps;
    params.getPreviewFpsRange(&minfps, &maxfps);
    if (minfps >= 60000)
    {
	params.set("preview-frame-rate-values", "15,24,30,60,120");
        if (minfps >= 120000)
	{
	     params.set("fast-fps-mode", "2");
             params.set("preview-frame-rate", "120");
	}
	else
	{
	     params.set("fast-fps-mode", "1");
	     params.set("preview-frame-rate", "60");
	}
    }

    if (strcmp (params.get("jpeg-quality"), "18") == 0)
    {
        ALOGI ("Special jpegquality command received from CM cam app. Camid: %d", CurrentCamId);
        params.set("jpeg-quality", "96");

       if ((CMcamapp == false) && (CurrentCamId == 0))  //Only back camera does low light photo mode.
       {
   	   ALOGI("CameraId = 0, send init vendor commands for lowlight mode");
           camera_send_command(device, 1515, 1, 0);
	   camera_send_command(device, 1508, 0, 0);//Samsung_camera
           camera_send_command(device, 1521, 0, 0); //Device orientation
           camera_send_command(device, 1515, 1, 0);
    	   camera_send_command(device, 1334, 0, 0); //Drama shot mode
           camera_send_command(device, 1801, 1, 0);
       	   camera_send_command(device, 1351, 1, 0); //Auto Low Light 
           CMcamapp = true;
        }
    }

    android::String8 strParams = params.flatten();

    if (fixed_set_params[id])
        free(fixed_set_params[id]);
    fixed_set_params[id] = strdup(strParams.string());
    char *ret = fixed_set_params[id];

#if !LOG_NDEBUG
    ALOGV("%s: fixed parameters:", __FUNCTION__);
    params.dump();
#endif

    return ret;
}

/*******************************************************************
 * implementation of camera_device_ops functions
 *******************************************************************/
static char *camera_get_parameters(struct camera_device *device);
static int camera_set_parameters(struct camera_device *device, const char *params);
static int camera_set_preview_window(struct camera_device *device,
        struct preview_stream_ops *window)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, set_preview_window, window);
}


static void mydataCallback(int32_t msg_type,
                          const camera_memory_t *data, unsigned int index,
                          camera_frame_metadata_t *metadata,
                          void *user)
{
    ALOGI("ThisisMyDataCallback ! msg_type: %d", msg_type);

    if (msg_type == CAMERA_MSG_PREVIEW_METADATA)   
    {
       lightcondition = *((int*)metadata+5);
       ALOGI("Lightcondition: %i", lightcondition);
    }

    bool ready = false;

    if ((msg_type == CAMERA_MSG_COMPRESSED_IMAGE) && (lowlightphotoinprogress == true) )
    {
        ALOGI("CAMERA_MSG_COMPRESSED_IMAGE, idx: %i", idx);
        android::sp<android::IMemoryHeap> heap  = 0;
        android::sp<android::CameraHardwareInterface::CameraHeapMemory> mem(static_cast<android::CameraHardwareInterface::CameraHeapMemory *>(data->handle));
        android::sp<android::IMemory> dataPtr = mem->mBuffers[index];
        ssize_t         offset = 0;
        size_t          size = 0;

        if (dataPtr.get())
        {
            heap = dataPtr->getMemory(&offset, &size);  //size = 23842816, offset 0

            sourcepic[idx].pYUVdata = operator new(size);

            if(sourcepic[idx].pYUVdata==NULL)                     
               ALOGE("Error! Destination memory not allocated.");
            else
               ALOGI("Destination Memory successfully allocated.");
                
            if  (NULL != heap.get() && NULL != heap->base())
            {
               ALOGI("Copying picture data now from the heap to our buffer");
               ::memcpy(sourcepic[idx].pYUVdata, ((uint8_t*)heap->base()) , size);  //Copy van heap mem naar onze buffer !
            }
            else
               ALOGE("Error ! Serious problem with heap memory !!");

            sourcepic[idx].format = 2050; //YUV NV21
            sourcepic[idx].width = picwidth; //0x14C0;
            sourcepic[idx].height = picheight; //0xBAC;
            sourcepic[idx].pYuv2ndpart= sourcepic[idx].pYUVdata + (picwidth*picheight);// + 0xF23100;
            sourcepic[idx].nothing = 0;
            sourcepic[idx].nothing2 = 0;
            sourcepic[idx].width2 = picwidth; //0x14C0;
            sourcepic[idx].width3 = picwidth; //0x14C0;
            sourcepic[idx].nothing3 = 0;
            sourcepic[idx].nothing4 = 0;
                
            if (idx==5)
            {
                ALOGI("Going to make the final Nightshot photo now");
                AMNC_GetDefaultParam(&AMNC_params);
                PicSummary.numberofpics = 4;
                PicSummary.minusone = -1;
                //Don't use the first 2 pics, because of timing issue's.
                PicSummary.Picinfo0 = &sourcepic[2];
                PicSummary.Picinfo1 = &sourcepic[3];
                PicSummary.Picinfo2 = &sourcepic[4];
                PicSummary.Picinfo3 = &sourcepic[5];

                if (AMNC_Enhancement (g_hEnhancer, &PicSummary, &destpic, &AMNC_params, 0, 0))
                   ALOGE("error AMNC_Enhancement!" );
                else
                   ALOGI("Succesful AMNC_Enhancement!");

                //Allocate JPEG destination buffer;
                JPEGbuffer = operator new(0x16b498f);//fixme: make size dependent on quality and photo dimensions. Currently max size.
                JPEGinfo.pYUVdata = destpic.pYUVdata;
                JPEGinfo.YUVsize= picwidth*picheight*1.5; //0x16B4980;
                JPEGinfo.nothing = 0;
                JPEGinfo.destbuf = JPEGbuffer;
                JPEGinfo.height = picheight;
                JPEGinfo.width = picwidth; 
                JPEGinfo.JPEGformat = 2;
                JPEGinfo.YUVsize2 =picwidth*picheight*1.5; //0x16B4980;
                JPEGinfo.quality = 96;
                JPEGinfo.nothing1 = 0;
                JPEGinfo.JPEGsize = 0;
                JPEGinfo.nothing3 = 0;
                JPEGinfo.nothing4 = 0;

                secjpeginterface = dlopen("libsecjpeginterface.so", RTLD_NOW);
                if (!secjpeginterface)
                   ALOGE("Error Could not load libsecjpeginterface.so: %s \n", dlerror());
                else
                   ALOGI("libsecjpeginterface.so succesfully loaded !!");
    
                jpegencoderInit = NULL;
                jpegencoderInit = (pjpegencoderInit) dlsym(secjpeginterface, "_ZN7android14SecJpegEncoder6createEPNS_12jpeg_encoderENS_12SecJpegCodec4TYPEE");
                if (jpegencoderInit == NULL)
                    ALOGE("_ZN7android14SecJpegEncoder6createEPNS_12jpeg_encoderENS_12SecJpegCodec4TYPEE not loaded!");
    
                //Get pointer to EncoderObject
                void * enc_p = jpegencoderInit(&JPEGinfo, 0);

                int v24;
                if ( (**(int (***)(void))enc_p)() == 1 )  //Hier called ie de deferenced pointer van Encoder object die 0 (software) of 1 (=hardware) teruggeeft
                   ALOGI("Hardware encoder !");
                else
                   ALOGI("Software encoder !");

                jpegencoder = NULL;
                jpegencoder = (pjpegencoder) dlsym(secjpeginterface, "_ZN7android14SecJpegEncoder15startEncodeSyncERNS_12jpeg_encoderE");

                if (jpegencoder == NULL)
                   ALOGE("_ZN7android14SecJpegEncoder15startEncodeSyncERNS_12jpeg_encoderE not loaded!");
 
                if (jpegencoder (enc_p, &JPEGinfo) == 0)
                   ALOGI("Succesfully encoded picture to jpeg !");
                else
                   ALOGE("Error encoded picture to jpeg !");

                if (devrotation == 90)
                {
                   ALOGI("Modifying EXIF info to reflect rotation!");
                   char * jp = (char*)JPEGinfo.destbuf;
                   jp[0x22]= 0x01;
                   jp[0x23]= 0x12;
                   jp[0x24]= 0x00;
                   jp[0x25]= 0x03;
                   jp[0x26]= 0x00;
                   jp[0x27]= 0x00;
                   jp[0x28]= 0x00;
                   jp[0x29]= 0x01;
                   jp[0x2a]= 0x00;
                   jp[0x2b]= 0x06;
                   jp[0x2c]= 0x00;
                   jp[0x2c]= 0x00;
                }

                ALOGI("Resulting JPEG size: %i", JPEGinfo.JPEGsize);

                //And clean it all up now.
                if(g_hEnhancer != 0)
                {
                    ALOGE("Destroying g_hEnhancer");
                    if (AMNC_Uninit(&g_hEnhancer))
          	       ALOGE("NightShot AMNC_Uninit error");  
                    else
	               ALOGI("NightShot AMNC_Uninit success");
	 	    g_hEnhancer = 0;
    	        }
 
           	if (g_hNSMemMgr)
                {
                    ALOGI("Destroying g_hNSMemMgr");
      		    MMemMgrDestroy(g_hNSMemMgr);
		    g_hNSMemMgr = 0;
	        }

                if (NULL != heap.get() && NULL != heap->base())
                {
                    android::sp<android::MemoryBase> image = new android::MemoryBase(heap, 0, JPEGinfo.JPEGsize);
                    ::memcpy( ((uint8_t*)heap->base()), JPEGinfo.destbuf , JPEGinfo.JPEGsize);  //Copy van heap mem naar onze buffer !

                   //release buffers
                   operator delete(membuffer);               //mmemmgr
                   for (int i = 0; i < 6; i++)
                      operator delete(sourcepic[i].pYUVdata);   //Release all burst photo buffers
                   operator delete(destbuffer);             //Release combined YUV picture
                   operator delete(JPEGbuffer); // Release final jpeg buffer
                   ALOGI("Going to call compressed picture callback now with Lowlight photo");
                   lowlightphotoinprogress = false;
                   ready = true;
                }
            }
            else
               idx++;
        } //   if (dataPtr.get())
   }  //if ((msg_type == CAMERA_MSG_COMPRESSED_IMAGE)
   else
       org_data_cb(msg_type, data, index, metadata, user);


   if (ready == true)
   {
       idx = 0;
       ALOGI ("Resetting idx. %d", idx);
       org_data_cb(msg_type, data, index, metadata, user);

       ALOGI("Camera is in Lowlightmode, so now sending the post picture vendor commands");
       camera_send_command(current_camera_device, 0x5ED, 0, 0);  //CAMERA_CMD_SET_STOP_SERIES_CAPTURE
       camera_send_command(current_camera_device, 0x5EE, 0, 0);  
       camera_send_command(current_camera_device, 1264, 0, 0);  
       camera_send_command(current_camera_device, 1515, 1, 0); 
       camera_send_command(current_camera_device, 1334, 0, 0);  //DRAMA_SHOT_MODE
       camera_send_command(current_camera_device, 1801, 1, 0);  
       camera_send_command(current_camera_device, 1351, 1, 0);  

       android::CameraParameters parameters;
       parameters.unflatten(android::String8(camera_get_parameters(current_camera_device)));
       parameters.set("auto-exposure-lock", "false");
       parameters.set("auto-whitebalance-lock", "false");
       camera_set_parameters(current_camera_device, strdup(parameters.flatten().string()));

   }

   ALOGI("End MyCallback!!!!!");
}



static void camera_set_callbacks(struct camera_device *device,
        camera_notify_callback notify_cb,
        camera_data_callback data_cb,
        camera_data_timestamp_callback data_cb_timestamp,
        camera_request_memory get_memory,
        void *user)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    org_data_cb = data_cb;

    VENDOR_CALL(device, set_callbacks, notify_cb,(camera_data_callback) mydataCallback, data_cb_timestamp, get_memory, user);
}

static void camera_enable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, enable_msg_type, msg_type);
}

static void camera_disable_msg_type(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, disable_msg_type, msg_type);
}

static int camera_msg_type_enabled(struct camera_device *device,
        int32_t msg_type)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return 0;

    return VENDOR_CALL(device, msg_type_enabled, msg_type);
}

static int camera_start_preview(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, start_preview);
}

static void camera_stop_preview(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, stop_preview);
}

static int camera_preview_enabled(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, preview_enabled);
}

static int camera_store_meta_data_in_buffers(struct camera_device *device,
        int enable)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, store_meta_data_in_buffers, enable);
}

static int camera_start_recording(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return EINVAL;

    if ((CurrentCamId == 0) && (CMcamapp == true))
    {
        ALOGI("Sending start_recording vendor commands");
        camera_send_command(device, 1515, 1, 0); // 
        camera_send_command(device, 1334, 0, 0);
        camera_send_command(device, 1801, 0, 0); //LIGHT_CONDITION_ENABLE naar HAL. 
        camera_send_command(device, 1351, 0, 0); 
        camera_send_command(device, 1264, 0, 0); //LOW_LIGHT_SHOT_SET 
    }

    android::CameraParameters parameters;
    parameters.unflatten(android::String8(camera_get_parameters(device)));
    parameters.set("dis", "disable");
    parameters.set("zsl", "off");

    if (is4kVideo(parameters)) {
        ALOGV("%s: UHD detected, switching preview-format to nv12-venus", __FUNCTION__);
        parameters.set("preview-format", "nv12-venus");
    }

    camera_set_parameters(device, strdup(parameters.flatten().string()));

    android::CameraParameters parameters2;
    parameters2.unflatten(android::String8(VENDOR_CALL(device, get_parameters)));
    parameters2.dump();

    return VENDOR_CALL(device, start_recording);
}

static void camera_stop_recording(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    if ((CurrentCamId == 0) && (CMcamapp == true))
    {
       ALOGI("stopRecording: Sending post record vendor commands");
       camera_send_command(device, 1515, 1, 0);
       camera_send_command(device, 1334, 0, 0); //drama mode
       camera_send_command(device, 1801, 1, 0); //LIGHT_CONDITION_ENABLE naar HAL.
       camera_send_command(device, 1351, 1, 0);
    }

    VENDOR_CALL(device, stop_recording);
}

static int camera_recording_enabled(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, recording_enabled);
}

static void camera_release_recording_frame(struct camera_device *device,
        const void *opaque)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, release_recording_frame, opaque);
}

static int camera_auto_focus(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, auto_focus);
}

static int camera_cancel_auto_focus(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_auto_focus);
}

static int camera_take_picture(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    current_camera_device = device;

    if ((lightcondition == 4) && (CurrentCamId == 0) && (CMcamapp == true))
    {
       ALOGI("changeToLowLightShot");

       lowlightphotoinprogress = true;

       camera_send_command(device, 1515, 0, 0);
       camera_send_command(device, 1334, 0, 0); //DRAMA_SHOT_MODE to HAL
       camera_send_command(device, 0x5EE, 1, 0); //1264, to hAL
       camera_send_command(device, 1264, 1, 0); //LOW_LIGHT_SHOT_SET

       android::CameraParameters parameters;
       parameters.unflatten(android::String8(camera_get_parameters(device)));
       parameters.set("auto-exposure-lock", "true");
       parameters.set("auto-whitebalance-lock", "true");
       parameters.getPictureSize (&picwidth, &picheight);
       devrotation = parameters.getInt("rotation");
       ALOGI("picdim: %i %i", picwidth, picheight);

       camera_set_parameters(device, strdup(parameters.flatten().string()));

       membuffer = operator new(0x1E00000);

       arcsoftlib= dlopen("libarcsoft_nightportrait.so", RTLD_NOW);
       if (!arcsoftlib)
       {
           ALOGE("Could not load libarcsoft_nightportrait.so: %s \n", dlerror());
           return -1;
       }

       MMemMgrCreate = (pfMMemMgrCreate) dlsym(arcsoftlib, "MMemMgrCreate");
       MMemMgrDestroy = (pMMemMgrDestroy) dlsym(arcsoftlib, "MMemMgrDestroy");
       AMNC_Init = (pAMNC_Init) dlsym(arcsoftlib, "AMNC_Init");
       AMNC_Uninit = (pAMNC_Uninit) dlsym(arcsoftlib, "AMNC_Uninit");
       AMNC_GetDefaultParam = (pAMNC_GetDefaultParam) dlsym(arcsoftlib, "AMNC_GetDefaultParam");
       AMNC_Enhancement = (pAMNC_Enhancement) dlsym(arcsoftlib, "AMNC_Enhancement");

       //And use the just created functions.
       g_hNSMemMgr = MMemMgrCreate (membuffer, 0x1E00000);

       if(0 == membuffer || 0 == g_hNSMemMgr)
       {
	   ALOGE("NightShot memory init is null");
   	   return -1;
       }

       if (AMNC_Init (g_hNSMemMgr, &g_hEnhancer))
       {
	   ALOGE("NightShot AMNC_Init error !");
           return -1;
       }
       else
   	   ALOGI("NightShot AMNC_Init success");

       destbuffer = operator new(0x16BD000);

       if(destbuffer==NULL)
          ALOGE("Error! destbuf mem not allocated.");
       else
          ALOGI("Destbuf memory got allocated.");

       destpic.format = 2050; //YUV NV21
       destpic.width = picwidth; //0x14C0;
       destpic.height = picheight; //0xBAC;
       destpic.pYUVdata= destbuffer;
       destpic.pYuv2ndpart = destbuffer + (picwidth*picheight); //+ 0xF23100;
       destpic.nothing = 0;
       destpic.nothing2 = 0;
       destpic.width2 = picwidth; //0x14C0;
       destpic.width3 = picwidth; //0x14C0;
       destpic.nothing3 = 0;
       destpic.nothing4 = 0;

       camera_send_command(device, 0x5EC, 6, 0);
    }

    return VENDOR_CALL(device, take_picture);
}

static int camera_cancel_picture(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, cancel_picture);
}

static int camera_set_parameters(struct camera_device *device, const char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    char *tmp = NULL;
    tmp = camera_fixup_setparams(CAMERA_ID(device), params, device);

    int ret = VENDOR_CALL(device, set_parameters, tmp);
    return ret;
}

static char *camera_get_parameters(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return NULL;

    char *params = VENDOR_CALL(device, get_parameters);

    char *tmp = camera_fixup_getparams(CAMERA_ID(device), params);
    VENDOR_CALL(device, put_parameters, params);
    params = tmp;

    return params;
}

static void camera_put_parameters(struct camera_device *device, char *params)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (params)
        free(params);
}


static void camera_release(struct camera_device *device)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return;

    VENDOR_CALL(device, release);
}

static int camera_dump(struct camera_device *device, int fd)
{
    ALOGV("%s->%08X->%08X", __FUNCTION__, (uintptr_t)device,
            (uintptr_t)(((wrapper_camera_device_t*)device)->vendor));

    if (!device)
        return -EINVAL;

    return VENDOR_CALL(device, dump, fd);
}

extern "C" void heaptracker_free_leaked_memory(void);

static int camera_device_close(hw_device_t *device)
{
    int ret = 0;
    wrapper_camera_device_t *wrapper_dev = NULL;

    ALOGV("%s", __FUNCTION__);

    android::Mutex::Autolock lock(gCameraWrapperLock);

    if (!device) {
        ret = -EINVAL;
        goto done;
    }

    for (int i = 0; i < camera_get_number_of_cameras(); i++) {
        if (fixed_set_params[i])
            free(fixed_set_params[i]);
    }

    wrapper_dev = (wrapper_camera_device_t*) device;

    wrapper_dev->vendor->common.close((hw_device_t*)wrapper_dev->vendor);
    if (wrapper_dev->base.ops)
        free(wrapper_dev->base.ops);
    free(wrapper_dev);
done:
#ifdef HEAPTRACKER
    heaptracker_free_leaked_memory();
#endif
    return ret;
}

/*******************************************************************
 * implementation of camera_module functions
 *******************************************************************/

/* open device handle to one of the cameras
 *
 * assume camera service will keep singleton of each camera
 * so this function will always only be called once per camera instance
 */

static int camera_device_open(const hw_module_t *module, const char *name,
        hw_device_t **device)
{
    CMcamapp = false;
    int rv = 0;
    int num_cameras = 0;
    int cameraid;
    wrapper_camera_device_t *camera_device = NULL;
    camera_device_ops_t *camera_ops = NULL;

    android::Mutex::Autolock lock(gCameraWrapperLock);

    ALOGI("camera_device open");

    if (name != NULL) {
        if (check_vendor_module())
            return -EINVAL;

        cameraid = atoi(name);
        num_cameras = gVendorModule->get_number_of_cameras();

        fixed_set_params = (char **) malloc(sizeof(char *) * num_cameras);
        if (!fixed_set_params) {
            ALOGE("parameter memory allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        memset(fixed_set_params, 0, sizeof(char *) * num_cameras);

        if (cameraid > num_cameras) {
            ALOGE("camera service provided cameraid out of bounds, "
                    "cameraid = %d, num supported = %d",
                    cameraid, num_cameras);
            rv = -EINVAL;
            goto fail;
        }

        camera_device = (wrapper_camera_device_t*)malloc(sizeof(*camera_device));
        if (!camera_device) {
            ALOGE("camera_device allocation fail");
            rv = -ENOMEM;
            goto fail;
        }
        memset(camera_device, 0, sizeof(*camera_device));
        camera_device->id = cameraid;

        CurrentCamId = cameraid;

        rv = gVendorModule->common.methods->open(
                (const hw_module_t*)gVendorModule, name,
                (hw_device_t**)&(camera_device->vendor));
        if (rv) {
            ALOGE("vendor camera open fail");
            goto fail;
        }
        ALOGV("%s: got vendor camera device 0x%08X",
                __FUNCTION__, (uintptr_t)(camera_device->vendor));

        camera_ops = (camera_device_ops_t*)malloc(sizeof(*camera_ops));
        if (!camera_ops) {
            ALOGE("camera_ops allocation fail");
            rv = -ENOMEM;
            goto fail;
        }

        memset(camera_ops, 0, sizeof(*camera_ops));

        camera_device->base.common.tag = HARDWARE_DEVICE_TAG;
        camera_device->base.common.version = CAMERA_DEVICE_API_VERSION_1_0;
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
    return rv;
}

static int camera_get_number_of_cameras(void)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_number_of_cameras();
}

static int camera_get_camera_info(int camera_id, struct camera_info *info)
{
    ALOGV("%s", __FUNCTION__);
    if (check_vendor_module())
        return 0;
    return gVendorModule->get_camera_info(camera_id, info);
}
