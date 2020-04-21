#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# home wake
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 99e2e8deed64c159736142bcd541c68d1d9b6966
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 8e12af5bcbeeef9762b67ae1c390611b3f718919

# lockscreen battery charge info fix
git -C frameworks/base fetch https://github.com/Staydirtyboi/android_frameworks_base-1 ten && git -C frameworks/base cherry-pick 2898028a61ca52defb5efd2e8068d3d6ba211563
