#pragma once

#include "tap_hold_config.h"

// display.c runs on whichever half holds the TFT, which may be the slave, so
// everything it reads has to cross the wire.
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_MODS_ENABLE

// Both module variants must reserve the same transaction ID.
#define SPLIT_TRANSACTION_IDS_USER ELORA_KEYLOG_SYNC
