#include "config.h"
#include "hash.h"
#include "Queue.h"
#include <qstring.h>

typedef hashtable<tchar,unsigned int>::iterator iter;

#define WORD_SIZE	32

class CRegExpFilt
{
  CQueue<tchar> m_outQueue;
  hashtable<tchar,unsigned int> CV;
  unsigned int bit[WORD_SIZE];
  unsigned int R;
  unsigned int bit_0, endpos;
  unsigned int patlength;
  unsigned int keep, len;
  unsigned int replace, cur;
  void regchar(tchar, bool);
  void prepreprocessing(const QString& pat, bool insens);
  unsigned int preprocessing(const QString& pat, bool insens);
  tchar word[WORD_SIZE];
  unsigned int lfcnt[WORD_SIZE];
  tchar escapedchar(tchar c);
  int islower(tchar c)
    {
      return (('a' <= c) && (c <= 'z'));
    }
  tchar upper(tchar c)
    {
      return (tchar)(islower(c) ? (c - 'a' + 'A') : c);
    }
  int isupper(tchar c)
    {
      return (('A' <= c) && (c <= 'Z'));
    }
  tchar lower(tchar c)
    {
      return (tchar)(isupper(c) ? (c + 'a' - 'A') : c);
    }
 public:
  CRegExpFilt(const QString& pat, bool insens);
  ~CRegExpFilt();
  bool addch(tchar);
  void restart();
  unsigned int matchlength()
    { return patlength; }
  bool empty();
  tchar pop();
};
