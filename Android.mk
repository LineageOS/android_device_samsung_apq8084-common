#
# Copyright (C) 2016 The Sayanogen Project
# Copyright (C) 2017 The LineageOS Project
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

LOCAL_PATH := $(call my-dir)

ifneq ($(filter kccat6 lentislte,$(TARGET_DEVICE)),)

include $(call all-subdir-makefiles,$(LOCAL_PATH))

include $(CLEAR_VARS)

ADSP_IMAGES := \
    adsp.b00 adsp.b01 adsp.b02 adsp.b03 adsp.b04 adsp.b05 adsp.b06 \
    adsp.b08 adsp.b09 adsp.b10 adsp.b11 adsp.b12 adsp.b13 adsp.mdt

ADSP_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(ADSP_IMAGES)))
$(ADSP_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "ADSP firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(ADSP_SYMLINKS)

DTCPIP_IMAGES := \
    dtcpip.b00 dtcpip.b01 dtcpip.b02 dtcpip.b03 dtcpip.mdt

DTCPIP_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(DTCPIP_IMAGES)))
$(DTCPIP_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "DTCPIP firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(DTCPIP_SYMLINKS)

SECSTOR_IMAGES := \
    sec_stor.b00 sec_stor.b01 sec_stor.b02 sec_stor.b03 sec_stor.mdt

SECSTOR_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(SECSTOR_IMAGES)))
$(SECSTOR_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Secstor firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SECSTOR_SYMLINKS)

SKM_IMAGES := \
    skm.b00 skm.b01 skm.b02 skm.b03 skm.mdt

SKM_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(SKM_IMAGES)))
$(SKM_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "SKM firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SKM_SYMLINKS)

SKMM_TA_IMAGES := \
    skmm_ta.b00 skmm_ta.b01 skmm_ta.b02 skmm_ta.b03 skmm_ta.mdt

SKMM_TA_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(SKMM_TA_IMAGES)))
$(SKMM_TA_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "SKMM firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SKMM_TA_SYMLINKS)

SSHDCPAP_IMAGES := \
    sshdcpap.b00 sshdcpap.b01 sshdcpap.b02 sshdcpap.b03 sshdcpap.mdt

SSHDCPAP_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(SSHDCPAP_IMAGES)))
$(SSHDCPAP_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "SSHDCPAP firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SSHDCPAP_SYMLINKS)

TIMA_IMAGES := \
    tima_atn.b00 tima_atn.b01 tima_atn.b02 tima_atn.b03 tima_atn.mdt \
    tima_key.b00 tima_key.b01 tima_key.b02 tima_key.b03 tima_key.mdt \
    tima_lkm.b00 tima_lkm.b01 tima_lkm.b02 tima_lkm.b03 tima_lkm.mdt \
    tima_pkm.b00 tima_pkm.b01 tima_pkm.b02 tima_pkm.b03 tima_pkm.mdt

TIMA_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(TIMA_IMAGES)))
$(TIMA_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Tima firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(TIMA_SYMLINKS)

TZ_CCM_IMAGES := \
    tz_ccm.b00 tz_ccm.b01 tz_ccm.b02 tz_ccm.b03 tz_ccm.mdt

TZ_CCM_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(TZ_CCM_IMAGES)))
$(TZ_CCM_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "TZ_CCM firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(TZ_CCM_SYMLINKS)

VENUS_IMAGES := \
    venus.b00 venus.b01 venus.b02 venus.b03 venus.b04 venus.mdt

VENUS_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(VENUS_IMAGES)))
$(VENUS_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Venus firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(VENUS_SYMLINKS)

WV_IMAGES := \
    widevine.b00 widevine.b01 widevine.b02 widevine.b03 widevine.mdt

WV_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(WV_IMAGES)))
$(WV_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Widevine firmware link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /firmware/image/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(WV_SYMLINKS)

include $(CLEAR_VARS)
SEC_LIB_FILES := \
	libprotobuf-cpp-full.so

SEC_LIB_SYMLINKS := $(addprefix $(TARGET_OUT)/lib/,$(notdir $(SEC_LIB_FILES)))
$(SEC_LIB_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "SEC LIB symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/lib/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SEC_LIB_SYMLINKS)

include $(CLEAR_VARS)
SEC_BIN_FILES := \
	ks

SEC_BIN_SYMLINKS := $(addprefix $(TARGET_OUT)/bin/,$(notdir $(SEC_BIN_FILES)))
$(SEC_BIN_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "SEC BIN symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/bin/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SEC_BIN_SYMLINKS)

include $(CLEAR_VARS)
ACBD_FILES := \
    Bluetooth_cal.acdb  General_cal.acdb  Global_cal.acdb  Handset_cal.acdb  Hdmi_cal.acdb  Headset_cal.acdb  Speaker_cal.acdb

ACBD_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/,$(notdir $(ACBD_FILES)))
$(ACBD_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "ACBD Audio Files: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/etc/acdbdata/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(ACBD_SYMLINKS)

# Create links for audcal data files
$(shell mkdir -p $(TARGET_OUT)/etc/firmware/wcd9320; \
	ln -sf /data/misc/audio/wcd9320_anc.bin \
		$(TARGET_OUT)/etc/firmware/wcd9320/wcd9320_anc.bin;\
	ln -sf /data/misc/audio/mbhc.bin \
		$(TARGET_OUT)/etc/firmware/wcd9320/wcd9320_mbhc.bin; \
	ln -sf /data/misc/audio/wcd9320_mad_audio.bin \
		$(TARGET_OUT)/etc/firmware/wcd9320/wcd9320_mad_audio.bin)

endif

include $(CLEAR_VARS)
BT_FW_FILES := \
	nvm_tlv.bin nvm_tlv_1.3.bin nvm_tlv_2.1.bin nvm_tlv_3.0.bin \
	rampatch_tlv.img rampatch_tlv_1.3.tlv rampatch_tlv_2.1.tlv \
	rampatch_tlv_3.0.tlv

BT_FW_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/firmware/,$(notdir $(BT_FW_FILES)))
$(BT_FW_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "BT FW symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/firmware/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(BT_FW_SYMLINKS)

include $(CLEAR_VARS)
CAMVERS_FILES := \
    F16UL_s5k2p2xx_module_info.xml F16US_imx240_module_info.xml

CAMVERS_SYMLINKS := $(addprefix $(TARGET_OUT_ETC)/,$(notdir $(CAMVERS_FILES)))
$(CAMVERS_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Camera module version link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/etc/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(CAMVERS_SYMLINKS)

include $(CLEAR_VARS)
CAMDATA_SYMLINK := $(TARGET_OUT)/cameradata
$(CAMDATA_SYMLINK): $(LOCAL_INSTALLED_MODULE)
	@echo "Cameradata symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/cameradata $@

ALL_DEFAULT_INSTALLED_MODULES += $(CAMDATA_SYMLINK)

include $(CLEAR_VARS)
CAMLIB_FILES := \
    F16UL_libchromatix_s5k2p2xx_default_video.so \
    F16UL_libchromatix_s5k2p2xx_golfshot.so \
    F16UL_libchromatix_s5k2p2xx_golfshot_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_liveshot_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_preview.so \
    F16UL_libchromatix_s5k2p2xx_hdr_preview_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_preview_lls_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_uhd_video.so \
    F16UL_libchromatix_s5k2p2xx_hdr_uhd_video_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_video.so \
    F16UL_libchromatix_s5k2p2xx_hdr_video_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_zslshot_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hdr_zslshot_lls_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hfr_1080p_b.so \
    F16UL_libchromatix_s5k2p2xx_hfr_1080p_b_cpp.so \
    F16UL_libchromatix_s5k2p2xx_hfr_120.so \
    F16UL_libchromatix_s5k2p2xx_hfr_120_cpp.so \
    F16UL_libchromatix_s5k2p2xx_liveshot_cpp.so \
    F16UL_libchromatix_s5k2p2xx_pip.so \
    F16UL_libchromatix_s5k2p2xx_preview.so \
    F16UL_libchromatix_s5k2p2xx_preview_cpp.so \
    F16UL_libchromatix_s5k2p2xx_preview_drama_cpp.so \
    F16UL_libchromatix_s5k2p2xx_preview_lls_cpp.so \
    F16UL_libchromatix_s5k2p2xx_preview_panorama_cpp.so \
    F16UL_libchromatix_s5k2p2xx_preview_pip_cpp.so \
    F16UL_libchromatix_s5k2p2xx_shotmode_preview.so \
    F16UL_libchromatix_s5k2p2xx_shotmode_preview_cpp.so \
    F16UL_libchromatix_s5k2p2xx_shotmode_zslshot_cpp.so \
    F16UL_libchromatix_s5k2p2xx_uhd_video.so \
    F16UL_libchromatix_s5k2p2xx_uhd_video_cpp.so \
    F16UL_libchromatix_s5k2p2xx_video_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_2_4_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_2_4_iso100_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_2_4_iso200_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_2_4_iso400_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_2_4_iso800_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_2_4_lls_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_iso100_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_iso200_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_iso400_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_iso800_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_lls_cpp.so \
    F16UL_libchromatix_s5k2p2xx_zslshot_pip_cpp.so \
    F16UL_libTsAe.so F16UL_libTsAf.so \
    F16UL_libTs_J_Accm.so F16UL_libTs_J_Awb.so \
    F16US_libchromatix_imx240_default_video.so \
    F16US_libchromatix_imx240_golfshot.so \
    F16US_libchromatix_imx240_golfshot_cpp.so \
    F16US_libchromatix_imx240_hdr_liveshot_cpp.so \
    F16US_libchromatix_imx240_hdr_preview.so \
    F16US_libchromatix_imx240_hdr_preview_cpp.so \
    F16US_libchromatix_imx240_hdr_preview_lls_cpp.so \
    F16US_libchromatix_imx240_hdr_uhd_video.so \
    F16US_libchromatix_imx240_hdr_uhd_video_cpp.so \
    F16US_libchromatix_imx240_hdr_video.so \
    F16US_libchromatix_imx240_hdr_video_cpp.so \
    F16US_libchromatix_imx240_hdr_zslshot_cpp.so \
    F16US_libchromatix_imx240_hdr_zslshot_lls_cpp.so \
    F16US_libchromatix_imx240_hfr_1080p_b.so \
    F16US_libchromatix_imx240_hfr_1080p_b_cpp.so \
    F16US_libchromatix_imx240_hfr_120.so \
    F16US_libchromatix_imx240_hfr_120_cpp.so \
    F16US_libchromatix_imx240_liveshot_cpp.so \
    F16US_libchromatix_imx240_pip.so \
    F16US_libchromatix_imx240_preview.so \
    F16US_libchromatix_imx240_preview_cpp.so \
    F16US_libchromatix_imx240_preview_drama_cpp.so \
    F16US_libchromatix_imx240_preview_lls_cpp.so \
    F16US_libchromatix_imx240_preview_panorama_cpp.so \
    F16US_libchromatix_imx240_preview_pip_cpp.so \
    F16US_libchromatix_imx240_shotmode_preview.so \
    F16US_libchromatix_imx240_shotmode_preview_cpp.so \
    F16US_libchromatix_imx240_shotmode_zslshot_cpp.so \
    F16US_libchromatix_imx240_uhd_video.so \
    F16US_libchromatix_imx240_uhd_video_cpp.so \
    F16US_libchromatix_imx240_video_cpp.so \
    F16US_libchromatix_imx240_zslshot_2_4_cpp.so \
    F16US_libchromatix_imx240_zslshot_2_4_iso100_cpp.so \
    F16US_libchromatix_imx240_zslshot_2_4_iso200_cpp.so \
    F16US_libchromatix_imx240_zslshot_2_4_iso400_cpp.so \
    F16US_libchromatix_imx240_zslshot_2_4_iso800_cpp.so \
    F16US_libchromatix_imx240_zslshot_2_4_lls_cpp.so \
    F16US_libchromatix_imx240_zslshot_cpp.so \
    F16US_libchromatix_imx240_zslshot_iso100_cpp.so \
    F16US_libchromatix_imx240_zslshot_iso200_cpp.so \
    F16US_libchromatix_imx240_zslshot_iso400_cpp.so \
    F16US_libchromatix_imx240_zslshot_iso800_cpp.so \
    F16US_libchromatix_imx240_zslshot_lls_cpp.so \
    F16US_libchromatix_imx240_zslshot_pip_cpp.so \
    F16US_libTsAe.so F16US_libTsAf.so \
    F16US_libTs_J_Accm.so F16US_libTs_J_Awb.so \
    libmmcamera_cac3_lib.so libmmcamera_fidelix_eeprom.so \
    libmmcamera_interface.so libmmcamera2_stats_algorithm.so \
    libTsAe.so libTsAf.so libTs_J_Accm.so libTs_J_Awb.so

CAMLIB_SYMLINKS := $(addprefix $(TARGET_OUT)/lib/,$(notdir $(CAMLIB_FILES)))
$(CAMLIB_SYMLINKS): $(LOCAL_INSTALLED_MODULE)
	@echo "Camera module version link: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/lib/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(CAMLIB_SYMLINKS)

include $(CLEAR_VARS)
QMUX_CONFIG_SYMLINK := $(TARGET_OUT_ETC)/data
$(QMUX_CONFIG_SYMLINK): $(LOCAL_INSTALLED_MODULE)
	@echo "qmuxd config dir symlink: $@"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf /system/vendor/etc/$(notdir $@) $@

ALL_DEFAULT_INSTALLED_MODULES += $(QMUX_CONFIG_SYMLINK)
