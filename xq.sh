#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

nuke fps
git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick 8264850111b6fd4667641a7cabe8c8e09dde8793 

screen recorder
git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick 250a594f0dd37b49f300a97033cc1bd62185d230 


Shutup pwr
git -C vendor/qcom/opensource/power fetch https://github.com/Jprimero15/android_10 power && git -C vendor/qcom/opensource/power cherry-pick 426144bad449ada7c3a98b15be4e5403c3693d2b

# fw/av picks wfd
git -C frameworks/av fetch https://github.com/Jprimero15/android_10 fw_av && git -C frameworks/av cherry-pick 55e086441abc08aeee583cbbe3ac9c4aeb874cd5
