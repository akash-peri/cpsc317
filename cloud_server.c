#include "cloud_server.h"

#define ALLOWED_CONNECTIONS 5
#define TRUE 1
#define FALSE 0

// This struct is created to save information that will be needed by the timer,
// such as socket file descriptors, frame numbers and video captures.

void *serve_client(void *ptr) {
  // Converts ptr back to integer.
  int client_fd = (int) (intptr_t) ptr;
  
  //struct create_timer_data timer_data;
  int state_of_client = UNINITIALIZED;
  int valid = 1;
  
  //loop until we want to close the program1
  while(TRUE)
  {
	//if data in socket
	
	char *socket_data = (char *)malloc(1024*sizeof(char));
	char **parsed_data = (char **)malloc(5*sizeof(char*));
	memset(parsed_data, 0, 5*sizeof(char*));
	char *count = (char *)malloc(5*sizeof(int));
	if(parsed_data)
	{
		int j = 0;
		for(j = 0; j < 5; j++)
		{
			parsed_data[j] = (char *)malloc(256);
			memset(parsed_data[j], 0, 256);
		}
	}
		
	int done = FALSE;
	int line_counter = 0;
	int char_counter = 0;
	while(done == 0)
	{
		valid = 1;
		int read_size = (int) recv(client_fd, socket_data, 1024, 0);
		//int read_size = (int) read(client_fd, data, 4096);
		if(read_size < 0)
		{
			//perror ("read failed");
		}
		//perror ("test");
		//parse data now
		
		if(read_size < 1024)
		{
			done = TRUE; // nothing else to read this time
		}
		
		//odd symbol is the first one, if that is the case, assume malformed request
		if(socket_data[0] == '@')
		{
			valid = 0;
			done = 1;
		}
		
        int j;
		for(j = 0; j < read_size; j++)
		{
			char new_line = '\n';
			char data_char = socket_data[j];
			if(data_char == new_line)
			{
				count[line_counter] = char_counter;
				line_counter++;
				char_counter = 0;
			}
			else if (data_char == '\r')
			{
				//do nothing, next char is a new line
				//this seems to happen exclusively in windows where instead of \n, we see \r\n
				//this is not part of the RFC specification, so it's handled in this roundabout way
			}
			else
			{
				parsed_data[line_counter][char_counter++] = data_char;
			}
		}
	}
	free(socket_data);
	free(count);
	
	if(valid)
	{
		int j;
		for(j = 0; j < 4; j++)
		{
			//parsed_data[j] = (char *) realloc (parsed_data[j], count[j]*sizeof(parsed_data[0][0]));
		}
	}
	
	//if it doesnt end with a new line, invalid response
	if(!valid || ((line_counter == 0) && ((parsed_data[line_counter] == NULL) || (parsed_data[line_counter][0] == '\0'))))
	{
		//perror("the request wasnt formulated properly");
	}
	else
	{
		//at this point, we have our parsed data
		//now, we need to analyze it
		//get first word: should be one of SETUP, PLAY, PAUSE, TEARDOWN
		
		
		char SPACE = ' ';
		//char NEWLINE = '\n';
		
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
		char *transport_type;
		char *interleaved_string;
		char *startpos_string;
		struct send_frame_data data; // Set fields as necessary
		data.socket_fd = client_fd;
		struct sigevent play_event;
		timer_t play_timer;
		struct itimerspec play_interval;
		struct response_data rdi;
		
		if(strncmp(control_string,"SETUP",5) == 0)
		//control string can be SETUP, PLAY, PAUSE, TEARDOWN
		{
			//check state first to see if we need to discard anything
			
			if(state_of_client != INIT && state_of_client != UNINITIALIZED)
			{
				perror("Setup can not be called in this state");
			}
			else
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
				
				//parse the video name into something more useable:
				char *movie_name = malloc(40);
				memset(movie_name, 0, 40);
				char *movie_tokens = strtok(movie_string, ":/");
				int count = 0;
				while(movie_tokens != NULL)
				{
					if(count == 0)
					{
						if(strncmp(movie_tokens, "cloud", 5) != 0)
						{
							//for error checking
						}
					}
					else
					{
						if(strncmp(movie_tokens, "distributed", 11) == 0)
						{
							strcat(movie_name, movie_tokens);
						}
						else if(strncmp(movie_tokens, "correct", 6) == 0)
						{
							strcat(movie_name, movie_tokens);
						}
					}
					count ++;
					//...
					movie_tokens = strtok(NULL, ":/");
				}
				movie_name = realloc(movie_name, strlen(movie_name));
				
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
				transport_type = (char *)malloc(char_length);
				set_word_double_array(parsed_data, transport_type, 2, char_count, char_length);
				char_count += char_length + 1;
				//This is for the interleaved=$a
				char_length = get_word_size_double_array(parsed_data, 2, char_count, ENDOFARR);
				interleaved_string = (char *)malloc(char_length);
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
				startpos_string = (char *)malloc(char_length);
				set_word_single_array(interleaved_string, startpos_string, char_count, char_length);
				char_count += char_length + 1;
				
				rdi.return_code = SUCCESS;
				rdi.return_msg = SUCCESS_MSG;
				/*
				if(!ret)
				{
					perror("load failed");
					rdi.return_code = NOT_FOUND;
					rdi.return_msg = NOT_FOUND_MSG;
				}
				*/
				data.video_name = movie_name;
				data.timestamp_start = 0;
				data.send_count = 0;
				data.frame_number = 0;

				memset(&play_event, 0, sizeof(play_event));
				play_event.sigev_notify = SIGEV_THREAD;
				play_event.sigev_value.sival_ptr = &data;
				play_event.sigev_notify_function = send_frame;

				timer_create(CLOCK_REALTIME, &play_event, &play_timer);
				
				state_of_client = READY;
				
				//once video setup is done, formulate response

				//for now, assume it's the standard successful scenario
                char *return_array = (char *)malloc(400);
				rdi.rtsp_format = rtsp_format;
				rdi.cseq = cseq;
				rdi.session = get_session_num();
				get_response(return_array, SETUP, rdi);
				
				if(send(client_fd, (void *)return_array, strlen(return_array), 0) < 0)
				{
					perror("send");
				}
				
				free(return_array);
				free(movie_string);
				free(rtsp_format);
				free(cseq);
				free(transport_type);
				free(interleaved_string);
				free(startpos_string);
				
			}
		}
		else if(strncmp(control_string,"PLAY",4) == 0)
		{
			if(state_of_client != READY)
			{
				perror("Play can not be called in this state");
				break;
			}
		
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
			//convert scale_value to int; 1 is default
			int scale = atoi(scale_value);
			if(scale < 1)
			{
				scale = 1;
			}
			data.scale = scale;
			data.cseq = atoi(cseq);
			data.play_interval = play_interval;
			data.play_timer = play_timer;
			start_timer(play_interval, play_timer);
			state_of_client = PLAYING;
            char *return_array = (char *)malloc(400);
			rdi.rtsp_format = rtsp_format;
			rdi.cseq = cseq;
			rdi.session = session_num;
			rdi.return_code = SUCCESS;
			rdi.return_msg = SUCCESS_MSG;
			get_response(return_array, PLAY, rdi);
			return_array = realloc(return_array, strlen(return_array));
			
			if(send(client_fd, (void *)return_array, strlen(return_array), 0) < 0)
			{
				perror("send");
			}
			
			free(return_array);
			free(movie_string);
			free(rtsp_format);
			free(cseq);
			free(session_num);
			free(scale_value);
		}
		else if(strncmp(control_string,"PAUSE",5) == 0)
		{
			if(state_of_client != PLAYING)
			{
				perror("Pause can not be called in this state");
				break;
			}
			
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
			
			/*
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
			*/

			data.cseq = atoi(cseq);
			//at this point, we have our char array, call video pause now
			stop_timer(play_interval, play_timer);
			state_of_client = READY;
			
			//for now, assume it's the standard successful scenario
			char *return_array = (char *)malloc(400);
			rdi.rtsp_format = rtsp_format;
			rdi.cseq = cseq;
			rdi.session = session_num;
			rdi.return_code = SUCCESS;
			rdi.return_msg = SUCCESS_MSG;
			get_response(return_array, SETUP, rdi);
			
			if(send(client_fd, (void *)return_array, strlen(return_array), 0) < 0)
			{
				perror("send");
			}
			
			free(movie_string);
			free(rtsp_format);
			free(cseq);
			free(session_num);
			//free(scale_value);
		}
		else if(strncmp(control_string,"TEARDOWN",8) == 0)
		{
			if(state_of_client == INIT)
			{
				perror("Teardown can not be called in this state");
				break;
			}
			
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
			state_of_client = INIT;
			
			//for now, assume it's the standard successful scenario
			char *return_array = (char *)malloc(400);
			rdi.rtsp_format = rtsp_format;
			rdi.cseq = cseq;
			rdi.session = session_num;
			rdi.return_code = SUCCESS;
			rdi.return_msg = SUCCESS_MSG;
			get_response(return_array, SETUP, rdi);
			
			if(send(client_fd, (void *)return_array, strlen(return_array), 0) < 0)
			{
				perror("send");
			}
			
			free(movie_string);
			free(rtsp_format);
			free(cseq);
			free(session_num);
			free(scale_value);
		}
		else
		{
			break;
		}
		free(control_string);
	}
	free(parsed_data);
  }
  return (void*)0;
}

int get_word_size_double_array(char **array, int line, int start_pos, char delimiter)
{
	int char_count = start_pos;
	int valid = 1;
	while(valid)
	{
		char val = array[line][char_count];
		if(val == delimiter || val == ENDOFARR)
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
	while(true)
	{
		char val = array[char_count];
		if(val == delimiter || val == ENDOFARR)
		{
			break;
		}
		else if(char_count - start_pos > 128)
		{
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
		//in this special case, we also need to check for \r, for windows (temp hack as it should only be executed on linux systems)
		if(array[line][j+start_pos] != '\r')
		{			
			destination[j] = array[line][j + start_pos];
		}
		else
		{
			destination[j] = '\0';
		}
	}
	//null terminated
	destination[j]='\0';
}

void set_word_single_array(char *array, char *destination, int start_pos, int char_count)
{

	int j;
	for(j = 0; j < char_count; j++)
	{
		//in this special case, we also need to check for \r, for windows (temp hack as it should only be executed on linux systems)
		if(array[j+start_pos] != '\r')
		{			
			destination[j] = array[j + start_pos];
		}
		else
		{
			destination[j] = '\0';
		}
	}
	destination[j]='\0';
}

char* get_response(char *return_array, int state, response_data rdi)
{

	perror("Work maybe?");
	
	strcpy(return_array, rdi.rtsp_format);
	strcat(return_array, rdi.return_code); // the ok code
	strcat(return_array, rdi.return_msg);
	if(state == SETUP)
	{
	
	}
	//if(strncmp(rdi.return_code, SUCCESS, 4) == 0)
	//{
		strcat(return_array, "\n");
		strcat(return_array, "CSeq: ");
		strcat(return_array, rdi.cseq);
		strcat(return_array, "\n");
		strcat(return_array, "Session: ");
		strcat(return_array, rdi.session);
	//}
	strcat(return_array, "\n");
	strcat(return_array, "\n");
	perror("Looks good");
	
	return return_array;
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
	
	char hostname[256];
	hostname[256] = '\0';
	gethostname(hostname, 256);
	printf("Hostname: %s\n", hostname);
	printf("Portnum: %s\n", server_port);
	freeaddrinfo(addr_info); 
	
	//finally, call accept
	
	struct sockaddr_storage client_addr_info;
	while(true)
	{
		socklen_t addr_size = sizeof(client_addr_info);
		int accept_con;
		again: 
			{
				accept_con = accept(sfd, (struct sockaddr *)&client_addr_info, &addr_size);
				if(accept_con < 0)
				{
					if(errno == EINTR)
					{
						goto again;
					}
					perror ("accept failed");
				}
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
	
	printf("Sending frame now\n");
	struct send_frame_data *data = (struct send_frame_data *) sv_data.sival_ptr;
	if(data != NULL)
	{
		const struct cloud_server *server_info = get_cloud_server((const char*)data->video_name, data->frame_number);
		//data->frame_number += 1;
		
		if(server_info != NULL)
		{
			struct addrinfo hints, *res;
			int sockfd;
			int ret;

			// first, load up address structs with getaddrinfo():

			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			
			char *port_num = malloc(10);
			sprintf(port_num, "%d", server_info->port);
			
			printf("Getting addrinfo now\n");
			ret = getaddrinfo(server_info->server, port_num, &hints, &res);
			if(ret != 0)
			{
				perror("getaddrinfo failed");
				ret = 0;
			}
			free(port_num);
			printf("Making socket now\n");
			sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			printf("Connecting now");
			ret = connect(sockfd, res->ai_addr, res->ai_addrlen);
			printf("Connected to socket\n");
			if(ret != 0)
			{
				perror ("could not connect to socket");
			}
			else
			{
				printf("Assemble header\n");
				char *payload;
				char *frame_num = malloc(10);
				sprintf(frame_num, "%d", data->frame_number);
			
				char *return_array = malloc(100);
				memset(return_array, 0, 100);
				strcat(return_array, data->video_name);
				strcat(return_array, ":");
				strcat(return_array, frame_num);
				strcat(return_array, "\n");
				return_array = realloc(return_array, strlen(return_array));
				
				free(frame_num);
				
				printf("Send request\n");
				if(send(sockfd, (void *)return_array, strlen(return_array), 0) < 0)
				{
					perror("send");	
				}
				free(return_array);
				char *payload_size_arr = malloc(5);
				
				printf("Read response\n");
				int read_size = (int) recv(sockfd, payload_size_arr, 5, 0);
				if(read_size != 5)
				{
					perror("cloud server sent wrong size format");
				}
				
				int payload_size = atoi(payload_size_arr);
				printf("Payload arr: %s size: %d", payload_size_arr, payload_size);
				free(payload_size_arr);
				if(payload_size < 0)
				{
					perror("invalid size returned by cloud server");
				}
				else if(payload_size == 0)
				{
					//empty frame, stop timer
					//stop_timer(data->play_interval, data->play_timer);
				}
				else
				{
					payload = malloc(payload_size);
					int retrieved = 0;
					
					printf("Start getting data\n");
					while(retrieved < payload_size)
					{
						printf("Keep getting data\n");
						int read_size = (int) recv(sockfd, payload + retrieved, payload_size - retrieved, 0);
						retrieved += read_size;
					}
					
					unsigned char *rtsp_prefix = malloc(4 + 12); //
					memset(rtsp_prefix,0,16);
					unsigned char *rtsp_interleave = malloc(1);
					rtsp_interleave[0] = 0x24;
					
					//assemble header info
					char version = 1; //this should be 2, so technically, this is 10
					char padding = 0;
					char extension = 0;
					char crsc = 0;
					char firstByte = (version << 7) + (padding << 5) + (extension << 4) + crsc;
					char secondByte = 26; // no marker, so just payload
					char thirdByte = (data->cseq)/256;
					char fourthByte = (data->cseq)%256;
					data->send_count++;
					char *rtsp_temp = malloc(4);
					sprintf(rtsp_temp,"%i",data->timestamp_start + (40*data->send_count));

					rtsp_prefix[0] = 0x24;
					rtsp_prefix[1] = '0';
					rtsp_prefix[2] = (payload_size + 12)/256;
					rtsp_prefix[3] = (payload_size + 12)%256;
					rtsp_prefix[4] = firstByte;
					rtsp_prefix[5] = secondByte;
					rtsp_prefix[6] = thirdByte;
					rtsp_prefix[7] = fourthByte;
					rtsp_prefix[8] = rtsp_temp[0];
					rtsp_prefix[9] = rtsp_temp[1];
					rtsp_prefix[10] = rtsp_temp[2];
					rtsp_prefix[11] = rtsp_temp[3];
					rtsp_prefix[12] = '0';
					rtsp_prefix[13] = '0';
					rtsp_prefix[14] = '0';
					rtsp_prefix[15] = '0';
					
					printf("Sending data now\n");
					if(send(data->socket_fd, (void *)rtsp_prefix, 16, 0) < 0)
					{
						perror("send");	
					}
					printf("Halfway there\n");
					if(sendall(data->socket_fd, (void *)payload, payload_size) < 0)
					{
						perror("send");
					}
					printf("Data sent\n");
					
					free(rtsp_prefix);
					free(rtsp_interleave);
					free(rtsp_temp);
					free(payload);
				}
			}
			freeaddrinfo(res);
			close(sockfd);
		}
		else
		{
			perror("server not found");
		}
		data->frame_number += data->scale;
	}
}

int sendall(int s, char *buf, int len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;

    while(total < len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

void start_timer(struct itimerspec play_interval, timer_t play_timer)
{
	play_interval.it_interval.tv_sec = 0;
	play_interval.it_interval.tv_nsec = 40 * 1000000; // 40 ms in ns
	play_interval.it_value.tv_sec = 0;
	play_interval.it_value.tv_nsec = 1; // can't be zero
	timer_settime(play_timer, 0, &play_interval, NULL);
} 

void stop_timer(struct itimerspec play_interval, timer_t play_timer)
{
	// The following snippet is used to stop a currently running timer. The current
	// task is not interrupted, only future tasks are stopped.
	play_interval.it_interval.tv_sec = 0;
	play_interval.it_interval.tv_nsec = 0;
	play_interval.it_value.tv_sec = 0;
	play_interval.it_value.tv_nsec = 0;
	timer_settime(play_timer, 0, &play_interval, NULL);
}
 
//timer needs to check if the return frame is NULL. NULL at end of frames.
CvMat* get_encoded(CvCapture *video, int scale){
    // Obtain the next frame from the video file
    
    IplImage *image;
    CvMat *encoded;
	
	if(scale > 1)
	{
		int j;
		for(j = 0; j < scale - 1; j++)
		{
			cvGrabFrame(video);
		}
	}
	
    image = cvQueryFrame(video);
    if (!image) {
        // Next frame doesn't exist or can't be obtained.
        return NULL;
    }
	
	// Convert the frame to a smaller size (WIDTH x HEIGHT)
	CvMat *thumb = cvCreateMat(360, 480, CV_8UC3);
	cvResize(image, thumb, CV_INTER_AREA);
    
	
	//if scale > 1, moves frames forward to reflect that
	//int frameNum = cvGetCaptureProperty(video, CV_CAP_PROP_POS_FRAMES);
    // Position the video at a specific frame number position
    //cvSetCaptureProperty(video, CV_CAP_PROP_POS_FRAMES, frameNum + scale - 1);
    
    
    // Encode the frame in JPEG format with JPEG quality 100%.
    const int encodeParams[] = { CV_IMWRITE_JPEG_QUALITY, 30 };
    encoded = cvEncodeImage(".jpeg", image, encodeParams);
    
	return encoded;
}

void close_video(CvCapture* video){
    // Close the video file
    cvReleaseCapture(&video);
}


