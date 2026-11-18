#include "control_link/protocol.hpp"

#include <cstdio>
#include <cstring>

#define CHECK(cond)                                                     \
    if (!(cond)) {                                                      \
        printf("%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #cond); \
        return 1;                                                       \
    }

int main() {
    const uint8_t catalogue[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    CHECK(crc8(catalogue, 9) == 0xF4); // CRC-8/SMBUS check value

    float x = 120.5f;
    float y = -30.25f;
    float theta = 1.5f;
    uint8_t frame[FRAME_LEN] = {};
    frame[0] = MAGIC;
    frame[1] = STATUS_ACK | FLAG_BATTERY_LOW;
    frame[2] = 7;
    memcpy(&frame[3], &x, 4);
    memcpy(&frame[7], &y, 4);
    memcpy(&frame[11], &theta, 4);
    frame[15] = crc8(frame, 15);

    reply_t reply;
    CHECK(decodeReply(frame, &reply));
    CHECK(reply.status == STATUS_ACK);
    CHECK(reply.flags == FLAG_BATTERY_LOW);
    CHECK(reply.seq == 7);
    CHECK(reply.x == x);
    CHECK(reply.y == y);
    CHECK(reply.theta == theta);

    frame[5] ^= 0x01;
    CHECK(!decodeReply(frame, &reply));

    printf("ok\n");
    return 0;
}
