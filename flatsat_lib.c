#include <stdlib.h>

#include <csp/csp.h>

#include "flatsat_lib.h"

int remotecli(int server_address, int server_port, const char * cmd, char * response, int maxlen, int timeout) {
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
    csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, server_port, 1000, CSP_O_NONE);
    csp_send(conn, req_pkt);

    int reslen = 0;

    int i_res = 0;
    while (1) {
        csp_packet_t *res_pkt = csp_read(conn, timeout);
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

