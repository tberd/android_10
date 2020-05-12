#!/bin/bash

#cd external/moto/faceunlock/ && wget https://raw.githubusercontent.com/crdroidandroid/android_external_moto_faceunlock/c8697f5d037adf57e07c42c40ae8364dfaa7ca50/config.mk?token=AEHA4UZNCZNLNL7IVB5YHSK6XK7OG

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# lockscreen batt fix
git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick f8775fdbf99d2fb719bda6d7803fd1122cbcdb17

Shutup pwr
git -C vendor/qcom/opensource/power fetch https://github.com/Jprimero15/android_10 power && git -C vendor/qcom/opensource/power cherry-pick 426144bad449ada7c3a98b15be4e5403c3693d2b

# fw/av picks
cd frameworks/av
git fetch https://github.com/crdroidandroid/android_frameworks_av 10.0-20200203
git cherry-pick 5147925b # ld.config: add libashmemd_client.so to platform shared libs
git cherry-pick b8772262 # Revert "Removed unused class and its test"
git cherry-pick bf64535c # Revert "stagefright: remove Miracast sender code"
git cherry-pick 4a9c4129 # libstagefright_wfd: libmediaplayer2: compilation fixes
git cherry-pick f168b4e8 # stagefright: Fix SurfaceMediaSource getting handle from wrong position
git cherry-pick 6f7f4e34 # stagefright: Fix buffer handle retrieval in signalBufferReturned
git cherry-pick 87dcc138 # libstagefright_wfd: video encoder does not actually release MediaBuffer
git cherry-pick e883e76e # Revert "Move unused classes out of stagefright foundataion"
git cherry-pick eb16bec7 # Remove libmediaextractor dependency from libstagefright_wfd and Surface
git cherry-pick d00e3f1c # audioflinger: Fix audio for WifiDisplay
cd .. && cd .. 
