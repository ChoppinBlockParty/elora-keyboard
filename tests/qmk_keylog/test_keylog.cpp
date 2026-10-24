#include "test_common.hpp"
#include <cstring>

using testing::_;

extern "C" {
#include "keylog.h"
#include "tap_hold.h"
#include "transactions.h"
}

static bool master = true;
static bool left = true;
static bool connected = true;
static bool send_ok = true;
static unsigned sends;
static slave_callback_t receive;
static keylog_state_t sent;

extern "C" bool is_keyboard_master(void) { return master; }
extern "C" bool is_keyboard_left(void) { return left; }
extern "C" bool is_transport_connected(void) { return connected; }
extern "C" void transaction_register_rpc(int8_t id, slave_callback_t callback) {
    EXPECT_EQ(id, ELORA_KEYLOG_SYNC);
    receive = callback;
}
extern "C" bool transaction_rpc_send(int8_t id, uint8_t size, const void *data) {
    EXPECT_EQ(id, ELORA_KEYLOG_SYNC);
    EXPECT_EQ(size, sizeof(sent));
    memcpy(&sent, data, sizeof(sent));
    ++sends;
    return send_ok;
}

class EloraKeylog : public TestFixture {};

TEST_F(EloraKeylog, labels_sync_with_right_usb_master_and_recover_after_failure) {
    TestDriver driver;
    EXPECT_NO_REPORT(driver);
    ASSERT_NE(receive, nullptr);
    keyrecord_t record = {};
    record.event.pressed = true;
    keylog_note_keypress(KC_Q, &record);
    EXPECT_STREQ(keylog_get_state()->labels[0], "Q");
    record.event.key.row = MATRIX_ROWS / 2;
    record.tap.count = 1;
    keylog_note_keypress(J_SHIFT, &record);
    EXPECT_STREQ(keylog_get_state()->labels[1], "J");
    record.event.pressed = false;
    keylog_note_keypress(KC_Z, &record);
    EXPECT_STREQ(keylog_get_state()->labels[1], "J");

    // A left master displays local labels without a wire transaction.
    idle_for(60);
    EXPECT_EQ(sends, 0u);
    left = false;
    idle_for(1);
    ASSERT_EQ(sends, 1u);
    EXPECT_STREQ(sent.labels[0], "Q");
    EXPECT_STREQ(sent.labels[1], "J");
    idle_for(60);
    EXPECT_EQ(sends, 1u);

    // Failed updates are retried after 50 ms, without blocking every scan.
    send_ok = false;
    record.event.pressed = true;
    keylog_note_keypress(KC_F12, &record);
    idle_for(1);
    EXPECT_EQ(sends, 2u);
    idle_for(20);
    EXPECT_EQ(sends, 2u);
    send_ok = true;
    idle_for(30);
    ASSERT_EQ(sends, 3u);
    EXPECT_STREQ(sent.labels[1], "F12");
    idle_for(1000);
    EXPECT_EQ(sends, 4u); // Periodic refresh recovers a restarted display half.

    connected = false;
    idle_for(1100);
    EXPECT_EQ(sends, 4u);
    connected = true;
    master = false;
    idle_for(1100);
    EXPECT_EQ(sends, 4u);

    keylog_state_t remote = {{"SPACE", "MOD"}};
    receive(sizeof(remote) - 1, &remote, 0, nullptr);
    EXPECT_STREQ(keylog_get_state()->labels[1], "F12");
    receive(sizeof(remote), &remote, 0, nullptr);
    EXPECT_STREQ(keylog_get_state()->labels[0], "SPACE");
    EXPECT_STREQ(keylog_get_state()->labels[1], "MOD");
}
