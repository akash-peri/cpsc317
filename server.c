#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>
#include <cv.h>
#include <highgui.h>

#define ALLOWED_CONNECTIONS 5

void *serve_client(void *ptr) {
  // Converts ptr back to integer.
//  int client_fd = (int) (intptr_t) ptr;
  // ...
  return (void*)0;
}

void start_server(int port)
{

//flow: 
//first: we need to set up socket
//then, call accept to get client connections
//once accept returns, we call code below, then recall accept (it should be in some kind of loop)

	int server_port = port;
	int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	typedef struct addrinfo *addr_info;
	typedef struct addrinfo hints;
		
	if(socketfd < 0)
	{
		perror ("socket");
        exit (EXIT_FAILURE);
	}
	
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	
	error = getaddrinfo(NULL, server_port, &hints, &addr_info);
	if(error != 0)
	{
		//error determining address info of server
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit (EXIT_FAILURE);
	}
	
	if(bind(sockfd, addr_info, sizeof(addr_info)) == -1)
	{
		exit (EXIT_FAILURE);
		perror ("binding failed");
	}
	
	if(listen(sockfd, ALLOWED_CONNECTIONS) == -1)
	{
		exit (EXIT_FAILURE);
		perror ("listening failed");
	}
	
	//finally, call accept
	
	struct addrinfo *client_addr_info;
	while(true)
	{
		int accept = accept(socketfd, &client_addr_info, sizeof(client_addr_info));
		
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
    int rv;
    int numaBytes;
    
    memset (&udpHints,0,sizeof udpHints);//initialize datagram struct, fill with binary zeros
    udpHints.ai_family = AF_UNSPEC;
    udpHints.ai_socktype = SOCK_DGRAM;

    // loop through all the results and make a socket
    // looks through serverinfo and looks for a port
    for(p = serverinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        
        break;
    }
    //check if we found a open socket
    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }
    
    //store datagram packets and send if exist
    if ((numBytes = sendto(sockfd, *PACK, LEN, 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
    
    //free the socket used
    freeaddrinfo(serverinfo);
    
    close(sockfd);
}

//should return encoded as jpeg packets to be sent through UDP
void load_video()
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

// After the call above, the encoded data is in encoded->data.ptr and has a length of encoded->cols bytes.

// Close the video file
cvReleaseCapture(&video);






