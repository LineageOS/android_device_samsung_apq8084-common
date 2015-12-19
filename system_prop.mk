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

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    qcom.bluetooth.soc=rome \
    enablebtsoclog=false

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    persist.hwc.mdpcomp.enable=false \
    ro.hdcp2.rx=tz \
    ro.qualcomm.cabl=1 \
    ro.secwvk=144 \
    ro.opengles.version=196609

# Fingerprint
PRODUCT_PROPERTY_OVERRIDES += \
    fingerprint_enabled=1

# GPS
PRODUCT_PROPERTY_OVERRIDES += \
    persist.gps.qc_nlp_in_use=0 \
    ro.gps.agps_provider=1 \
    ro.qc.sdk.izat.premium_enabled=0 \
    ro.qc.sdk.izat.service_mask=0x0

# NFC
PRODUCT_PROPERTY_OVERRIDES += \
    ro.nfc.port=I2C

# QCOM Perf lib
PRODUCT_PROPERTY_OVERRIDES += \
   ro.vendor.extension_library=/vendor/lib/libqc-opt.so

# Radio
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libargs=-d /dev/smd0 \
    rild.libpath=/system/lib/libsec-ril.so \
    ro.ril.telephony.mqanelements=6 \
    ro.telephony.ril_class=lentislteRIL \
    persist.data.netmgrd.qos.enable=true \
    persist.radio.add_power_save=1 \
    persist.radio.lte_vrat_report=1 \
    ro.telephony.mms_data_profile=5

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sensors=1

# USB OTG interface
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.isUsbOtgEnabled=true

# USB
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp
