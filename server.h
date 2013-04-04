

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define NEW_LINE {'\n')
/*
#define typedef e_rtsp_requests { \
SETUP, \
PLAY, \
PAUSE, \
TEARDOWN, \
} RTSP_Requests;
*/

typedef struct send_frame_data {
  int socket_fd;
  
} send_frame_data;

typedef struct create_timer_data {
	struct send_frame_data data; // Set fields as necessary
	struct sigevent play_event;
	timer_t play_timer;
	struct itimerspec play_interval;
} create_timer_data;

void *serve_client(void *ptr);
void start_server(int port);
char* get_session_num();
void set_word_double_array(char **array, char *destination, int line, int start_pos, int char_count);
void set_word_single_array(char *array, char *destination, int start_pos, int char_count);
int get_word_size_double_array(char **array, int line, int start_pos, char delimiter);
int get_word_size_single_array(char *array, int start_pos, char delimiter);
char* get_session_num();
void create_timer(create_timer_data timer_data);
void start_timer(struct itimerspec *play_interval, timer_t *play_timer);
void stop_timer(struct itimerspec *play_interval, timer_t *play_timer);
int load_video(char *filename);