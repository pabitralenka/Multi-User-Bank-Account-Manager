#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAXFILE 500
int main(int argc, char **argv)
{

	if(argc < 4)
	{
		fprintf(stderr, "Usage client <hostname> <port number> <timestamp> <filename>\n");
		exit(1);
	}
	
	char buffer[MAXFILE];
	int x=0;
	char ch;
	char line[200];
	char *filename;
	filename=argv[4];

	//open the filename received from the command line
	FILE *myfile = fopen(filename, "r"); 
	if(myfile==NULL)
	{
		printf("Error opening file\n");
	}                                                              

	struct hostent *server;
	int portno;
	double sec;
	//extract values from command line arguements
	portno = atoi(argv[2]);
	sec = atof(argv[3]);
	server = gethostbyname(argv[1]);
	if(server == NULL)
	{
		fprintf(stderr, "No such host exists\n");
		exit(1);
	}

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);	


	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(socketfd < 0)
	{
		fprintf(stderr, "Socket not formed \n");
		exit(0);
	}

    double time;
    double initial;
    //connect to server socket
	if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "Error connecting \n");
	}

	x++;
	memset(buffer, 0, MAXFILE);
	int size=0;
	char string[MAXFILE];
    int transactions=0;
    
    //get each transaction and send it one by one
    while (fgets(buffer, MAXFILE, myfile) != NULL)
    {
    	transactions++;
    	initial=initial+1;
    	char *p;
    	p=strtok(buffer,"\n");
    	if(p)
    	{
    		strcpy(buffer,p);
    	}

    	size+=strlen(buffer);
    	//printf("%s %lu %d buffer \n", buffer,strlen(buffer),size);

    	char record[256];
    	strcpy(record,buffer);

    	//checking timestamp before sending
    	p=strtok(record," ");
    	if(p)
    	{
    		time=atoi(p);
    	}
    	//delay based on the timestamp provided in the file and the time already spent
    	
    	while(initial!=time)
    	{
    		if(initial>time)
    		{
    			sleep((initial-time)*sec);
    			initial=initial+1;
    			//printf("wait%f\n",(initial-time)*sec);
    		}

    		else
    		{
    			sleep((time-initial)*sec);
    			initial=initial+1;
    		}
    	
    	}

    	int n;
    	if(initial==time)
    	{
    		//initial=time;
    		printf("Sending----->> %s\n\n",buffer);
    		
			n = write(socketfd, buffer, sizeof(buffer));
			if(n < 0)
			{
				fprintf(stderr, "Error with writing to socket\n");
				exit(1);	
			}
			bzero(buffer, MAXFILE);
			//read the acknowledgement received from server
			n = read(socketfd, buffer, MAXFILE);
			printf("Message Received : %s\n",buffer);

	    }
	  	 	
    }	
	
	close(socketfd);
	exit(0); 


	return 0;
}

//IP addr - 172.17.7.74