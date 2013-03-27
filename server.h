#include <stdio.h>

#define NEW_LINE {'\n')
#define typedef e_rtsp_requests { \
SETUP, \
PLAY, \
PAUSE, \
TEARDOWN, \
} RTSP_Requests;

void *serve_client(void *ptr);
void start_server(int port);
