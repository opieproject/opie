#ifndef __BSWAP_H__
#define __BSWAP_H__

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#else
#include <inttypes.h>


#ifdef ARCH_X86
inline static unsigned short ByteSwap16(unsigned short x)
{
  __asm("xchgb %b0,%h0"	:
        "=q" (x)	:
        "0" (x));
    return x;
}
#define bswap_16(x) ByteSwap16(x)

inline static unsigned int ByteSwap32(unsigned int x)
{
#if __CPU__ > 386
 __asm("bswap	%0":
      "=r" (x)     :
#else
 __asm("xchgb	%b0,%h0\n"
      "	rorl	$16,%0\n"
      "	xchgb	%b0,%h0":
      "=q" (x)		:
#endif
      "0" (x));
  return x;
}
#define bswap_32(x) ByteSwap32(x)

inline static unsigned long long int ByteSwap64(unsigned long long int x)
{
  register union { __extension__ unsigned long long int __ll;
          unsigned long int __l[2]; } __x;
  asm("xchgl	%0,%1":
      "=r"(__x.__l[0]),"=r"(__x.__l[1]):
      "0"(bswap_32((unsigned long)x)),"1"(bswap_32((unsigned long)(x>>32))));
  return __x.__ll;
}
#define bswap_64(x) ByteSwap64(x)

#else

#define bswap_16(x) (((x) & 0x00ff) << 8 | ((x) & 0xff00) >> 8)

#define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#ifdef __GNUC__
/* code from bits/byteswap.h (C) 1997, 1998 Free Software Foundation, Inc. */
#define bswap_64(x) \
     (__extension__						\
      ({ union { __extension__ unsigned long long int __ll;	\
                 unsigned long int __l[2]; } __w, __r;		\
         __w.__ll = (x);					\
         __r.__l[0] = bswap_32 (__w.__l[1]);			\
         __r.__l[1] = bswap_32 (__w.__l[0]);			\
         __r.__ll; }))
#else
#define bswap_64(x) \
     ((((x) & 0xff00000000000000LL) >> 56) | \
      (((x) & 0x00ff000000000000LL) >> 40) | \
      (((x) & 0x0000ff0000000000LL) >> 24) | \
      (((x) & 0x000000ff00000000LL) >>  8) | \
      (((x) & 0x00000000ff000000LL) <<  8) | \
      (((x) & 0x0000000000ff0000LL) << 24) | \
      (((x) & 0x000000000000ff00LL) << 40) | \
      (((x) & 0x00000000000000ffLL) << 56)) 
#endif  /* !__GNUC__ */

#endif	/* !ARCH_X86 */

#endif	/* !HAVE_BYTESWAP_H */

/* be2me ... BigEndian to MachineEndian */
/* le2me ... LittleEndian to MachineEndian */

#ifdef WORDS_BIGENDIAN
#define be2me_16(x) (x)
#define be2me_32(x) (x)
#define be2me_64(x) (x)
#define le2me_16(x) bswap_16(x)
#define le2me_32(x) bswap_32(x)
#define le2me_64(x) bswap_64(x)
#else
#define be2me_16(x) bswap_16(x)
#define be2me_32(x) bswap_32(x)
#define be2me_64(x) bswap_64(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)
#define le2me_64(x) (x)
#endif

#define ABE_16(x) (be2me_16(*(uint16_t*)(x)))
#define ABE_32(x) (be2me_32(*(uint32_t*)(x)))
#define ABE_64(x) (be2me_64(*(uint64_t*)(x)))
#define ALE_16(x) (le2me_16(*(uint16_t*)(x)))
#define ALE_32(x) (le2me_32(*(uint32_t*)(x)))
#define ALE_64(x) (le2me_64(*(uint64_t*)(x)))

#ifdef ARCH_X86

#define BE_16(x) ABE_16(x)
#define BE_32(x) ABE_32(x)
#define BE_64(x) ABE_64(x)
#define LE_16(x) ALE_16(x)
#define LE_32(x) ALE_32(x)
#define LE_64(x) ALE_64(x)

#else

#define BE_16(x) ((((uint8_t*)(x))[0] << 8) | ((uint8_t*)(x))[1])
#define BE_32(x) ((((uint8_t*)(x))[0] << 24) | \
                  (((uint8_t*)(x))[1] << 16) | \
                  (((uint8_t*)(x))[2] << 8) | \
                   ((uint8_t*)(x))[3])
#define BE_64(x) ((uint64_t)(((uint8_t*)(x))[0] << 56) | \
                  (uint64_t)(((uint8_t*)(x))[1] << 48) | \
                  (uint64_t)(((uint8_t*)(x))[2] << 40) | \
                  (uint64_t)(((uint8_t*)(x))[3] << 32) | \
                  (uint64_t)(((uint8_t*)(x))[4] << 24) | \
                  (uint64_t)(((uint8_t*)(x))[5] << 16) | \
                  (uint64_t)(((uint8_t*)(x))[6] << 8) | \
                  (uint64_t)((uint8_t*)(x))[7])
#define LE_16(x) ((((uint8_t*)(x))[1] << 8) | ((uint8_t*)(x))[0])
#define LE_32(x) ((((uint8_t*)(x))[3] << 24) | \
                  (((uint8_t*)(x))[2] << 16) | \
                  (((uint8_t*)(x))[1] << 8) | \
                   ((uint8_t*)(x))[0])
#define LE_64(x) ((uint64_t)(((uint8_t*)(x))[7] << 56) | \
                  (uint64_t)(((uint8_t*)(x))[6] << 48) | \
                  (uint64_t)(((uint8_t*)(x))[5] << 40) | \
                  (uint64_t)(((uint8_t*)(x))[4] << 32) | \
                  (uint64_t)(((uint8_t*)(x))[3] << 24) | \
                  (uint64_t)(((uint8_t*)(x))[2] << 16) | \
                  (uint64_t)(((uint8_t*)(x))[1] << 8) | \
                  (uint64_t)((uint8_t*)(x))[0])

#endif /* !ARCH_X86 */

#ifdef WORDS_BIGENDIAN
#define ME_16(x) BE_16(x)
#define ME_32(x) BE_32(x)
#define ME_64(x) BE_64(x)
#define AME_16(x) ABE_16(x)
#define AME_32(x) ABE_32(x)
#define AME_64(x) ABE_64(x)
#else
#define ME_16(x) LE_16(x)
#define ME_32(x) LE_32(x)
#define ME_64(x) LE_64(x)
#define AME_16(x) ALE_16(x)
#define AME_32(x) ALE_32(x)
#define AME_64(x) ALE_64(x)
#endif

#define BE_FOURCC( ch0, ch1, ch2, ch3 )             \
        ( (uint32_t)(unsigned char)(ch3) |          \
        ( (uint32_t)(unsigned char)(ch2) << 8 ) |   \
        ( (uint32_t)(unsigned char)(ch1) << 16 ) |  \
        ( (uint32_t)(unsigned char)(ch0) << 24 ) )

#define LE_FOURCC( ch0, ch1, ch2, ch3 )             \
        ( (uint32_t)(unsigned char)(ch0) |          \
        ( (uint32_t)(unsigned char)(ch1) << 8 ) |   \
        ( (uint32_t)(unsigned char)(ch2) << 16 ) |  \
        ( (uint32_t)(unsigned char)(ch3) << 24 ) )
        
#ifdef WORDS_BIGENDIAN
#define ME_FOURCC BE_FOURCC
#else
#define ME_FOURCC LE_FOURCC
#endif

#endif
