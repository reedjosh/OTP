#include <stdio.h>  // Manipulate file and I/O streams: printf(), fprintf(), sprintf(),fputs(), fgets(), etc...
#include <stdlib.h> // atoi(), exit(), qsort(), rand(), etc...
#include <string.h> // strncat(), strcmp(), strncpy(), strlen(), strtok(), etc...
#include <dirent.h> // To use the DIR type. Includes: closedir(), opendir(), readdir(), etc...
#include <unistd.h> // PID, Signals, and Process Management
#include <sys/stat.h> // chmod(), mkdir(), mkfifo(), stat(), etc...
#include <time.h> // asctime(), clock(), time(), etc...
#include <assert.h> // assert(int expression)
#include <sys/wait.h> // used to wait for processes
#include <sys/types.h> // to work with file redirection
#include <fcntl.h> // to open files and redirect streams 
#include <signal.h> // To catch and react to signals
#include <sys/socket.h> // to use sockets
#include <netinet/in.h> // to work with ip
#include <netdb.h>

void error(const char *msg,int exVal)
{
	perror(msg);
	exit(exVal);
}

/*************************************************
 * int sockSetup(int portno);
 * Paramaters - 
 * portno: a number to open a port on 
 * Precondition: portno is a valid port number
 * Postcondition: as long as no errors occur, a
 * port is opened for use
 * Return Value: sockFD is an int that points to a
 * socket file descriptor
 ************************************************/
int sockSetup(int portno)
{
    int sockfd; // socket pointer
	struct sockaddr_in serv_addr; // server address struct
	struct hostent *server = gethostbyname("localhost"); // get host ip
	if (server == NULL)error("ERROR, no such host\n",2); // error if host not found
	
	/* Open a socket */	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0)error("ERROR opening socket",2); // error message if failure
	
	/* setup server address struct */ 
	bzero((char *) &serv_addr, sizeof(serv_addr)); // blank the mem for serv_addr
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length); // copy serv_addr ip into server->h_addr
	serv_addr.sin_port = htons(portno); // set portnumber from user input
	
	/* Connect to socket */	
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)error("ERROR connecting",2);
	return (sockfd);
}

/*************************************************
 * void auth(int sockFD);
 * Paramaters - 
 * sockFD: an integer that points to a socket FD
 * authCode: a string to use for authorization
 * Precondition: sockFD points to an opened socket
 * Postcondition: Either the server and the client
 * are authorized matches based upon the key "encACK" 
 * or both the server and client will exit with an
 * error code
 ************************************************/
void auth(int sockFD, const char * authCode)
{ 
	char buffer[256],badACK[256];
	int n;
	/* Transmission one, read data */
	bzero(buffer,256);
	n = read(sockFD,buffer,255);
   	if (n < 0) error("ERROR reading from socket in daemon",2);
 	strcpy(badACK,buffer);

  	//printf("auth() reading for %s received %s\n", authCode, badACK); 
	//	fflush(stdout);

	/* Transmission two, write "ack" */
	bzero(buffer,256);	
	strcpy(buffer,authCode);
   	n = write(sockFD,buffer,strlen(buffer));
   	if (n < 0) error("ERROR writing to socket in daemon",2);

  	//printf("auth() writing %s and received %s\n", authCode, badACK); 
	//	fflush(stdout);
	
	if(strcmp(badACK,authCode))
	{
		close(sockFD);

		//printf("auth() mismatch exiting\n");
		//fflush(stdout);

		error("ERROR cannot find server",2);
	}
}

void sendAuth(int sockFD, const char * authCode)
{
	char buffer[256];
	int n;
	/* Transmit auth */
	strcpy(buffer,authCode);
    n = write(sockFD,buffer,strlen(buffer));
    if(n < 0)error("ERROR writing to socket",2);

  		//printf("sendAuth() writing %s\n", authCode); 
	//	fflush(stdout);

	/* Receive ack */
	bzero(buffer,256);
	n = read(sockFD,buffer,255);
	if (n < 0)error("ERROR reading from socket",2);

  		//printf("sendAuth() checking authCode %s vs received %s \n", authCode, buffer); 
	//	fflush(stdout);

	if(strcmp(buffer,authCode))
	{
		close(sockFD);

  		//printf("sendAuth() mismatch; wanted %s received %s exiting\n", authCode, buffer); 
	//	fflush(stdout);

		error("ERROR bad auth client",2);
	}
}

/*************************************************
 * receive(char str[], int sockFD)
 * Paramaters - 
 * sockFD: an integer that points to a socket FD
 * str: atring to hold received data
 * Precondition: sockFD points to an opened socket
 * str is large enough for incoming data
 * Postcondition: data is stored in str
 ************************************************/
int receive(char str[], int sockFD)
{
	char buffer[256];
	int keepAlive = 1,n;
	while(keepAlive)
	{
		/* Transmission one, read data */
		bzero(buffer,256);
	    n = read(sockFD,buffer,255);
    	if (n < 0) error("ERROR reading from socket in daemon",2);
		if(strcmp(buffer,"exit")==0)keepAlive=0;
		else strcat(str,buffer); // Print data

		/* Transmission two, write "ack" */
		bzero(buffer,256);	
		strcpy(buffer,"ack");
    	n = write(sockFD,buffer,strlen(buffer));
    	if (n < 0) error("ERROR writing to socket in daemon",2);
	}
	return(0);
}

/*************************************************
 * int sendFile(FILE * fp, int sockFD)
 * Paramaters - 
 * fp: an open file pointer
 * sockFD: an open socket pointer
 * Precondition: sockFD points to an opened socket
 * fp is open for reading and at the begining of
 * the file
 * Postcondition: file is transmitted to server
 * Return Value: 0 on success
 ************************************************/
int sendFile(FILE * fp, int sockFD)
{
	int n;
	char buffer[256];
	/* Transmission of files */
    bzero(buffer,256);
	while(fgets(buffer,255,fp))
	{ // send pTextFile
		/* Transmit data */
    	n = write(sockFD,buffer,strlen(buffer));
    	if(n < 0)error("ERROR writing to socket",2);
    
		/* Receive acks */
		bzero(buffer,256);
		n = read(sockFD,buffer,255);
		if (n < 0)error("ERROR reading from socket",2);
	}
	
	/* Transmit exit */
	bzero(buffer,256);
	strcpy(buffer,"exit");
    n = write(sockFD,buffer,strlen(buffer));
    if(n < 0)error("ERROR writing to socket",2);
   
	/* Receive ack */
	bzero(buffer,256);
	n = read(sockFD,buffer,255);
	if (n < 0)error("ERROR reading from socket",2);
	return (0);
}

/*************************************************
 * int fSize(FILE * fp)
 * Paramaters - 
 * fp: an open file pointer
 * Precondition: fp is open for reading 
 * Postcondition: fp is open at beginning
 * Return Value: The file size in bytes
 ************************************************/
int fSize(FILE * fp)
{
	fseek(fp,0L,SEEK_END); 
	int sz = (int)ftell(fp);
	if(sz<0)error("ERROR could not get file size.",2);
	fseek(fp,0L,SEEK_SET);
	return (sz);
}

/*************************************************
 * int checkContents(FILE * fp)
 * Paramaters - 
 * fp: an open file pointer
 * Precondition: fp is open for reading and 
 * at the begining of the file
 * Postcondition: fp is open at beginning
 * Return Value: 0 if all chars are within proper
 * values, 1 if a bad char is found
 ************************************************/
int checkContents(FILE * fp) // checks file content for proper data, returns 1 if bad data
{ 
	if(fp==NULL)error("ERROR passed null fp to checkContents()",2);
	fseek(fp,0L,SEEK_SET); // move to file begining 
	int ch; // char as an int to be used with fgetc()
	while(!feof(fp))
	{ // loop through chars
		ch = fgetc(fp); // get one char
		if((ch<'A' || ch>'Z') && ch!=' ' && ch!='\n' && ch!=EOF) // if char is not A-Z or space
			return(1); // return bad char present
	}
	fseek(fp,0L,SEEK_SET); // move to file begining
	return(0); // return all good chars
}

int sendString(char str[], int sockFD)
{
	FILE * fp;
	fp = fmemopen(str,strlen(str)+1,"r");
	int n;
	char buffer[256];
	/* Transmission of files */
    bzero(buffer,256);
	while(fgets(buffer,255,fp))
	{ // send pTextFile
		/* Transmit data */
    	n = write(sockFD,buffer,strlen(buffer));
    	if(n < 0)error("ERROR writing to socket",2);
    
		/* Receive acks */
		bzero(buffer,256);
		n = read(sockFD,buffer,255);
		if (n < 0)error("ERROR reading from socket",2);
    
		bzero(buffer,256);
	}
	
	/* Transmit exit */
	strcpy(buffer,"exit");
    n = write(sockFD,buffer,strlen(buffer));
    if(n < 0)error("ERROR writing to socket",2);
   
	/* Receive ack */
	bzero(buffer,256);
	n = read(sockFD,buffer,255);
	if (n < 0)error("ERROR reading from socket",2);
	return (0);
	fclose(fp);
}

int numAlph(int num)
{
	if(num == 26)
		return (32);
	return (num + 65);
}

int alphNum(char aChar)
{
	if(aChar == 32)
		return (26);
	return (aChar - 65);
}



char codeChar(char letter,char key)
{
	char cipher;
	cipher = numAlph((alphNum(letter)+alphNum(key))%27);
	return (cipher);
}

char decodeChar(char cipher,char key)
{
	char decodedLetter;
	if((alphNum(cipher) - alphNum(key)) < 0)
		decodedLetter = numAlph(27+(alphNum(cipher)-alphNum(key)));
	else
		decodedLetter = numAlph((alphNum(cipher)-alphNum(key))%27);
	return (decodedLetter);
}

void codeString(char pText[],char keyText[], char cText[])
{
	int i = -1;
	do{
	i+=1;
	if(pText[i]!='\0' && pText[i]!='\n')cText[i]=codeChar(pText[i],keyText[i]);
	else cText[i] = '\0';
	}while(pText[i]!='\0');
}

void decodeString(char pText[],char keyText[], char cText[])
{
	int i = -1;
	do{
	i+=1;
	if(cText[i]!='\0' && cText[i]!='\n')pText[i]=decodeChar(cText[i],keyText[i]);
	else pText[i] = '\0';
	}while(cText[i]!='\0');
}

int servSockSetup(portno)
{
	int sockfd;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)error("ERROR opening socket", 2);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr))<0)error("ERROR on binding",2);
	listen(sockfd,5); // Create a client connection queue
	return (sockfd);
}


