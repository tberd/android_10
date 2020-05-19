#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Jprimero15/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# replace doze package
#git -C packages/apps/ConfigCenter fetch https://github.com/Staydirtyboi/android_packages_apps_ConfigCenter ten && git -C packages/apps/ConfigCenter cherry-pick c67d6bc20f5a7736e6663df2ce9e6b56140b3d34

# home wake
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 9879ba3b59c737d0198fe8d34ce3dfb980d3a837
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 26f39da1ad72e302a4949e91f52b6c98077c9d45

# fck fps info tile
#git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 0c2f65476ee64e1e05d631bc9897178b8a471825

# shutup pwr
git -C vendor/qcom/opensource/power fetch https://github.com/Jprimero15/android_10 power && git -C vendor/qcom/opensource/power cherry-pick 426144bad449ada7c3a98b15be4e5403c3693d2b

# fw/av picks wfd
git -C frameworks/av fetch https://github.com/Jprimero15/android_10 fw_av && git -C frameworks/av cherry-pick 55e086441abc08aeee583cbbe3ac9c4aeb874cd5

