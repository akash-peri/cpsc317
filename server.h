#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <time.h>

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
void set_word_double_array(char **array, char *destination, int line, int start_pos, int char_count);
void set_word_single_array(char *array, char *destination, int start_pos, int char_count);
int get_word_size_double_array(char **array, int line, int start_pos, char delimiter);
int get_word_size_single_array(char *array, int start_pos, char delimiter);
char* get_session_num();
void create_timer();
void start_timer(struct itimerspec play_interval, timer_t play_timer);
void stop_timer(struct itimerspec play_interval, timer_t play_timer);