/*
   Simple program to emulate the "text" mode of supplying WEP keys.
   The procedure only uses the ASCII codes of the characters as WEP
   key bits (13 characters produces 104 bits as needed by 128-bit
   WEP). Such keys of course do not have much entropy, but that is
   a choice for the installation to make.
*/
#include <stdio.h>

int main(int argc, char** argv)
{
    int a=0,l;
    char* c=argv[1];
    if ( argc != 3 || !*c ) {
	fprintf(stderr, "Usage: %s textkey [length]\n",argv[0]);
	exit(1);
    }
    l = atoi(argv[2]);
    while (l) {
	if ( !*c ) c=argv[1];
	printf("%02x",*c);
	c++;
	if ( --l )
	    printf(":");
    }
    printf("\n");
}
