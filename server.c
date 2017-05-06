#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include </usr/include/semaphore.h>
#define MAXDATASIZE 256

void *connhandler(void *);

char* buffer[MAXDATASIZE];
int lines = 0;
char* records[MAXDATASIZE];
int id[MAXDATASIZE], balance[MAXDATASIZE];
char* name[MAXDATASIZE];

int socketfd, newsockfd, portno,*new_sock;
struct sockaddr_in serveradd, clientaddr;

FILE* myfile;
int i = 0;
int accid, amount;
char op[2];
char contents[MAXDATASIZE];
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
int x = 0;
	

size_t len = 0;

int main(int argc, char** argv)
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: server <port_number>\n");
        exit(1);
    }

    //reading from records of accounts
    char ch;
     myfile = fopen("Records.txt", "r+");
    if (myfile == NULL) 
    {
        printf("Error opening file\n");
    }

    do 
    {
        ch = fgetc(myfile);
        if (ch == '\n')
            lines++;
    } while (ch != EOF);

    //printf("%d\n", lines);
    rewind(myfile);


    //fetching individual data from each record
    for (i = 0; i < lines; i++) 
    {
        records[i] = NULL;
        len = 0;
        getline(&records[i], &len, myfile);
        //printf("%s\n", records[i]);
        char* p;
        p = strtok(records[i], " ");

        if (p) 
        {
            id[i] = atoi(p);
            //printf("%s %d\n", p, id[i]);
        }

        p = strtok(NULL, " ");

        if (p) 
        {
            name[i] = p;
            //printf("%s\n", p);
        }

        p = strtok(NULL, " ");

        if (p) 
        {
            balance[i] = atoi(p);
           // printf("%s\n", p);
        }
    }

     /* First call to socket() function */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        fprintf(stderr, "Error creating a socket\n");

    /* Initialize socket structure */
    bzero((char*)&serveradd, sizeof(serveradd));

    portno = atoi(argv[1]);
    serveradd.sin_family = AF_INET;
    serveradd.sin_addr.s_addr = INADDR_ANY;
    serveradd.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(socketfd, (struct sockaddr*)&serveradd, sizeof(serveradd)) < 0) 
    {
        fprintf(stderr, "Error binding socket\n");
    }

    /* Now start listening for the clients, here process will
       go in sleep mode and will wait for the incoming connection
   */
    listen(socketfd, 5);

    socklen_t clientlen = sizeof(clientaddr);
    
    
    pthread_mutex_init(&mutex2,NULL);

    /* Accept actual connection from multiple clients */
    while (newsockfd = accept(socketfd, (struct sockaddr*)&clientaddr, &clientlen)) 
    {
        
       
        if (newsockfd < 0) 
        {
            fprintf(stderr, "Error accepting client request\n");
        }

        else
            printf("\n\nConnection Established\n\n");
        
        pthread_t p_thread;
		new_sock = malloc(1);
        *new_sock = newsockfd;

        //create a new thread for each client
        if(pthread_create(&p_thread , NULL ,  connhandler , (void*) new_sock) < 0)
        {
            printf("Could not create thread\n");
            return 1;
        }
    }

    //close the connection socket
    close(newsockfd);
    pthread_exit(NULL);

    //close the file
    fclose(myfile);

    //close the server socket
    close(socketfd);
    return 0;
}

//called for each client thread
void *connhandler(void *socket_desc)
{
    //Get the socket descriptor
     	x++;
        //printf("Client %d\n:", x);

        /* If connection is established then start communicating */
        bzero(buffer, MAXDATASIZE);
        int newsocket=*(int *)socket_desc;
        //receiving from client (transactions)
		printf("Thread %ld\n", pthread_self());
        bzero(contents, MAXDATASIZE);
        int n=0;
        while(n=read(newsocket, contents, MAXDATASIZE))
        {
            if (n < 0) 
            {
                fprintf(stderr, "Error in receiving data\n");
                exit(1);
            }

            if(strlen(contents)==0)
            {
            	continue;
            }

            printf("Data received from client: %s\n", contents);

            printf("Thread number %ld\n", pthread_self());

            //separating account id, transaction to be performed and the amount from the received data
            char* p;
            p = strtok(contents, " ");
            p = strtok(NULL, " ");

            if (p) 
            {
                accid = atoi(p);
                //printf("%d\n", accid);
            }

            p = strtok(NULL, " ");

            if (p) 
            {
                strcpy(op, p);
                //printf("%s\n", op);
            }

            p = strtok(NULL, " ");

            if (p) 
            {
                amount = atoi(p);
                //printf("%d\n", amount);
            }


            int size = 0;

            int exist = 0;

            //check whether id in the requested transaction exists in the database
            for (i = 0; i < lines; i++) 
            {
                if (i == 0) 
                {
                    size = 0;
                }
                else 
                {
                    size += (floor(log10(abs(id[i - 1]))) + 1) + strlen(name[i - 1]) + (floor(log10(abs(balance[i - 1]))) + 1) + 3;
                }
                if (accid == id[i]) 
                {
                	pthread_mutex_lock(&mutex2);
                    printf("This account id exists in the database\n");

                    //Withdrawal Operation
                    if (strcmp(op, "w") == 0) 
                    {

                        printf("Withdrawing amount\n");

                        if (balance[i] - amount > 0) 
                        {

                            balance[i] = balance[i] - amount;
                            printf("Amount balance %d\n", balance[i]);

                            //writing the newly calculated balance in the file
                            fseek(myfile, size, SEEK_SET); //set the stream pointer i bytes from the start.
                            fprintf(myfile, "%d %s %d\n", id[i], name[i], balance[i]);
                        	printf("Acknowldgement Sent\n\n");

                            /* Write a response to the client */
                            n = write(newsocket, "ACK: Amount withdrawn\n", 50);
                            if (n < 0)
                                fprintf(stderr, "Error writing to socket\n");

                        }

                        else 
                        {
                            //balance insufficient to perform the withdrawal
                            printf("Insufficient balance %d\n", balance[i]);
                        	printf("Acknowledgement Sent\n\n");

                            n = write(newsocket, "NACK: Insufficient balance\n", 50);
                            if (n < 0)
                                fprintf(stderr, "Error writing to socket\n");
                        }
                    }

                    //Deposit Operation
                    else if (strcmp(op, "d") == 0) 
                    {

                        printf("Depositing amount\n");
                        balance[i] = balance[i] + amount;
                        printf("Amount balance %d\n", balance[i]);

                        //writing the newly calculated balance in the file
                        fseek(myfile, size, SEEK_SET); //set the stream pointer i bytes from the start i.e. beginning of line
                        fprintf(myfile, "%d %s %d\n", id[i], name[i], balance[i]);
                        printf("Acknowledgement Sent\n\n");

                        /* Write a response to the client */
                        n = write(newsocket, "ACK: Amount deposited\n", 50);
                        if (n < 0)
                            fprintf(stderr, "Error writing to socket\n");
                    }

                    else 
                    {

                        printf("Diff value\n");
                        /* Write a response to the client */
                        n = write(newsocket, "NACK: Invalid transaction type\n", 50);
                        if (n < 0)
                            fprintf(stderr, "Error writing to socket\n");
                    }

                    exist = 1;

                    //release the lock
					pthread_mutex_unlock(&mutex2);
                    break;
                }

            }

            if (exist == 0) 
            {
                //the requested id is invalid
                printf("This account id does not exist in the rocrds\n\n");
                n = write(newsocket, "NACK\n", 4);
                if (n < 0)
                    fprintf(stderr, "Error writing to socket\n");
            }        	
         }

    //flush the contents of the buffer into the file
    fflush(myfile);
    
    return 0;
} 