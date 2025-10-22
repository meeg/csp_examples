#include <csp/csp_debug.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdio.h>

#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>

#include "csp_posix_helper.h"

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

#define __maybe_unused __attribute__((__unused__))

void print_help(void) {
    csp_print("Usage: remotecli [options] command\n");
    csp_print("The command must be a single string (quoted if necessary)\n");
    csp_print(" -c <can-device>  set CAN device\n"
                " -a <address>     set interface address\n"
                " -C <address>     connect to server at address\n"
                " -t               enable test mode\n"
                " -T <duration>    enable test mode with running time in seconds\n"
                " -s               set up the CAN interface if not already enabled (you must be root)\n"
                " -h               print help\n");
}

int remotecli(int server_address, const char * cmd, char * response, int maxlen) {
    csp_packet_t * req_pkt = csp_buffer_get(0);
    if (req_pkt == NULL) {
        /* Could not get buffer element */
        csp_print("Failed to get CSP buffer\n");
        return(EXIT_FAILURE);
    }
    int len = strlen(cmd);
    if (len > 126) { // 128 minus 2 bytes for the \r\n
        csp_print("command is too long for a single packet\n");
        return(EXIT_FAILURE);
    }
    // fill the request data
    remotecli_req_t req;
    req.cmdId = 1;
    req.delayBetweenPacketsInMs = 0;
    memcpy(req.inputData, cmd, len);
    memcpy(req.inputData+len, "\r\n", 2); // not null-terminated

    // fill the request packet
    memcpy(&req_pkt->data, &req, sizeof(remotecli_req_t));
    req_pkt->length = len+5; // 3 bytes of header, 2 bytes for the \r\n

    // send the request
    csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, REMOTECLI_PORT, 1000, CSP_O_NONE);
    csp_send(conn, req_pkt);

    int reslen = 0;

    int i_res = 0;
    while (1) {
        csp_packet_t *res_pkt = csp_read(conn, 1000);
        if (res_pkt == NULL) {
            csp_print("Failed to get response\n");
            return(EXIT_FAILURE);
        }
        //csp_print("length %d\n", res_pkt->length);
        int datalen = res_pkt->length-4;
        remotecli_res_t res;
        memcpy(&res, res_pkt->data, sizeof(remotecli_res_t));
        //csp_print("cmd %d\n", res.cmdId);
        //csp_print("error %d\n", res.errorCode);
        //csp_print("packetId %d\n", res.packetId);
        //csp_print("isFinal %d\n", res.isFinal);
        //csp_print("%s\n",res.outputData);
        //for (int i=0; i<res->length-4; i++) {
        //    csp_print("%d %d\n", i, x.outputData[i]);
        //}
        if (res.cmdId != 1) {
            csp_print("unexpected cmdId in response packet\n");
            return(EXIT_FAILURE);
        }
        if (res.errorCode != 0) {
            csp_print("unexpected errorCode in response packet\n");
            return(EXIT_FAILURE);
        }
        if (res.packetId != i_res) {
            csp_print("unexpected packetId in response packet\n");
            return(EXIT_FAILURE);
        }
        if (reslen+datalen >= maxlen) {
            csp_print("response is too long for the response array\n");
            return(EXIT_FAILURE);
        }
        memcpy(response+reslen, res.outputData, datalen); // not null-terminated
        reslen += datalen;
        csp_buffer_free(res_pkt);
        if (res.isFinal) break;
        i_res++;
    }
    response[reslen] = 0;
    csp_close(conn);
    return(EXIT_SUCCESS);
}

/* main - initialization of CSP and start of client task */
int main(int argc, char * argv[]) {

    const char * device_name = "can0";
    csp_iface_t * default_iface;
    int ret = EXIT_SUCCESS;
    int opt;

    bool setup_can = false;
    csp_conf.version = 1;

    uint8_t server_address = EPS_ADDRESS;
    uint8_t client_address = 0;

    bool debug = false;

    while ((opt = getopt(argc, argv, "c:a:C:svh")) != -1) {
        switch (opt) {
            case 'c':
                device_name = optarg;
                break;
            case 'a':
                client_address = atoi(optarg);
                break;
            case 'C':
                server_address = atoi(optarg);
                break;
            case 's':
                setup_can = true;
                break;
            case 'v':
                debug = true;
                break;
            case 'h':
            case '?':
                // Invalid option or missing argument
                print_help();
                exit(EXIT_FAILURE);
        }
    }
    if (debug) csp_dbg_packet_print = 1;
    /*
       char cmd[1000];
       cmd[0]=0;
       for (int i=optind; i<argc; i++) {
       strcat(cmd, argv[i]);
       if (i<argc-1) strcat(cmd, " ");
       }
       csp_print("%s\n", cmd);
       return(0);
     */
    if (optind != argc-1) {
        print_help();
        exit(EXIT_FAILURE);
    }

    char * cmd = argv[argc-1];
    if (debug) csp_print("command: %s\n", cmd);

    if (debug) csp_print("Initialising CSP\n");

    /* Init CSP */
    csp_init();

    /* Start router */
    router_start();

    /* Add interface(s) */
    int bitrate = setup_can ? 1000000 : 0;
    int error = csp_can_socketcan_open_and_add_interface(device_name, CSP_IF_CAN_DEFAULT_NAME, client_address, bitrate, true, &default_iface);
    if (error != CSP_ERR_NONE) {
        csp_print("failed to add CAN interface [%s], error: %d\n", device_name, error);
        exit(1);
    }
    default_iface->is_default = 1;

    /*
       csp_reboot(server_address);
       csp_print("reboot system request sent to address: %u\n", server_address);

       while (1) {
       int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
       csp_print("Ping address: %u, result %d [mS]\n", server_address, result);
       if (result >= 0) break;
       }
     */

    int maxlen = 10000;
    char response[maxlen];
    remotecli(server_address, cmd, response, maxlen);
    csp_print("%s\n",response);

    return ret;
}
