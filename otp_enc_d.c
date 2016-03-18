/**************************************************
 * Joshua Reed
 * EECS Oregon State
 * CS 344 Operating Systems
 * Program otc_enc_d.c
 * This program receives data from otc_enc.c and 
 * encodes it. It then writes the data to stdout with
 * the intent of that output being redirected to a file
 **************************************************/

#include "oneheader.h"
int serviceClient(int);

int main(int argc, char *argv[])
{
	/* Check port was provided */
    if(argc < 2)error("ERROR, no port provided\n",2);

    int clientSockFD; // client socket number
	int sockfd = servSockSetup(atoi(argv[1])); // setup socket

    socklen_t clilen; // Client address length 
	struct sockaddr_in cli_addr; // Client address
	signal(SIGCHLD, SIG_IGN);
	while(1)
	{
   		clilen = sizeof(cli_addr);
		clientSockFD = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
    	if (clientSockFD<0)error("ERROR on accept",2);
		if(fork()==0)
		{ // I am the child			
			auth(clientSockFD,"encAck"); // send one time auth code
			serviceClient(clientSockFD);
			exit(0);
			close(clientSockFD);
		}
		else
		{ // I'm parrent, and my work is done
			close(clientSockFD);
		}
	}
    
    close(sockfd);
    return (0); 
}

int serviceClient(int sockFD)
{
	/* Recieve Transmission */	
	char pText[290000];
	char keyText[290000];
	char cipherText[290000];
	
	receive(pText,sockFD);
	receive(keyText,sockFD);
	//printf("pText at serviceClient %s\n", pText);
	//printf("keyText at serviceClient %s\n", keyText);
	codeString(pText,keyText,cipherText);
	//printf("cipherText at serviceClient %s\n", cipherText);
	sendString(cipherText,sockFD);	
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
