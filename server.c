#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <signal.h>
#include <time.h>

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
#define BUFLEN 512 //define arbitrary buffer length
#define NPACK 10 //temp initialization of packets to be recieved.
#define PORT 1111 //temp port to be used for datagram.

//method to do error check
void checkErr(char *e);
{
    perror(e);
    exit(1);
}

//UDP connection
void udp_server(){
    
    
    struct sockaddr_in serverDgram;
    struct sockaddr_in clientDgram;
    int s;
    int i;
    int slen = sizeof(clientDgram); //size of data sent from client
    char buf[BUFLEN]; //set buffer length
    
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      checkErr("Socket Error");
    
    memset((char *) &serverDgram, 0, sizeof(serverDgram)); //initialize datagram struct, fill with binary zeros
    
    serverDgram.sin_family = AF_INET; //use internet addresses
    serverDgram.sin_port = htons(PORT); //used defined portnumber. with htons() which ensures byte order.
    serverDgram.sin_addr.s_addr = htonl(INADDR_ANY); //accept packets from any IP interface
    
   //bind socket s to address of serverDgram
    if (bind(s, &serverDgram, sizeof(serverDgram))==-1)
            checkErr("Binding Error");
    
    
    for (i=0; i<NPACK; i++) { //for every packet sent
        if (recvfrom(s, buf, BUFLEN, 0, &clientDgram, &slen)==-1) //recieve packet from s and store into buf, max size of BUFLEN. store info of the client into clientDgram.
             checkErr("recvfrom() error");
        //display info about the client and data in the packets with inet_ntoa. takes a stuct in_addr and converts it to a string.
        printf("Received packet from %s:%d\nData: %s\n\n",
        inet_ntoa(clientDgram.sin_addr), ntohs(clientDgram.sin_port), buf);
        }

    close(s);

}
