#!/bin/bash

# ril sprint fix
git -C packages/providers/TelephonyProvider fetch https://github.com/Claymore1297/packages_providers_TelephonyProvider q10.0 && git -C packages/providers/TelephonyProvider cherry-pick a1e2b2e355f47aadea78599a746081ebaebc73bd

# home wake
git -C frameworks/base fetch https://github.com/Staydirtyboi/frameworks_base xq && git -C frameworks/base cherry-pick a64c28c261463a497a8c632d30ce8378ed91c634 
