/**************************************************
 * Joshua Reed
 * Oregon State EECS
 * Operating Systems
 * Program 4
 * File otp_enc
 * This reads a plaintext file and sends it to the 
 * respective daemon for encoding (via tcp sockets)
 *************************************************/

#include "oneheader.h" // function prototypes are in here

int main(int argc, char *argv[])
{
    if (argc < 3) 
	{ // Check that there is an appropriate amount of arguements for usage
       fprintf(stderr,"usage %s plaintextfile keytextfile portno\n", argv[0]);
       exit(0);
    }

	FILE *cipherTextFP,*keyTextFP; // File pointers
	
	/* Open files */
	cipherTextFP= fopen(argv[1],"r");	
	keyTextFP = fopen(argv[2],"r");	
	
	/* ERROR out if files don't open */
	if(cipherTextFP==NULL)error("ERROR opening plaintext file for input",2);
	if(keyTextFP==NULL)error("ERROR opening keytext file for input",2);
	
	/* This if statement checks that the key file is at least as big as the ptext file */	
	if(fSize(cipherTextFP)>fSize(keyTextFP))error("ERROR cipherText is larger than keyText",2);

	/* These if statements employ checkContents to check file for bad chars */
	if(checkContents(cipherTextFP))error("ERROR cipherText contains bad char data",2);
	if(checkContents(keyTextFP))error("ERROR cipherText contains bad char data",2);
	
	/* Setup socket */
	int sockfd = sockSetup(atoi(argv[3]));
	
	/* Authorize server by listening for one time code */
	auth(sockfd,"decAck"); // exit if not correct server

	/* Transmit key and ptext files */	
	sendFile(cipherTextFP,sockfd);
	sendFile(keyTextFP,sockfd);
	
	/* close open files */	
	fclose(cipherTextFP);
	fclose(keyTextFP);
	
	/* Receive decoded text */	
	char pText[290000];
	receive(pText,sockfd);
   	close(sockfd);

	printf("%s\n", pText);
    return (0);
}
