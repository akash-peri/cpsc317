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
#include "server.h"
#include <arpa/inet.h>

#define ALLOWED_CONNECTIONS 5
#define TRUE 1
#define FALSE 0

char *rtsp_format;
char *transport_type;

void *serve_client(void *ptr) {
  // Converts ptr back to integer.
  int client_fd = (int) (intptr_t) ptr;
  
  //loop until we want to close the program
  while(TRUE)
  {
	//if data in socket
	

	char *data = (char *)malloc(4096);
	char **parsed_data = (char **)malloc(4);
	if(parsed_data)
	{
		int j = 0;
		for(j = 0; j < 4; j++)
		{
			parsed_data[0] = (char *)malloc(1024);
		}
	}

      char* data = (char *)malloc(4096);
	char parsed_data[16][1024] = (char [][]]malloc(16*1024);

		
	int done = FALSE;
	int line_counter = 0;
	int char_counter = 0;
	while(done == 0)
	{
		int read_size = (int) read(client_fd, data, 4096);
		if(read_size < 0)
		{
			perror ("read failed");
		}
		//parse data now
		
		if(read_size < 4096)
		{
			done = TRUE; // nothing else to read this time
		}
		

        int j;
		for(j = 0; j < read_size; j++)
		{
			char data_char = data[j];
			char *data_array = malloc(2);
			data_array[0] = data_char;
			char *new_line = {"\n"};
			if(strpbrk(data_array, new_line) != NULL)

		int j;
		for(j = 0; j < read_size; j++)
		{
			char data_char = data[j];
			char new_line = {'\n'};
			if(strpbrk(data_char, new_line) != NULL)
			{
				line_counter++;
				//parsed_data[line_counter] = malloc(1024);
			}
			else
			{
				parsed_data[line_counter][char_counter++] = data_char;
			}
		}
	}
	free(data);
	parsed_data = (char **) realloc (parsed_data, (line_counter + 1) * 1024);
	
	//at this point, we have our parsed data
	//now, we need to analyze it
	
	//get first word: should be one of SETUP, PLAY, PAUSE, TEARDOWN
	
	char SPACE = ' ';
	int valid = 1;
	
	int char_length = get_word_size(parsed_data, 0, 0, SPACE);
	int char_count = char_length;
	char *control_string = (char *)malloc(char_length);
	set_word(parsed_data, control_string, 0, char_count, char_length);
	
	//control string can be SELECT, PLAY, PAUSE, TEARDOWN
	if(strncmp(control_string,"SELECT",7))
	{
		//get the video name now
		char_length = get_word_size(parsed_data, 0, ++char_count, SPACE);
		char_count += char_length;
		char *movie_string = (char *)malloc(char_length);
		set_word(parsed_data, movie_string, 0, char_count, char_length);
		//get the rtsp format
		char_length = get_word_size(parsed_data, 0, ++char_count, SPACE);
		char_count += char_length;
		char *rtsp_format = (char *)malloc(char_length);
		set_word(parsed_data, rtsp_format, 0, char_count, char_length);
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size(parsed_data, 1, ++char_count, SPACE);
		char_count += char_length;
		//This is for the sequence number
		char_length = get_word_size(parsed_data, 1, ++char_count, SPACE);
		char_count += char_length;
		char *cseq = (char *)malloc(char_length);
		set_word(parsed_data, cseq, 1, char_count, char_length);
		
		//goto third line, get port number
		
		//reset count
		char_count = 0;
		//This is for the Transport:
		char_length = get_word_size(parsed_data, 2, ++char_count, SPACE);
		char_count += char_length;
		//This is for the RTP/UDP;
		char_length = get_word_size(parsed_data, 2, ++char_count, SPACE);
		char_count += char_length;
		transport_type = (char *)malloc(char_length);
		set_word(parsed_data, transport_type, 2, char_count, char_length);
		//This is for the client_port=
		char_length = get_word_size(parsed_data, 2, ++char_count, SPACE);
		char_count += char_length;
		//This the port nubmer which we wnat1
		char_length = get_word_size(parsed_data, 2, ++char_count, SPACE);
		char_count += char_length;
		char *port_string = (char *)malloc(char_length);
		set_word(parsed_data, port_string, 2, char_count, char_length);
		//at this point, we have our char array with port_num, call video setup now
		
		//once video setup is done, formulate response
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, "200"); // the ok code
		strcat(return_array, "OK");
		strcat(return_array, "/n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
	}
	else if(strncmp(control_string,"PLAY",4))
	{
		
	}
	else if(strncmp(control_string,"PAUSE",5))
	{
	
	}
	else if(strncmp(control_string,"TEARDOWN",8))
	{
	
	}
	else
	{
		valid = 0;
	}
	
	if(!valid)
	{
		break;
	}
	
	free(control_string);
	
	//deal with it
  }
  
  // ...
  return (void*)0;
}

/*
e_rtsp_requests get_requestType()
{

}
*/

int get_word_size(char **array, int line, int start_pos, char delimiter)
{
	int char_count = start_pos;
	int valid = 1;
	if(char_count >= sizeof(array)) // if we dont have enough space to read one char, then this is invalid
	{
		valid = 0;
	}
	while(valid)
	{
		char val = array[line][char_count++];
		if(val == delimiter || char_count >= sizeof(array))
		{
			break;
		}
		//maximum length of first code; longer than this is an invalid value
		else if(char_count > 8)
		{
			valid = 0;
			break;
		}
	}
	
	return char_count;
}

//randomize
char* get_session_num()
{
	return "123456";
}
	
void set_word(char **array, char *destination, int line, int start_pos, int char_count)
{

	int j;
		for(j = 0; j < char_count; j++)
		{
			destination[j] = array[line][j];
		}

		/*
	if(valid == 0)
	{
		perror ("Unexpected request from client");
		return (void*)0;
	}
	
	else
	{
		
	}
	*/
}

void start_server(int port)
{

//flow: 
//first: we need to set up socket
//then, call accept to get client connections
//once accept returns, we call code below, then recall accept (it should be in some kind of loop)

	char *server_port = malloc(sizeof(int));
	server_port[0] = (char)port;
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct addrinfo *addr_info;
	struct addrinfo hints;
		
	if(sockfd < 0)
	{
		perror ("socket can't be made");
	}
	
	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
	hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
	
	int error = getaddrinfo(NULL, server_port, &hints, &addr_info);
	if(error != 0)
	{
		//error determining address info of server
		perror("getaddrinfo failed");
	}
	
	/*
	struct sockaddr_in addr_sock;
	memset(&addr_sock, 0, sizeof(struct sockaddr_in));
	addr_sock.sin_family = AF_INET;
	addr_sock.sin_port = htons(server_port);
	//get IP for server, put into dsr
	addr_sock.sin_addr = inet_pton(AF_INET, client, dst);
	*/
	
	if(bind(sockfd, addr_info->ai_addr, sizeof(addr_info->ai_addr)) == -1)
	{
		perror ("binding failed");
	}
	
	if(listen(sockfd, ALLOWED_CONNECTIONS) == -1)
	{
		perror ("listening failed");
	}
	
	//finally, call accept
	
	struct addrinfo *client_addr_info;
	memset(&client_addr_info, 0, sizeof(client_addr_info));
	while(true)
	{
		int accept_con = accept(sockfd, client_addr_info->ai_addr, sizeof(client_addr_info->ai_addr));
		
		// Required variables
		pthread_t thread; // Thread to be created
		// Creates the thread and calls the function serve_client.
		pthread_create(&thread, NULL, serve_client, (void *) (intptr_t) server_port);
		// Detaches the thread. This means that, once the thread finishes, it is destroyed.
		pthread_detach(thread);
		// we then want to start a timer for this pthread
		//thread
	}
}
                                  /*

//set up a separate datagram socket for recieving packets after the TCP connection has been made.
#define PORT 1111 //temp port to be used for datagram. GET FROM TCP.
#define PACK // pointer to the data packets we want to sent GET FROM DECODED VIDEO
#define LEN //number of bytes we want to send. GET FROM DECODED VIDEO

//UDP connection
void udp_server(){
    
    int sockfd;
    struct addrinfo udpHints;
    struct addrinfo *serverInfo;
    struct addrinfo *p;
    
    memset (&udpHints,0,sizeof udpHints);//initialize datagram struct
    udpHints.ai_family = AF_UNSPEC;
    udpHints.ai_socktype = SOCK_DGRAM;
    
    //gets all addr info and set it to serverinfo
    getaddrinfo("hostname", PORT, &udpHints, &serverInfo);

    // loop through all the results and make a socket
    for(p = serverInfo; p != NULL; p = p.ai_next) {
        sockfd = socket(p.ai_family, p.ai_socktype, p.ai_protocol);
    }
   
    
    //store datagram packets and send if exist
    sendto(sockfd, *PACK, LEN, 0, p.ai_addr, p.ai_addrlen);
    
    //free the socket used
    freeaddrinfo(serverInfo);
    
    close(sockfd);
}




//should return encoded as jpeg packets to be sent through UDP
void load_video(String filename){
    CvCapture *video;
    IplImage *image;
    CvMat *thumb;
    CvMat *encoded;
    
    // Open the video file.
    //get file name from TCP client
    video = cvCaptureFromFile(filename);

        if (!video) {
    // The file doesn't exist or can't be captured as a video file.
}

    // Obtain the next frame from the video file
        image = cvQueryFrame(video);
            if (!image) {
    // Next frame doesn't exist or can't be obtained.
}

    // Position the video at a specific frame number position
            cvSetCaptureProperty(video, CV_CAP_PROP_POS_FRAMES, next_frame);

    // Convert the frame to a smaller size (WIDTH x HEIGHT)
            thumb = cvCreateMat(WIDTH, HEIGHT, CV_8UC3);
            cvResize(img, thumb, CV_INTER_AREA);

    // Encode the frame in JPEG format with JPEG quality 30%.
        const static int encodeParams[] = { CV_IMWRITE_JPEG_QUALITY, 30 };
        encoded = cvEncodeImage(".jpeg", thumb, encodeParams);

// After the call above, the encoded data is in encodeddata.ptr and has a length of encoded->cols bytes.

// Close the video file
cvReleaseCapture(&video);
    
    
}
void timer(){
    // This struct is created to save information that will be needed by the timer,
    // such as socket file descriptors, frame numbers and video captures.
    struct send_frame_data {
        int socket_fd;
        // other fields
    };
    // This function will be called when the timer ticks
    void send_frame(union sigval sv_data) {
        struct send_frame_data *data = (struct send_frame_data *) sv_data.sival_ptr;
        // You may retrieve information from the caller using data->field_name
        // ...
    }// The following snippet is used to create and start a new timer that runs
    // every 40 ms.
    struct send_frame_data data; // Set fields as necessary
    struct sigevent play_event;
    timer_t play_timer;
    struct itimerspec play_interval;
    memset(&play_event, 0, sizeof(play_event));
    play_event.sigev_notify = SIGEV_THREAD;
    play_event.sigev_value.sival_ptr = &play_data;
    play_event.sigev_notify_function = send_frame;
    play_interval.it_interval.tv_sec = 0;
    play_interval.it_interval.tv_nsec = 40 * 1000000; // 40 ms in ns
    play_interval.it_value.tv_sec = 0;
    play_interval.it_value.tv_nsec = 1; // can't be zero
    timer_create(CLOCK_REALTIME, &play_event, &play_timer);
    timer_settime(play_timer, 0, &play_interval, NULL);
    // The following snippet is used to stop a currently running timer. The current
    // task is not interrupted, only future tasks are stopped.
    play_interval.it_interval.tv_sec = 0;
    play_interval.it_interval.tv_nsec = 0;
    play_interval.it_value.tv_sec = 0;
    play_interval.it_value.tv_nsec = 0;
    timer_settime(play_timer, 0, &play_interval, NULL);
    // The following line is used to delete a timer.
    timer_delete(play_timer);
}

*/

