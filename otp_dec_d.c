/**************************************************
 * Joshua Reed
 * Oregon State EECS
 * Operating Systems
 * Program 4 (1/5)
 * File otp_enc_d.c
 * This is the decoding workhorse of the program.
 *************************************************/

#include "oneheader.h"

int alphNum(char); // Takes a capital char and returns number in alphabet (0-27)
int numAlph(int); // Takes an int (0-27) and returns a char A-' '
char codeChar(char, char); // takes a message char and a key char and returns a coded char
char decodeChar(char, char); // takes a codded char and a key char and returns a message char
int sendString(char [],int);
void codeString(char[],char[],char[]);
void decodeString(char[],char[],char[]);
int serviceClient(int);

int main(int argc, char *argv[])
{
    int sockfd, clientSockFD, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket", 2);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
             error("ERROR on binding",2);
    listen(sockfd,5);
	
	signal(SIGCHLD, SIG_IGN);
	while(1)
	{
		clilen=sizeof(cli_addr);
		clientSockFD = accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
		if(clientSockFD<0)error("ERROR on accpet",2);
		if(fork()==0)
		{ // I am the child
			sendAuth(clientSockFD,"encAck"); // send one time auth code
			serviceClient(clientSockFD);
			exit(0);
			close(clientSockFD);
		}
		else
		{ // I am the parent, and my work is done
			close(clientSockFD);
		}
	}
	
    close(sockfd);
    return (0); 
}



int serviceClient(int sockFD)
{
	/* Buffers */	
	char pText[290000];
	char keyText[290000];
	char cipherText[290000];
	/* These are rediculously large, 
     * but dynamic allocation
     * isn't a requirement of 
     * this assignment */	

	/* Recieve cipherText and keyText files */
	receive(cipherText,sockFD);
	receive(keyText,sockFD);

	/* Decode cipher */
	decodeString(pText,keyText,cipherText);

	/* Transmit the decoded text */
	sendString(pText,sockFD);	

	return (0);
}

/* Testcode for alphNum and numAlph
  	int i;
  	for(i=0;i<27;i++)	
  		printf("%c'",numAlph(i));
  		printf("\n");
  	for(i=0;i<27;i++)	
  		printf("%d'",alphNum(numAlph(i)));
 		printf("\n");
 */

/* Testcode for codeChar and decodeChar
  	int i;
  	printf("input char: \n");
 	for(i=0;i<27;i++)
 	{
 		printf("%c'",numAlph(i));
 		cipherLine[i] = codeChar(numAlph(i),keyLine[i]);
 	}
 	printf("\nkey char: \n");
 	for(i=0;i<27;i++)
 		printf("%c'",keyLine[i]);
 	printf("\ncoded char: \n");
 	for(i=0;i<27;i++)
 		printf("%c'",cipherLine[i]);
 	printf("\ndecoded char: \n");
 	for(i=0;i<27;i++)
 		printf("%c'",decodeChar(cipherLine[i],keyLine[i]));
  	printf("\n");
 */


