/* ----------------------------------------------------------------------- 
 * Name            : rngs.h  (header file for the library file rngs.c) 
 * Author          : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * ----------------------------------------------------------------------- 
 */

#if !defined( _RNGS_ )
#define _RNGS_


#define nextInt(x) nextInteger( (x),__FILE__, __LINE__ )
//#define DEBUG_NEW new

double Random(void);
int nextInteger( int range,const char *file, int line);
void   PlantSeeds(long x);
void   GetSeed(long *x);
void   PutSeed(long x);
void   SelectStream(int index);
void   TestRandom(void);

#endif
