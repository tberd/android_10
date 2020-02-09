#!/bin/bash

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
