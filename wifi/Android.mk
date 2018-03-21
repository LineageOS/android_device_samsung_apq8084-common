LOCAL_PATH:= $(call my-dir)

#----------------------------------------------------------------------
# Copy additional target-specific files
#----------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE       := wpa_supplicant_overlay.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/wifi
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := p2p_supplicant_overlay.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/wifi
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := hostapd_default.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/hostapd
LOCAL_SRC_FILES    := hostapd.conf
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := hostapd.accept
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/hostapd
LOCAL_SRC_FILES    := hostapd.accept
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := hostapd.deny
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)/hostapd
LOCAL_SRC_FILES    := hostapd.deny
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
SEC_WIFI_FILES := \
	WCNSS_cfg.dat WCNSS_qcom_cfg.ini WCNSS_qcom_wlan_nv.bin

SEC_WIFI_SYMLINKS := $(addprefix $(TARGET_OUT)/etc/firmware/wlan/qca_cld/,$(notdir $(SEC_WIFI_FILES)))
$(SEC_WIFI_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "WIFI symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/firmware/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SEC_WIFI_SYMLINKS)

include $(CLEAR_VARS)
SEC_PERSIST_FILES := \
	wlan_mac.bin

SEC_PERSIST_SYMLINKS := $(addprefix $(TARGET_OUT)/etc/firmware/wlan/qca_cld/,$(notdir $(SEC_PERSIST_FILES)))
$(SEC_PERSIST_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "WIFI PERSIST symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /persist/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SEC_PERSIST_SYMLINKS)
