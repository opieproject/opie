/* décodage arithmétique 
 * optimisé pour une arithmétique 16 bits
 */
#include <stdlib.h>
#include "arith.h"


/*
 * Initialisation du décodeur.
 * bufsize doit être multiple de 2 et supérieur à 4
 */

void ArithClass::Arith_DecodeInit(PPM_ReadBuf* readbuf,UCHAR *buf,UINT bufsize) {

	 /* gestion buffer */
	 
	 ainbuf=buf;
	 ainbufsize=bufsize;
	 areadbuf=readbuf;
	 aendinbuf=ainbuf+ainbufsize;
	 areadbuf->readbuf(ainbuf,ainbufsize);
	 apinbuf=ainbuf;
	 
	 /* intervalle et position dans l'intervalle */
	 alow=0;
	 ahigh=0xFFFF;
	 avalue=(*apinbuf++)<<8;
	 avalue|=(*apinbuf++);
	 
	 /* remplissage du buffer 16 bits */
	 
	 abitbuf=(*apinbuf++)<<8;
	 abitbuf|=(*apinbuf++);
	 abitcnt=16;
}

#define DIV16(a,b)		( (UINT)(a)/(UINT)(b) )
#define MUL16(a,b)		( (UINT)(a)*(UINT)(b) )

/*
 * Décodage: première étape
 */

UINT ArithClass::Arith_DecodeVal(UINT asize)  {
	 USHORT range,c;

	 range=ahigh-alow+1;
	 c=avalue-alow+1;
	 if (range!=0) {
			if (c==0) return DIV16(((UINT)asize<<16)-1,range);
			else return DIV16(MUL16(c,asize)-1,range);
	 } else {
			if (c==0) return (asize-1);
			else return (MUL16(c,asize)-1)>>16;
	 }
}

/*
 * Décodage: deuxième étape
 */

void ArithClass::Arith_Decode(UINT amin,UINT amax,UINT asize) {
	 USHORT range;

	 range = ahigh - alow;
	 if (amax!=asize) ahigh=alow+DIV16(MUL16(range,amax)+amax,asize)-1;
	 if (amin!=0) alow+=DIV16(MUL16(range,amin)+amin,asize);
	 for ( ; ; ) {
			if ( alow>=0x4000 && ahigh<0xC000 ) {
				 avalue -= 0x4000;  alow -= 0x4000;  ahigh -= 0x4000;
			} else if ( ahigh>=0x8000 && alow<0x8000 ) break;
			
			alow+=alow;
			ahigh+=ahigh+1;
			avalue=(avalue<<1)|( (abitbuf&0x8000)!=0 );
			abitbuf<<=1;
			if ( (--abitcnt)==0 ) {
				 abitbuf=(*apinbuf++)<<8;
				 abitbuf|=(*apinbuf++);
				 abitcnt=16;
				 if (apinbuf>=aendinbuf) {
						areadbuf->readbuf(ainbuf,ainbufsize);
						apinbuf=ainbuf;
				 }
			}
	 }
}

/*
 * fin du décodage: rien à faire
 */

void ArithClass::Arith_DecodeEnd(void) {
}
