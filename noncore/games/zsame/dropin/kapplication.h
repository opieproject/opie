#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

static int _random() {
        static int init = false;
        if (!init) {
            unsigned int seed;
            init = true;
            int fd = ::open("/dev/urandom", O_RDONLY);
            if( fd<=0 || ::read(fd, &seed, sizeof(seed)) != sizeof(seed) ) {
                srand(getpid());
                seed = rand()+time(0);
            }
            if(fd>=0) close( fd );
            srand(seed);
        }
        return rand();
}
