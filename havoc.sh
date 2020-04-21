#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# home wake
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 99e2e8deed64c159736142bcd541c68d1d9b6966
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 8e12af5bcbeeef9762b67ae1c390611b3f718919

# lockscreen battery charge info fix
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 2898028a61ca52defb5efd2e8068d3d6ba211563

# enabl3 screen record boi
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick cb60979075bc91af518284670dc4550602748472 

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
