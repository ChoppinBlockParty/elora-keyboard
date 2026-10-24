#pragma once

// Time window in milliseconds for deciding whether a dual key is tapped or held.
#define TAPPING_TERM 200
// Quick second-press window in milliseconds; zero disables tap repeat to allow holds.
#define QUICK_TAP_TERM 0
// Use the hands involved to distinguish rolling taps from layer chords.
#define CHORDAL_HOLD
// Let selected layer thumbs hold as soon as an allowed second key is pressed.
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
// Let selected modifier keys hold when a second key is pressed and released.
#define PERMISSIVE_HOLD_PER_KEY
// Synchronize input activity timestamps so idle behavior follows either half.
#define SPLIT_ACTIVITY_ENABLE
// Synchronize active layers so the TFT can show them when it is on the slave.
#define SPLIT_LAYER_STATE_ENABLE
// Synchronize active modifiers so the TFT can show them on either USB setup.
#define SPLIT_MODS_ENABLE

// Both module variants must reserve the same split transaction ID.
#define SPLIT_TRANSACTION_IDS_USER ELORA_KEYLOG_SYNC
