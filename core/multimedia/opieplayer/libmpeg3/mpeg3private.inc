#ifndef LIBMPEG3_INC
#define LIBMPEG3_INC

#ifdef _WIN32

// Disable some compiler warnings that happen a lot but don't matter
#pragma warning( disable : 4003 ) // not enough parameters for macro
#pragma warning( disable : 4305 ) // truncation frm double to float
#pragma warning( disable : 4715 ) // not all control paths return a value
#pragma warning( disable : 4716 ) // must return a value

#ifdef LIBMPEG_EXPORTS
#define LIBMPEG_EXPORT		    __declspec( dllexport )
#else
#define LIBMPEG_EXPORT		    __declspec( dllimport )
#endif

#ifdef ERROR
#undef ERROR
#include <windows.h>
#undef ERROR
#define ERROR (-1)
#else
#include <windows.h>
#undef ERROR
#endif

#define inline			    __inline
#define M_PI			    3.14159265358979323846
#define M_SQRT2			    1.41421356237309504880

#define pthread_mutexattr_t	    int
#define pthread_mutexattr_init(a)   // Nothing
#define pthread_mutex_t		    CRITICAL_SECTION
#define pthread_mutex_init(a,b)	    InitializeCriticalSection(a)
#define pthread_mutex_lock(a)	    EnterCriticalSection(a)
#define pthread_mutex_unlock(a)	    LeaveCriticalSection(a)
#define pthread_mutex_destroy(a)    //DeleteCriticalSection(a)

#define pthread_attr_t		    int
#define pthread_attr_init(a)	    // Nothing
#define pthread_t		    unsigned long
#define pthread_create(a,b,c,d)	    *(a) = _beginthread(c,0,d)
//#define pthread_join(a,b)	    _endthread(b)
//#define pthread_join(a,b)	    _cwait(NULL,b,NULL)
#define pthread_join(a,b)

#define strncasecmp(a,b,c)	    _strnicmp(a,b,c)
#define strcasecmp(a,b)		    _stricmp(a,b)
#define bzero(a,b)		    memset(a,0,b)

#else

#define LONGLONG		    long long
#define ULONGLONG		    unsigned long long

#endif

#ifndef LIBMPEG_EXPORT
#define LIBMPEG_EXPORT
#endif

#define MPEG3_FLOAT32 mpeg3_real_t
#define MPEG3_INT16   short int
#define MPEG3_INT32   int
#define MPEG3_INT64   long

#define MPEG3_TOC_PREFIX                 0x544f4356
#define MPEG3_TOC_PREFIXLOWER            0x746f6376
#define MPEG3_ID3_PREFIX                 0x494433
#define MPEG3_RIFF_CODE                  0x52494646
#define MPEG3_PROC_CPUINFO               "/proc/cpuinfo"
#define MPEG3_TS_PACKET_SIZE             188
#define MPEG3_DVD_PACKET_SIZE            0x800
#define MPEG3_SYNC_BYTE                  0x47
#define MPEG3_PACK_START_CODE            0x000001ba
#define MPEG3_SEQUENCE_START_CODE        0x000001b3
#define MPEG3_SEQUENCE_END_CODE          0x000001b7
#define MPEG3_SYSTEM_START_CODE          0x000001bb
#define MPEG3_STRLEN                     1024
#define MPEG3_PIDMAX                     20 /* Maximum number of PIDs in one stream */
#define MPEG3_PROGRAM_ASSOCIATION_TABLE  0x00
#define MPEG3_CONDITIONAL_ACCESS_TABLE   0x01
#define MPEG3_PACKET_START_CODE_PREFIX   0x000001
#define MPEG3_PRIVATE_STREAM_2           0xbf
#define MPEG3_PADDING_STREAM             0xbe
#define MPEG3_GOP_START_CODE             0x000001b8
#define MPEG3_PICTURE_START_CODE         0x00000100
#define MPEG3_EXT_START_CODE             0x000001b5
#define MPEG3_USER_START_CODE            0x000001b2
#define MPEG3_SLICE_MIN_START            0x00000101
#define MPEG3_SLICE_MAX_START            0x000001af
#define MPEG3_AC3_START_CODE             0x0b77
#define MPEG3_PCM_START_CODE             0x0180
#define MPEG3_MAX_CPUS                   256
#define MPEG3_MAX_STREAMS                256
#define MPEG3_MAX_PACKSIZE               262144
#define MPEG3_CONTIGUOUS_THRESHOLD       10  /* Positive difference before declaring timecodes discontinuous */
#define MPEG3_PROGRAM_THRESHOLD          5   /* Minimum number of seconds before interleaving programs */
#define MPEG3_SEEK_THRESHOLD             16  /* Number of frames difference before absolute seeking */

/* Values for audio format */
#define AUDIO_UNKNOWN 0
#define AUDIO_MPEG 1
#define AUDIO_AC3  2
#define AUDIO_PCM  3
#define AUDIO_AAC  4
#define AUDIO_JESUS  5

#endif
