#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

struct head {
	int magic;
	int flags;
	int chlen;
	int funclen;
	int datalen;
};

int main( int argc, char* argv[]  ) {
	int fd, groesse;
	struct sockaddr_un unix_adr;
	struct head he;
	char* ch = "QPE/System";
	char* fun = "call()";
	int test;
	char te;

	
	if( ( fd = socket(PF_UNIX, SOCK_STREAM, 0 ) ) < 0 ){
		printf("Could not socket\n" ); return 0;
	}

	memset(&unix_adr, 0, sizeof(unix_adr ) );
	unix_adr.sun_family = AF_UNIX;
	strcpy(unix_adr.sun_path, "/home/ich/.opie.cop");
	groesse = sizeof(unix_adr.sun_family) + strlen(unix_adr.sun_path);

	connect(fd, (struct sockaddr*)&unix_adr, groesse );

	test= 260;
	//te = (char)test;
        //write(fd,&test,sizeof(test));

	
	he.magic = 47;
	he.flags = 5;
	he.chlen = strlen(ch);
	he.funclen = strlen(fun);
	he.datalen = 0;
	
	for(;;){
	   write(fd,&he,sizeof(he) );   
	   //write(fd,(char*)&he.magic,1);
	   write(fd,ch,strlen(ch) );
	   write(fd,fun,strlen(fun) );
	   sleep(5);
	};
	

	
}
