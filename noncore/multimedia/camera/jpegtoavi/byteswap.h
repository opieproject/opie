#ifndef _BYTESWAP_H_
#define _BYTESWAP_H_

#include <endian.h>
#include <sys/types.h>

#ifndef BYTE_ORDER
# error "Aiee: BYTE_ORDER not defined\n";
#endif

#define SWAP2(x) (((x>>8) & 0x00ff) |\
                  ((x<<8) & 0xff00))

#define SWAP4(x) (((x>>24) & 0x000000ff) |\
                  ((x>>8)  & 0x0000ff00) |\
                  ((x<<8)  & 0x00ff0000) |\
                  ((x<<24) & 0xff000000))

#if BYTE_ORDER==BIG_ENDIAN
# define LILEND2(a) SWAP2((a))
# define LILEND4(a) SWAP4((a))
# define BIGEND2(a) (a)
# define BIGEND4(a) (a)
#else
# define LILEND2(a) (a)
# define LILEND4(a) (a)
# define BIGEND2(a) SWAP2((a))
# define BIGEND4(a) SWAP4((a))
#endif

#endif
