/**************************************************
 * Joshua Reed
 * Oregon State EECS
 * Operating Systems
 * Program 4 (1/5)
 * File keygen.c
 * This program generates a key "pad" for otc use of 
 * the length given at startup
 *************************************************/

#include "oneheader.h" // basic includes in oneheader.h file

int main(int argc, char *argv[])
{
	if(argc > 1)
	{ // User entered arguement for key length	
		if(atoi(argv[1]))
		{ // argument is valid
			
			/* Ititialize rand num generator */	
			time_t t;
			srand((unsigned) time(&t));

			/* Print n random numbers */	
			int i;
			for(i=0;i<atoi(argv[1]);i++)
			{
				printf("%c", numAlph(rand()%27));
			}
			

			return (0);
		}
	}
	printf("bad input\n");
	return (1);
}

