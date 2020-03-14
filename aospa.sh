#!/bin/bash

# Needed Exports to fix Some Errors
export SELINUX_IGNORE_NEVERALLOWS=true
export TEMPORARY_DISABLE_PATH_RESTRICTIONS=true

#netd fix 
 git -C system/netd fetch https://github.com/Jprimero15/android_10 system_netd && git -C system/netd cherry-pick 1b57bbbae670664b9e1b3095f7f998c74a640b45 

# fw/base: Enable home button wake
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 fw_base && git -C frameworks/base cherry-pick e25266e6ceb1f8912699f4beb46e3d30d10509e3

# fw/native:Gralloc2: Allow invalid usage bits
git -C frameworks/native fetch https://github.com/RevengeOS/android_frameworks_native r10.0 && git -C frameworks/native cherry-pick 47721b05d326e5a12d470a54e6698ba2a2f7801e

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
