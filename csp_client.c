#include <csp/csp_debug.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdio.h>

#include <csp/csp.h>
#include <csp/drivers/can_socketcan.h>

#include "csp_posix_helper.h"
#include "flatsat_lib.h"

/* Server port, the port the server listens on for incoming connections from the client. */
#define EPS_ADDRESS             4
#define EPS_PORT_TELEMETRY      7

/* Commandline options */
static uint8_t server_address = EPS_ADDRESS;
static uint8_t client_address = 0;

/* Test mode, check that server & client can exchange packets */
static bool test_mode = false;
static unsigned int successful_ping = 0;
static unsigned int run_duration_in_sec = 3;

#define __maybe_unused __attribute__((__unused__))

static struct option long_options[] = {
    {"can-device", required_argument, 0, 'c'},
    {"interface-address", required_argument, 0, 'a'},
    {"connect-to", required_argument, 0, 'C'},
    {"protocol-version", required_argument, 0, 'v'},
    {"test-mode", no_argument, 0, 't'},
    {"test-mode-with-sec", required_argument, 0, 'T'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

void print_help(void) {
    csp_print("Usage: csp_client [options]\n");
    csp_print(" -c <can-device>  set CAN device\n"
                " -a <address>     set interface address\n"
                " -C <address>     connect to server at address\n"
                " -t               enable test mode\n"
                " -T <duration>    enable test mode with running time in seconds\n"
                " -s               set up the CAN interface if not already enabled (you must be root)\n"
                " -h               print help\n");
}

/* main - initialization of CSP and start of client task */
int main(int argc, char * argv[]) {

    const char * device_name = "can0";
    const char * rtable __maybe_unused = NULL;
    csp_iface_t * default_iface;
    struct timespec start_time;
    unsigned int count;
    int ret = EXIT_SUCCESS;
    int opt;

    bool setup_can = false;
    csp_conf.version = 1;
    csp_dbg_packet_print = 1;
    csp_dbg_rdp_print = 1;

    while ((opt = getopt_long(argc, argv, "c:a:C:tT:sh", long_options, NULL)) != -1) {
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
            case 't':
                test_mode = true;
                break;
            case 'T':
                test_mode = true;
                run_duration_in_sec = atoi(optarg);
                break;
            case 's':
                setup_can = true;
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            case '?':
                // Invalid option or missing argument
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    csp_print("Initialising CSP\n");

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

    csp_print("Connection table\r\n");
    csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    if (CSP_USE_RTABLE) {
        csp_print("Route table\r\n");
        csp_rtable_print();
    }

    /* Start client work */
    csp_print("Client started\n");
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    count = 'A';

    /*
    csp_reboot(server_address);
    csp_print("reboot system request sent to address: %u\n", server_address);

    while (1) {
        int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
        csp_print("Ping address: %u, result %d [mS]\n", server_address, result);
        if (result >= 0) break;
    }
    */

    /* 1. Connect to host on 'server_address', port SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
    csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, EPS_PORT_TELEMETRY, 1000, CSP_O_NONE);
    if (conn == NULL) {
        /* Connect failed */
        csp_print("Connection failed\n");
        ret = EXIT_FAILURE;
    } else while (1) {
        struct timespec current_time;

        usleep(test_mode ? 200000 : 1000000);

        /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
        int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
        csp_print("Ping address: %u, result %d [mS]\n", server_address, result);
        // Increment successful_ping if ping was successful
        if (result >= 0) {
            ++successful_ping;
        }


        /* 2. Get packet buffer for message/data */
        csp_packet_t * packet = csp_buffer_get(0);
        if (packet == NULL) {
            /* Could not get buffer element */
            csp_print("Failed to get CSP buffer\n");
            ret = EXIT_FAILURE;
            break;
        }

        /* 3. Copy data to packet */
        uint8_t b = 128;
        memcpy(packet->data, &b, 1);
        count++;

        /* 4. Set packet length */
        packet->length = 1;

        /* 5. Send packet */
        csp_send(conn, packet);

        csp_packet_t *res = csp_read(conn, 1000);
        if (res != NULL) {
            printf("length %d\n", res->length);
            if (res->length == 9) {
                telemetry_battery_t x;
                memcpy(&x, res->data, 9);
                printf("cmd %d\n", x.cmdId);
                printf("result %d\n", x.resultCode);
                printf("state %d\n", x.state);
                printf("voltage %d\n", x.voltage);
                printf("charge %d\n", x.chargeCurrent);
                printf("discharge %d\n", x.dischargeCurrent);
            }
        } else {
            csp_print("Failed to get response\n");
            //ret = EXIT_FAILURE;
            //break;
        }
        // sent packets are freed for you, but you need to free received packets when you're done with them
        csp_buffer_free(res);

        printf("Client successfully pinged the server %u times\n", successful_ping);
        printf("%d %d %d %d %d %d %d\n", csp_dbg_buffer_out, csp_dbg_conn_out, csp_dbg_conn_ovf, csp_dbg_conn_noroute, csp_dbg_inval_reply, csp_dbg_errno, csp_dbg_can_errno);
        printf("%d\n", csp_buffer_remaining());


        /* 7. Check for elapsed time if test_mode. */
        if (test_mode) {
            clock_gettime(CLOCK_MONOTONIC, &current_time);

            /* We don't really care about the precision of it. */
            if (current_time.tv_sec - start_time.tv_sec > run_duration_in_sec) {
                /* Test mode, check that server & client can exchange packets */
                if (successful_ping < 5) {
                    csp_print("Client successfully pinged the server %u times\n", successful_ping);
                    ret = EXIT_FAILURE;
                    break;
                }
                csp_print("Client successfully pinged the server %u times\n", successful_ping);
                break;
            }
        }
    }

    /* 6. Close connection */
    csp_close(conn);
    /* Wait for execution to end (ctrl+c) */

    return ret;
}
