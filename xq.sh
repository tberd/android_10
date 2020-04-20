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
