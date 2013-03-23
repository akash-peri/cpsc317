#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

void *serve_client(void *ptr) {
  // Converts ptr back to integer.
//  int client_fd = (int) (intptr_t) ptr;
  // ...
  return (void*)0;
}

void start_server(int port)
{

	int server_port = port;
	//this starts the server
	//listening on ports?
  
//flow: 
//first: we need to set up socket
//then, call accept to get client connections
//once accept returns, we call code below, then recall accept (it should be in some kind of loop)
    // ...
    // Required variables
    int client_fd = server_port;    // Socket returned by accept
    pthread_t thread; // Thread to be created
    // Creates the thread and calls the function serve_client.
    pthread_create(&thread, NULL, serve_client, (void *) (intptr_t) client_fd);
    // Detaches the thread. This means that, once the thread finishes, it is destroyed.
    pthread_detach(thread);
    // we then want to start a timer for this pthread
	//thread


	
}
