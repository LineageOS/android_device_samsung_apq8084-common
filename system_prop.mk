# Art
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.dex2oat-swap=false

# API
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.first_api_level=19

# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    af.fast_track_multiplier=1 \
    audio_hal.period_size=192 \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicerec=false \
    ro.qc.sdk.audio.fluencetype=fluence \
    use.voice.path.for.pcm.voip=false \
    use.dedicated.device.for.voip=true \
    audio.offload.buffer.size.kb=32 \
    audio.offload.gapless.enabled=false \
    media.stagefright.legacyencoder=true \
    media.stagefright.less-secure=true

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    bt.max.hfpclient.connections=1 \
    qcom.bluetooth.soc=rome

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    camera2.portability.force_api=1

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    persist.hwc.mdpcomp.enable=false \
    ro.hdcp2.rx=tz \
    ro.qualcomm.cabl=1 \
    ro.secwvk=144 \
    debug.egl.hw=1 \
    debug.sf.hw=1 \
    debug.gralloc.enable_fb_ubwc=1 \
    persist.hwc.enable_vds \
    sdm.perf_hint_window=50 \
    ro.opengles.version=196609 \
    persist.debug.wfd.enable=1 \
    persist.sys.wfd.virtual=0

# Fingerprint
PRODUCT_PROPERTY_OVERRIDES += \
    fingerprint_enabled=1

# GPS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.gps.qc_nlp_in_use=0 \
    ro.gps.agps_provider=1

# NFC
PRODUCT_PROPERTY_OVERRIDES += \
    ro.nfc.port=I2C

# QCOM Vendor
PRODUCT_PROPERTY_OVERRIDES += \
    persist.data.qmi.adb_logmask=0 \
    ro.vendor.extension_library=/vendor/lib/libqc-opt.so

# Radio
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libargs=-d /dev/smd0 \
    rild.libpath=/system/lib/libsec-ril.so \
    ro.ril.telephony.mqanelements=6 \
    ro.telephony.ril_class=apq8084RIL \
    persist.radio.apm_mdm_not_pwdn=1 \
    persist.radio.apm_sim_not_pwdn=1 \
    ro.use_data_netmgrd=false \
    persist.radio.add_power_save=1 \
    persist.rmnet.data.enable=true \
    persist.data.wda.enable=true \
    persist.data.df.dl_mode=5 \
    persist.data.df.ul_mode=5 \
    persist.data.df.agg.dl_pkt=10 \
    persist.data.df.agg.dl_size=4096 \
    persist.data.df.mux_count=8 \
    persist.data.df.iwlan_mux=9 \
    persist.data.df.dev_name=rmnet_usb0 \
    persist.data.llf.enable=true \
    persist.net.doxlat=true \
    ro.telephony.mms_data_profile=5

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sensors=1

# Storage
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.sdcardfs=true

# Tethering
PRODUCT_PROPERTY_OVERRIDES += \
    net.tethering.noprovisioning=true

# Time
PRODUCT_PROPERTY_OVERRIDES += \
    persist.timed.enable=true

# WLAN
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0
