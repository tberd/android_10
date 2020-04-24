#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Jprimero15/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# replace doze package
git -C packages/apps/ConfigCenter fetch https://github.com/Staydirtyboi/android_packages_apps_ConfigCenter ten && git -C packages/apps/ConfigCenter cherry-pick c67d6bc20f5a7736e6663df2ce9e6b56140b3d34

# home wake
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 99e2e8deed64c159736142bcd541c68d1d9b6966
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 8e12af5bcbeeef9762b67ae1c390611b3f718919

# lockscreen battery charge info fix
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 2898028a61ca52defb5efd2e8068d3d6ba211563

# shutup pwr
git -C vendor/qcom/opensource/power fetch https://github.com/Jprimero15/android_10 power && git -C vendor/qcom/opensource/power cherry-pick 426144bad449ada7c3a98b15be4e5403c3693d2b

# enabl3 screen record boi
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick cb60979075bc91af518284670dc4550602748472 

# fw/av picks wfd
git -C frameworks/av fetch https://github.com/Jprimero15/android_10/tree/fw_av && git -C frameworks/av cherry-pick 55e086441abc08aeee583cbbe3ac9c4aeb874cd5

