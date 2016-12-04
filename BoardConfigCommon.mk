#
# Copyright (C) 2016 The Sayanogen Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# inherit from qcom-common
-include device/samsung/qcom-common/BoardConfigCommon.mk

LOCAL_PATH := device/samsung/lentislte-common

# Architecture
TARGET_CPU_VARIANT := krait

# Audio
BOARD_USES_ALSA_AUDIO := true
AUDIO_FEATURE_ENABLED_FLUENCE := true
AUDIO_FEATURE_LOW_LATENCY_PRIMARY := true
AUDIO_FEATURE_ENABLED_HWDEP_CAL := true
AUDIO_FEATURE_ENABLED_LOW_LATENCY_CAPTURE := true
AUDIO_FEATURE_ENABLED_NEW_SAMPLE_RATE := true
AUDIO_FEATURE_ENABLED_ANC_HEADSET := true
BOARD_USES_ES705 := true
USE_CUSTOM_AUDIO_POLICY := 1

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_QCOM := true
BOARD_HAS_QCA_BT_ROME := true

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := APQ8084

# Camera
TARGET_USE_COMPAT_GRALLOC_ALIGN := true
TARGET_PROVIDES_CAMERA_HAL := true
USE_DEVICE_SPECIFIC_CAMERA := true

# Charger
BOARD_BATTERY_DEVICE_NAME := "battery"
BOARD_CHARGING_CMDLINE_NAME := "androidboot.mode"
BOARD_CHARGING_CMDLINE_VALUE := "charger"
BOARD_CHARGER_ENABLE_SUSPEND := true

# CMHW
BOARD_HARDWARE_CLASS += hardware/samsung/cmhw
BOARD_HARDWARE_CLASS += device/samsung/lentislte-common/cmhw

# Display
OVERRIDE_RS_DRIVER:= libRSDriver_adreno.so
MAX_EGL_CACHE_KEY_SIZE := 12*1024
MAX_EGL_CACHE_SIZE := 2048*1024
HAVE_ADRENO_SOURCE := false
USE_OPENGL_RENDERER := true

# Include an expanded selection of fonts
EXTENDED_FONT_FOOTPRINT := true

# Include path
TARGET_SPECIFIC_HEADER_PATH := $(LOCAL_PATH)/include

# Graphics
TARGET_BOOTANIMATION_MULTITHREAD_DECODE := true

# Display
SF_VSYNC_EVENT_PHASE_OFFSET_NS := 5000000
VSYNC_EVENT_PHASE_OFFSET_NS := 7500000

# Legacy BLOB Support
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true

# Lights
TARGET_PROVIDES_LIBLIGHT := true

# Media
TARGET_ENABLE_QC_AV_ENHANCEMENTS := true

# NFC
BOARD_NFC_CHIPSET := pn547

# Partitions
BOARD_FLASH_BLOCK_SIZE := 262144
TARGET_USERIMAGES_USE_EXT4 := true
TARGET_USERIMAGES_USE_F2FS := true

# Platform
TARGET_BOARD_PLATFORM := apq8084
TARGET_BOARD_PLATFORM_GPU := qcom-adreno420

# Power
TARGET_POWERHAL_VARIANT := qcom

# Qualcomm support
TARGET_USES_QCOM_BSP := true
TARGET_GLOBAL_CFLAGS += -DQCOM_BSP
TARGET_GLOBAL_CPPFLAGS += -DQCOM_BSP

# Radio
BOARD_RIL_CLASS := ../../../$(LOCAL_PATH)/ril

# Recovery
BOARD_CUSTOM_RECOVERY_KEYMAPPING := ../../device/samsung/lentislte-common/recovery/recovery_keys.c
TARGET_RECOVERY_FSTAB := $(LOCAL_PATH)/rootdir/etc/fstab.qcom
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_23x41.h\"
BOARD_USES_MMCUTILS := true
BOARD_HAS_LARGE_FILESYSTEM := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_HAS_NO_SELECT_BUTTON := true
BOARD_RECOVERY_SWIPE := true

# Sensors
TARGET_NO_SENSOR_PERMISSION_CHECK := true

# SELinux
include device/qcom/sepolicy/sepolicy.mk

BOARD_SEPOLICY_DIRS += \
    device/samsung/lentislte-common/sepolicy

# Time
BOARD_USES_QC_TIME_SERVICES := true

# Wifi
BOARD_HAS_QCOM_WLAN := true
BOARD_HAS_QCOM_WLAN_SDK := true
BOARD_WLAN_DEVICE := qcwcn
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_$(BOARD_WLAN_DEVICE)
WIFI_DRIVER_MODULE_NAME := "wlan"
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/wlan.ko"
WPA_SUPPLICANT_VERSION := VER_0_8_X
