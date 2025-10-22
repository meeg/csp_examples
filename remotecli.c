#include <csp/csp_debug.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>

#include <csp/csp.h>
#include <csp/drivers/can_socketcan.h>

#include "csp_posix_helper.h"
#include "flatsat_lib.h"


#define __maybe_unused __attribute__((__unused__))

void print_help(void) {
    csp_print("Usage: remotecli [options] command\n");
    csp_print("The command must be a single string (quoted if necessary)\n");
    csp_print(" -c <can-device>  set CAN device\n"
                " -a <address>     set interface address\n"
                " -C <address>     connect to server at address\n"
                " -s               set up the CAN interface if not already enabled (you must be root)\n"
                " -v               verbose (enable debug printouts)\n"
                " -h               print help\n");
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
    csp_print("command: %s\n", cmd);

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
