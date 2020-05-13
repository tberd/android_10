#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# home wake
#git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick a64c28c261463a497a8c632d30ce8378ed91c634 

# lockscreen battery charge info fix
#git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick a581994a4fb6a7a2564ce4f61f2a61f718dd1e11 
#git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick aea0c6b914fb99171d5c94ddf3547c195fac4dfe 

# Fix compile
git -C frameworks/av revert --no-edit cf5b100a734071d1698564911254783f1eae9ea1

Shutup pwr
git -C vendor/qcom/opensource/power fetch https://github.com/Jprimero15/android_10 power && git -C vendor/qcom/opensource/power cherry-pick 426144bad449ada7c3a98b15be4e5403c3693d2b

# fw/av picks wfd
git -C frameworks/av fetch https://github.com/Jprimero15/android_10 fw_av && git -C frameworks/av cherry-pick 55e086441abc08aeee583cbbe3ac9c4aeb874cd5
