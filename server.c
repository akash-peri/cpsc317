#include "server.h"


#define ALLOWED_CONNECTIONS 5
#define TRUE 1
#define FALSE 0

// This struct is created to save information that will be needed by the timer,
// such as socket file descriptors, frame numbers and video captures.

void *serve_client(void *ptr) {
  // Converts ptr back to integer.
  int client_fd = (int) (intptr_t) ptr;
  
  	struct create_timer_data timer_data;
  
  //loop until we want to close the program1
  while(TRUE)
  {
	//if data in socket
	
	char *data = (char *)malloc(4096*sizeof(char));
	char **parsed_data = (char **)malloc(5*sizeof(char*));
	char *count = (char *)malloc(5*sizeof(int));
	if(parsed_data)
	{
		int j = 0;
		for(j = 0; j < 4; j++)
		{
			parsed_data[j] = (char *)malloc(1024);
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
			char new_line = '\n';
			if(data_char == new_line)
			{
				count[line_counter] = char_counter;
				line_counter++;
				char_counter = 0;
			}
			else
			{
				parsed_data[line_counter][char_counter++] = data_char;
			}
		}
	}
	free(data);
	int j;
	for(j = 0; j < 4; j++)
	{
		parsed_data[j] = (char *) realloc (parsed_data[j], count[j]*sizeof(parsed_data[0][0]));
	}
	
	//if it doesnt end with a new line, invalid response
	if(parsed_data[line_counter] != NULL)
	{
		perror("the request wasnt formulated properly");
	}
	
	//at this point, we have our parsed data
	//now, we need to analyze it
	
	//get first word: should be one of SETUP, PLAY, PAUSE, TEARDOWN
	
	char SPACE = ' ';
	//char NEWLINE = '\n';
	char ENDOFARR = '\0';
	int valid = 1;
	
	int char_length = get_word_size_double_array(parsed_data, 0, 0, SPACE);
	int char_count = 0;
	char *control_string = (char *)malloc(char_length);
	memset(control_string, 0, char_length);
	set_word_double_array(parsed_data, control_string, 0, char_count, char_length);
	char_count += char_length + 1; // for space
	
	char *movie_string;
	char *rtsp_format;
	char *cseq;
	char *session_num;
	char *scale_value;
	
	if(strncmp(control_string,"SETUP",5) == 0)
	//control string can be SETUP, PLAY, PAUSE, TEARDOWN
	{
		//get the video name now
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		movie_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, movie_string, 0, char_count, char_length);
		char_count += char_length + 1;
		//get the rtsp format
		char_length = get_word_size_double_array(parsed_data, 0, char_count, ENDOFARR);
		rtsp_format = (char *)malloc(char_length);
		set_word_double_array(parsed_data, rtsp_format, 0, char_count, char_length);
		char_count += char_length + 1;
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the sequence number
		char_length = get_word_size_double_array(parsed_data, 1, char_count, ENDOFARR);
		cseq = (char *)malloc(char_length);
		set_word_double_array(parsed_data, cseq, 1, char_count, char_length);
		char_count += char_length + 1;
		
		//goto third line, get port number
		
		//reset count
		char_count = 0;
		//This is for the Transport:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the RTP/UDP;
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char *transport_type = (char *)malloc(char_length);
		set_word_double_array(parsed_data, transport_type, 2, char_count, char_length);
		char_count += char_length + 1;
		//This is for the interleaved=$a
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		char *interleaved_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, interleaved_string, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//split the interleaved string into useable portions
		
		//reset count
		char_count = 0;
		//(for interleaved=a)
		char_length = get_word_size_single_array(interleaved_string, char_count, '=');
		char_count += char_length + 1;
		//for a: check what the delimeter should be, space for now
		char_length = get_word_size_single_array(interleaved_string, char_count, ENDOFARR);
		char *startpos_string = (char *)malloc(char_length);
		set_word_single_array(interleaved_string, startpos_string, char_count, char_length);
		char_count += char_length + 1;
		
		//at this point, we have our char array, call video setup now
		int response = load_video(movie_string);
		if(response != 0)
		{
			perror("load failed");
		}
		create_timer(timer_data);
		
		//once video setup is done, formulate response
		
		free(movie_string);
		free(rtsp_format);
		free(cseq);
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, " 200"); // the ok code
		strcat(return_array, " OK");
		strcat(return_array, "\n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "\n");
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
		
		if(send(client_fd, (void *)return_array, 400, 0) < 0)
		{
			perror("send");
		}
	}
	else if(strncmp(control_string,"PLAY",4) == 0)
	{
		//get the video name now
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		movie_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, movie_string, 0, char_count, char_length);
		char_count += char_length + 1;
		//get the rtsp format
		char_length = get_word_size_double_array(parsed_data, 0, char_count, ENDOFARR);
		rtsp_format = (char *)malloc(char_length);
		set_word_double_array(parsed_data, rtsp_format, 0, char_count, char_length);
		char_count += char_length + 1;
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the sequence number
		char_length = get_word_size_double_array(parsed_data, 1, char_count, ENDOFARR);
		cseq = (char *)malloc(char_length);
		set_word_double_array(parsed_data, cseq, 1, char_count, char_length);
		char_count += char_length + 1;//get the video name now
		
		//goto third line, get port number
		
		//reset count
		char_count = 0;
		//This is for the Session:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the session value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		session_num = (char *)malloc(char_length);
		set_word_double_array(parsed_data, session_num, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//reset count
		char_count = 0;
		//This is for the Scale:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the Scale value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		scale_value = (char *)malloc(char_length);
		set_word_double_array(parsed_data, scale_value, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//at this point, we have our char array, call video play now
		
		free(movie_string);
		free(rtsp_format);
		free(cseq);
		free(session_num);
		free(scale_value);
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, " 200"); // the ok code
		strcat(return_array, " OK");
		strcat(return_array, "\n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "\n");
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
		
		if(send(client_fd, (void *)return_array, 400, 0) < 0)
		{
			perror("send");
		}
	}
	else if(strncmp(control_string,"PAUSE",5) == 0)
	{
		//get the video name now
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		movie_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, movie_string, 0, char_count, char_length);
		char_count += char_length + 1;
		//get the rtsp format
		char_length = get_word_size_double_array(parsed_data, 0, char_count, ENDOFARR);
		rtsp_format = (char *)malloc(char_length);
		set_word_double_array(parsed_data, rtsp_format, 0, char_count, char_length);
		char_count += char_length + 1;
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the sequence number
		char_length = get_word_size_double_array(parsed_data, 1, char_count, ENDOFARR);
		cseq = (char *)malloc(char_length);
		set_word_double_array(parsed_data, cseq, 1, char_count, char_length);
		char_count += char_length + 1;//get the video name now
		
		//goto third line, get port number
		
		//reset count
		char_count = 0;
		//This is for the Session:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the session value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		session_num = (char *)malloc(char_length);
		set_word_double_array(parsed_data, session_num, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//reset count
		char_count = 0;
		//This is for the Scale:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the Scale value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		scale_value = (char *)malloc(char_length);
		set_word_double_array(parsed_data, scale_value, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//at this point, we have our char array, call video pause now
		
		free(movie_string);
		free(rtsp_format);
		free(cseq);
		free(session_num);
		free(scale_value);
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, " 200"); // the ok code
		strcat(return_array, " OK");
		strcat(return_array, "\n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "\n");
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
		
		if(send(client_fd, (void *)return_array, 400, 0) < 0)
		{
			perror("send");
		}
	}
	else if(strncmp(control_string,"TEARDOWN",8) == 0)
	{
		//get the video name now
		char_length = get_word_size_double_array(parsed_data, 0, char_count, SPACE);
		movie_string = (char *)malloc(char_length);
		set_word_double_array(parsed_data, movie_string, 0, char_count, char_length);
		char_count += char_length + 1;
		//get the rtsp format
		char_length = get_word_size_double_array(parsed_data, 0, char_count, ENDOFARR);
		rtsp_format = (char *)malloc(char_length);
		set_word_double_array(parsed_data, rtsp_format, 0, char_count, char_length);
		char_count += char_length + 1;
		
		//goto second line, get sequence number
		
		//reset count
		char_count = 0;
		//This is for CSeq: 
		char_length = get_word_size_double_array(parsed_data, 1, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the sequence number
		char_length = get_word_size_double_array(parsed_data, 1, char_count, ENDOFARR);
		cseq = (char *)malloc(char_length);
		set_word_double_array(parsed_data, cseq, 1, char_count, char_length);
		char_count += char_length + 1;//get the video name now
		
		//goto third line, get port number
		
		//reset count
		char_count = 0;
		//This is for the Session:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the session value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		session_num = (char *)malloc(char_length);
		set_word_double_array(parsed_data, session_num, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//reset count
		char_count = 0;
		//This is for the Scale:
		char_length = get_word_size_double_array(parsed_data, 2, char_count, SPACE);
		char_count += char_length + 1;
		//This is for the Scale value
		char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
		scale_value = (char *)malloc(char_length);
		set_word_double_array(parsed_data, scale_value, 2, char_count, char_length);
		char_count += char_length + 1;
		
		//at this point, we have our char array, call video teardown now
		
		free(movie_string);
		free(rtsp_format);
		free(cseq);
		free(session_num);
		free(scale_value);
		
		//for now, assume it's the standard successful scenario
		char *return_array = (char *)malloc(400);
		strcpy(return_array, rtsp_format);
		strcat(return_array, " 200"); // the ok code
		strcat(return_array, " OK");
		strcat(return_array, "\n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, cseq);
		strcat(return_array, "\n");
		strcat(return_array, "Session: ");
		strcat(return_array, get_session_num());
		
		if(send(client_fd, (void *)return_array, 400, 0) < 0)
		{
			perror("send");
		}
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
	while(valid)
	{
		char val = array[line][char_count];
		if(val == delimiter)
		{
			break;
		}
		else if(char_count - start_pos> 128)
		{
			valid = 0;
			break;
		}
		char_count++;
	}
	
	return char_count - start_pos;
}

int get_word_size_single_array(char *array, int start_pos, char delimiter)
{
	int char_count = start_pos;
	int valid = 1;
	while(valid)
	{
		char val = array[char_count];
		if(val == delimiter)
		{
			break;
		}
		else if(char_count - start_pos > 128)
		{
			valid = 0;
			break;
		}
		char_count++;
	}
	
	return char_count - start_pos;
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
	//null terminated
	destination[j]='\0';
}

void set_word_single_array(char *array, char *destination, int start_pos, int char_count)
{

	int j;
	for(j = 0; j < char_count; j++)
	{
		destination[j] = array[j + start_pos];
	}
	destination[j]='\0';
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

	if (rp == NULL) {               /* No address succeeded */
        perror("Could not bind\n");
        exit(1);
    }
	
	
	if(listen(sfd, ALLOWED_CONNECTIONS) == -1)
	{
		perror ("listening failed");
	}
	
	//Print out info on server host and port number
	
	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1024);
	printf("Hostname: %s\n", hostname);
	printf("Portnum: %s\n", server_port);
	freeaddrinfo(addr_info); 
	
	//finally, call accept
	
	struct sockaddr_storage client_addr_info;
	while(true)
	{
		socklen_t addr_size = sizeof(client_addr_info);
		int accept_con = accept(sfd, (struct sockaddr *)&client_addr_info, &addr_size);
		if(accept_con < 0)
		{
			perror ("accept failed");
		}
		// Required variables
		pthread_t thread; // Thread to be created
		// Creates the thread and calls the function serve_client.
		pthread_create(&thread, NULL, serve_client, (void *) (intptr_t) accept_con);
		// Detaches the thread. This means that, once the thread finishes, it is destroyed.
		pthread_detach(thread);
		// we then want to start a timer for this pthread
		//thread
	}
}    



// This function will be called when the timer ticks
void send_frame(union sigval sv_data) {
  
  struct send_frame_data *data = (struct send_frame_data *) sv_data.sival_ptr;
  if(data != NULL)
  {
  
  }
  // You may retrieve information from the caller using data->field_name
  // ...
}

void create_timer(create_timer_data timer_data)
{
	// The following snippet is used to create a new timer that runs
	// every 40 ms.
	
	//struct send_frame_data data; // Set fields as necessary
	//struct sigevent play_event;
	//timer_t play_timer;
	//struct itimerspec play_interval;

	memset(&(timer_data.play_event), 0, sizeof(timer_data.play_event));
	timer_data.play_event.sigev_notify = SIGEV_THREAD;
	timer_data.play_event.sigev_value.sival_ptr = &(timer_data.data);
	timer_data.play_event.sigev_notify_function = send_frame;

	timer_create(CLOCK_REALTIME, &(timer_data.play_event), &(timer_data.play_timer));
	//start_timer(&(timer_data.play_interval), &(timer_data.play_timer));
	
	// The following line is used to delete a timer.
	//timer_delete(timer_data.play_timer);
}

void start_timer(struct itimerspec *play_interval, timer_t *play_timer)
{
	play_interval->it_interval.tv_sec = 0;
	play_interval->it_interval.tv_nsec = 40 * 1000000; // 40 ms in ns
	play_interval->it_value.tv_sec = 0;
	play_interval->it_value.tv_nsec = 1; // can't be zero
	timer_settime(play_timer, 0, play_interval, NULL);
} 

void stop_timer(struct itimerspec *play_interval, timer_t *play_timer)
{
	// The following snippet is used to stop a currently running timer. The current
	// task is not interrupted, only future tasks are stopped.
	play_interval->it_interval.tv_sec = 0;
	play_interval->it_interval.tv_nsec = 0;
	play_interval->it_value.tv_sec = 0;
	play_interval->it_value.tv_nsec = 0;
	timer_settime(play_timer, 0, play_interval, NULL);
}




 int load_video(char *filename){
 
 /*
 CvCapture *video;
 
 // Open the video file.
 video = cvCaptureFromFile(filename);
    
    if (!video) {
		 // The file doesn't exist or can't be captured as a video file.
		 return -1;
    }
	*/
	return 0;
 }
 
 /*
 
//timer needs to check if the return frame is NULL. NULL at end of frames.
char[] get_frame(){
    // Obtain the next frame from the video file
    
    IplImage *image;
    CvMat *encoded;
    char [] frames;
    
    image = cvQueryFrame(video);
    if (!image) {
        // Next frame doesn't exist or can't be obtained.
        return -2
    }
    
    // Position the video at a specific frame number position
    cvSetCaptureProperty(video, CV_CAP_PROP_POS_FRAMES, next_frame);
    
    
    // Encode the frame in JPEG format with JPEG quality 100%.
    const static int encodeParams[] = { CV_IMWRITE_JPEG_QUALITY, 100 };
    encoded = cvEncodeImage(".jpeg", image, encodeParams);
    
    //Loop through the jpeg and convert the 2D matrix into a char array of first matrix objects
    int i;
    for (i=0; i<encoded.data; i++) {
        frames[i] = cvGetReal1D(encoded.data,i);
    }
    return frames;
    
}


void close_video(char* filename){
    // Close the video file
    cvReleaseCapture(&filename);
}


*/