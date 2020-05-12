#!/bin/bash

#cd external/moto/faceunlock/ && wget https://raw.githubusercontent.com/crdroidandroid/android_external_moto_faceunlock/c8697f5d037adf57e07c42c40ae8364dfaa7ca50/config.mk?token=AEHA4UZNCZNLNL7IVB5YHSK6XK7OG

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# lockscreen batt fix
git -C frameworks/base revert --no-edit 2ca7f6fc0307ab7ec2f48c8de4e78e2d1c6ded3b
git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick f8775fdbf99d2fb719bda6d7803fd1122cbcdb17

Shutup pwr
git -C vendor/qcom/opensource/power fetch https://github.com/Jprimero15/android_10 power && git -C vendor/qcom/opensource/power cherry-pick 426144bad449ada7c3a98b15be4e5403c3693d2b

# fw/av picks wfd
git -C frameworks/av fetch https://github.com/Jprimero15/android_10 fw_av && git -C frameworks/av cherry-pick 55e086441abc08aeee583cbbe3ac9c4aeb874cd5

