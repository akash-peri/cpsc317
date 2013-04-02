#include <stdio.h>

#define NEW_LINE {'\n')
/*
#define typedef e_rtsp_requests { \
SETUP, \
PLAY, \
PAUSE, \
TEARDOWN, \
} RTSP_Requests;
*/

void *serve_client(void *ptr);
void start_server(int port);
char* get_session_num();
void set_word(char **array, char *destination, int line, int start_pos, int char_count);
int get_word_size(char **array, int line, int start_pos, char delimiter);
char* get_session_num();
