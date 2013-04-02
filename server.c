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
char *video_name;
char *scale_speed;
char *session_string;

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
	
	//if it doesnt end with a new line, invalid response
	if(parsed_data[line_counter][0] != '\n')
	{
		perror("the request wasnt formulated properly");
	}
	
	//at this point, we have our parsed data
	//now, we need to analyze it
	
	//get first word: should be one of SETUP, PLAY, PAUSE, TEARDOWN
	
	char SPACE = ' ';
	int valid = 1;
	
	int char_length = get_word_size_double_array(parsed_data, 0, 0, SPACE);
	int char_count = 0;
	char *control_string = (char *)malloc(char_length);
	set_word_double_array(parsed_data, control_string, 0, char_count, char_length);
	char_count += char_length;
	
	//control string can be SELECT, PLAY, PAUSE, TEARDOWN
	if(strncmp(control_string,"SELECT",7))
	{
		//get the video name now
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		char_count += char_length;
		char *movie_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, movie_string, 0, char_count, char_length);
		//get the rtsp format
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		char_count += char_length;
		char *rtsp_format = (char *)malloc(char_length);
		set_word_double_array(parsed_data, rtsp_format, 0, char_count, char_length);
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length;
		//This is for the sequence number
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length;
		char *cseq = (char *)malloc(char_length);
		set_word_double_array(parsed_data, cseq, 1, char_count, char_length);
		
		//goto third line, get port number
		
		//reset count
		char_count = 0;
		//This is for the Transport:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		//This is for the RTP/UDP;
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		transport_type = (char *)malloc(char_length);
		set_word_double_array(parsed_data, transport_type, 2, char_count, char_length);
		
		//This is for the interleaved=$a
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		char *interleaved_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, interleaved_string, 2, char_count, char_length);
		
		char_count = 0;
		
		//split the string into useable portions
		
		//(for interleaved=$a)
		char_length = get_word_size_single_array(interleaved_string, char_count, '$');
		char_count += char_length;
		//for a: check what the delimeter should be, space for now
		char_length = get_word_size_single_array(interleaved_string, char_count, '-');
		char_count += char_length;
		char *startpos_string = (char *)malloc(char_length);
		set_word_single_array(interleaved_string, startpos_string, char_count, char_length);
		
		//at this point, we have our char array, call video setup now
		
		//then call 
		
		//once video setup is done, formulate response
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, "200"); // the ok code
		strcat(return_array, "OK");
		strcat(return_array, "/n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "/n");
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
	}
	else if(strncmp(control_string,"PLAY",4))
	{
		//get the video name now
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		char_count += char_length;
		char *movie_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, movie_string, 0, char_count, char_length);
		//get the rtsp format
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		char_count += char_length;
		char *rtsp_format = (char *)malloc(char_length);
		set_word_double_array(parsed_data, rtsp_format, 0, char_count, char_length);
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length;
		//This is for the sequence number
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length;
		char *cseq = (char *)malloc(char_length);
		set_word_double_array(parsed_data, cseq, 1, char_count, char_length);
		
		//goto third line, for session number
		
		//reset count
		char_count = 0;
		//This is for the Session:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		//This is for the value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		session_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, session_string, 2, char_count, char_length);
		
		//goto fourth line, for Scale=1, etc
		//reset count
		char_count = 0;
		
		//This is for the scale
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		//This is for the scale value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length;
		char *scale_val = (char *)malloc(char_length);
		set_word_double_array(parsed_data, scale_val, 2, char_count, char_length);
		
		char_count = 0;
		
		//split the string into useable portions
		//at this point, we have our char array, start playing the video

		//we need to: make sure the video has already been opened (ie, check to make sure setup is already done, and it finished successfuly)
		//once that is done, send request to play, make sure you include scale as well, for the timer
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, "200"); // the ok code
		strcat(return_array, "OK");
		strcat(return_array, "/n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "/n");
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
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

int get_word_size_double_array(char **array, int line, int start_pos, char delimiter)
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

int get_word_size_single_array(char *array, int start_pos, char delimiter)
{
	int char_count = start_pos;
	int valid = 1;
	if(char_count >= sizeof(array)) // if we dont have enough space to read one char, then this is invalid
	{
		valid = 0;
	}
	while(valid)
	{
		char val = array[char_count++];
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
	
void set_word_double_array(char **array, char *destination, int line, int start_pos, int char_count)
{

	int j;
	for(j = 0; j < char_count; j++)
	{
		destination[j] = array[line][j + start_pos];
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

void set_word_single_array(char *array, char *destination, int start_pos, int char_count)
{

	int j;
	for(j = 0; j < char_count; j++)
	{
		destination[j] = array[j + start_pos];
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

	char *server_port = malloc(sizeof(int)*8+1);
	//itoa(port,server_port,10);
	sprintf(server_port,"%d",port);
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct addrinfo *addr_info, *rp;
	int sfd;
	struct addrinfo hints;
		
	if(sockfd < 0)
	{
		perror ("socket can't be made");
	}
	
	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
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
	
	for (rp = addr_info; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1)
            continue;

       if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

       close(sfd);
    }
	
	/*
	struct sockaddr_in addr_sock;
	memset(&addr_sock, 0, sizeof(struct sockaddr_in));
	addr_sock.sin_family = AF_INET;
	addr_sock.sin_port = htons(server_port);
	//get IP for server, put into dsr
	addr_sock.sin_addr = inet_pton(AF_INET, client, dst);
	*/
	
	/*
	if(bind(sockfd, addr_info->ai_addr, sizeof(addr_info->ai_addr)) == -1)
	{
		perror ("binding failed");
	}
	*/
	if (rp == NULL) {               /* No address succeeded */
        perror("Could not bind\n");
        exit(1);
    }
	
	
	if(listen(sockfd, ALLOWED_CONNECTIONS) == -1)
	{
		perror ("listening failed");
	}
	
	//Print out info on server host and port number
	
	//printf("Hostname: %s\n", addr_info->ai_flags);
	//printf("Hostname: %s\n", addr_info->ai_canonname);
	get_host_name();
	printf("Portnum: %s\n", server_port);
	freeaddrinfo(addr_info); 
	
	//finally, call accept
	
	struct sockaddr_storage client_addr_info;
	while(true)
	{
		socklen_t addr_size = sizeof(client_addr_info);
		int accept_con = accept(sockfd, (struct sockaddr *)&client_addr_info, &addr_size);
		if(accept_con < 0)
		{
			perror ("accept failed");
		}
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

void get_host_name()
{
	struct addrinfo hints, *info, *p;
	int gai_result;

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
		exit(1);
	}

	for(p = info; p != NULL; p = p->ai_next) {
		printf("Hostname: %s\n", p->ai_canonname);
	}

	freeaddrinfo(info);
}
                                  
/*


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

*/

