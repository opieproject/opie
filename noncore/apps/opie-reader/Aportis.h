/*
  Derived from makedoc9 by Pat Beirne
*/

#ifndef __Aportis_h
#define __Aportis_h

#include "CExpander.h"
#include "pdb.h"

typedef UInt32 DWORD;
typedef UInt16 WORD;

#define DISP_BITS 11
#define COUNT_BITS 3
/*
// all numbers in these structs are big-endian, MAC format
struct tDocHeader {
	char sName[32];
	DWORD dwUnknown1;
	DWORD dwTime1;
	DWORD dwTime2;
	DWORD dwTime3;
	DWORD dwLastSync;
	DWORD ofsSort;
	DWORD ofsCatagories;
	DWORD dwCreator;
	DWORD dwType;
	DWORD dwUnknown2;
	DWORD dwUnknown3;
	WORD  wNumRecs;
};
*/
struct tDocRecord0 {
	WORD wVersion;	// 1=plain text, 2=compressed
	WORD wSpare;
	DWORD dwStoryLen;   // in chars, when decompressed
	WORD wNumRecs; 			// text records only; equals tDocHeader.wNumRecs-1
	WORD wRecSize;			// usually 0x1000
	DWORD dwSpare2;
};

struct PeanutHeader
{
    UInt16 Version;
    UInt8 Junk1[6];
    UInt16 Records;
    UInt8 Junk2[106];
};

////////////// utilities //////////////////////////////////////

inline WORD SwapWord(WORD r)
{
	return (r>>8) + (r<<8);
}

inline DWORD SwapLong(DWORD r)
{
	return  ((r>>24) & 0xFF) + (r<<24) + ((r>>8) & 0xFF00) + ((r<<8) & 0xFF0000);
}

class Aportis : public CExpander, Cpdb {
    bool peanutfile;
    void dePeanut(int&);
  DWORD dwLen;
  WORD nRecs2;
  DWORD dwTLen;
  WORD nRecs;
  WORD BlockSize;
  DWORD dwRecLen;
  int currentrec, currentpos;
  unsigned int cbptr;
  unsigned int outptr;
  unsigned char circbuf[2048];
  char bCompressed;
public:
  virtual void suspend()
      {
	  CExpander::suspend(fin);
      }
  virtual void unsuspend()
      {
	  CExpander::unsuspend(fin);
      }
  virtual void sizes(unsigned long& _file, unsigned long& _text)
    {
      _file = dwLen;
      _text = dwTLen;
    }
  virtual bool hasrandomaccess() { return true; }
  virtual ~Aportis() {}
  Aportis();
  virtual int OpenFile(const char *src);
  virtual int getch();
  virtual unsigned int locate();
  virtual void locate(unsigned int n);
  virtual CList<Bkmk>* getbkmklist();
  virtual MarkupType PreferredMarkup()
      {
	  return (peanutfile) ? cPML : cTEXT;
      }
private:
  bool refreshbuffer();
  unsigned int GetBS(unsigned int bn);
};

#endif
