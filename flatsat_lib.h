#pragma once

/* Server port, the port the server listens on for incoming connections from the client. */
#define EPS_ADDRESS             4
#define EPS_PORT_TELEMETRY      7
#define REMOTECLI_PORT          13

typedef struct __attribute__((packed)) {
    uint8_t cmdId;
    int8_t resultCode;
    uint8_t state; // Battery state. 0 - critical, 1 - safe, 2 - normal, 3 - full.
    uint16_t voltage; // Battery voltage in mV.
    uint16_t chargeCurrent; // Battery charge current in mA.
    uint16_t dischargeCurrent; // Battery discharge current in mA.
} telemetry_battery_t;

typedef struct __attribute__((packed)) {
    uint8_t cmdId;
    uint16_t delayBetweenPacketsInMs;
    char inputData[128];
} remotecli_req_t;

typedef struct __attribute__((packed)) {
    uint8_t cmdId;
    uint8_t errorCode;
    uint8_t packetId;
    bool isFinal;
    char outputData[128];
} remotecli_res_t;

int remotecli(int server_address, const char * cmd, char * response, int maxlen);
