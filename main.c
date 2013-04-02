#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

int port_number;

int main(int argc, char **argv)
{
    printf("This is output from my first program!\n");
	/*
	if(argc != 1)
	{
		return 1;
	}
	
	port_number = atoi(argv[1]);
	start_server(port_number);
	*/
	start_server(12346);
	
	//scanf("%i", portNumber);
	
    return 0;
}
