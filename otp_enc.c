/**************************************************
 * Joshua Reed
 * EECS OregonState
 * CS 344 Operating Systems
 * Program otp_enc.c
 * This program accepts a key file, a plaintext file,
 * and a port number. It reads these files and sends the 
 * info to otp_enc_d.c to be encoded
 *************************************************/

#include "oneheader.h" // function prototypes are in here

int main(int argc, char *argv[])
{
	/* Check for appropriate number arguements for usage */
    if (argc < 3)error("Usage otp_enc plaintextfile keytextfile portno\n",1);

	FILE *pTextFP,*keyTextFP; // File pointers
	
	/* Open files for reading */
	pTextFP = fopen(argv[1],"r");	
	keyTextFP = fopen(argv[2],"r");	
	
	/* Check that the files opened successfully */
	if(pTextFP==NULL)error("ERROR opening plaintext file for input",2);
	if(keyTextFP==NULL)error("ERROR opening keytext file for input",2);
	
	/* Check that the keytext file is at least as big as the plaintext file */	
	if(fSize(pTextFP)>fSize(keyTextFP))error("ERROR pText is larger than keyText",2);

	/* Check the files for out of range chars */
	if(checkContents(pTextFP))error("ERROR pText contains bad char data",2);
	if(checkContents(keyTextFP))error("ERROR pText contains bad char data",2);

	/* Setup a socket */
	int sockfd = sockSetup(atoi(argv[3]));

	/* Authorize server by listening for one time code */
	sendAuth(sockfd,"encAck"); // exits if not correct server	

	/* Transmit key and ptext files */	
	sendFile(pTextFP,sockfd);
	sendFile(keyTextFP,sockfd);
	
	/* close open files */	
	fclose(pTextFP);
	fclose(keyTextFP);
	
	/* Receive cipher text */	
	char cipherText[290000]; // This should be made dynamic in a professional program
	receive(cipherText,sockfd); 
    close(sockfd);

	printf("%s\n", cipherText);
    return (0);
}





























