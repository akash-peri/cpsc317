#include "cloud_helper.h"
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
#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define ENDOFARR '\0'
#define NEW_LINE {'\n'}
#define UNKNOWN 0
#define UNINITIALIZED 1
#define SETUP 2
#define PLAY 3
#define PAUSE 4
#define TEARDOWN 5
#define INIT 6
#define READY 7
#define PLAYING 8

#define SUCCESS " 200"
#define SUCCESS_MSG " OK"
#define NOT_FOUND " 404"
#define NOT_FOUND_MSG " Not Found"

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
  int scale;
  int timestamp_start;
  int send_count;
  char *video_name;
  int frame_number;
  int cseq;
  int done;
  struct itimerspec play_interval;
  timer_t play_timer;
} send_frame_data;

typedef struct response_data
{
	char *rtsp_format;
	char *return_code;
	char *return_msg;
	char *cseq;
	char *session;
} response_data;

void *serve_client(void *ptr);
void start_server(int port);
char* get_session_num();
void set_word_double_array(char **array, char *destination, int line, int start_pos, int char_count);
void set_word_single_array(char *array, char *destination, int start_pos, int char_count);
int get_word_size_double_array(char **array, int line, int start_pos, char delimiter);
int get_word_size_single_array(char *array, int start_pos, char delimiter);
char* get_session_num();
//void create_timer(create_timer_data timer_data);
void send_frame(union sigval sv_data);
void start_timer(struct itimerspec play_interval, timer_t play_timer);
void stop_timer(struct itimerspec play_interval, timer_t play_timer);
CvMat* get_encoded(CvCapture *video, int scale);
char* get_response(char *return_array, int state, response_data rdi);
int sendall(int s, char *buf, int len);