# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    persist.audio.fluence.speaker=true \
    persist.audio.fluence.voicecall=true \
    persist.audio.fluence.voicerec=false \
    ro.qc.sdk.audio.fluencetype=fluence \
    use.voice.path.for.pcm.voip=false \
    use.dedicated.device.for.voip=true \
    audio.offload.buffer.size.kb=32 \
    av.offload.enable=true \
    av.streaming.offload.enable=true \
    audio.offload.multiple.enabled=false \
    audio.offload.gapless.enabled=true \
    audio.offload.pcm.16bit.enable=true \
    audio.offload.pcm.24bit.enable=true

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    persist.hwc.mdpcomp.enable=true \
    ro.hdcp2.rx=tz \
    ro.qualcomm.cabl=0 \
    ro.secwvk=144 \
    ro.sf.lcd_density=560 \
    ro.opengles.version=196608

# GPS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.gps.qc_nlp_in_use=0 \
    ro.gps.agps_provider=1 \
    ro.qc.sdk.izat.premium_enabled=0 \
    ro.qc.sdk.izat.service_mask=0x0

# System prop for standalone GNSS service
PRODUCT_PROPERTY_OVERRIDES += \
    persist.qca1530=detect

# Radio
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.jbims=1 \
    ro.use_data_netmgrd=false \
    persist.data.netmgrd.qos.enable=true \
    persist.radio.add_power_save=1 \
    persist.radio.lte_vrat_report=1

PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0 \
    wifi.supplicant_scan_interval=15 

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sensors=1

# disable strict mode (no more UI flashing red)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.android.strictmode=0 \
    persist.sys.strictmode.disable=1
