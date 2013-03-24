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
	struct addrinfo *addr_info;
	struct addrinfo hints;
		
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
