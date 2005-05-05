#include "CRegExp.h"

//#include <stdio.h>
#include <stdlib.h>
//#include <string.h>


tchar CRegExpFilt::escapedchar(tchar c)
{
  switch (c)
    {
    case '\\':
      return '\\';
      break;
    case '"':
      return '\"';
      break;
    case 'a':
      return '\a';
      break;
    case 'b':
      return '\b';
      break;
    case 'f':
      return '\f';
      break;
    case 'n':
      return '\n';
      break;
    case 'r':
      return '\r';
      break;
    case 't':
      return '\t';
      break;
    case 'v':
      return '\v';
      break;
    default:
      return c;
      break;
    }
}

void CRegExpFilt::regchar(tchar c, bool insens)
{
  if (insens)
    {
      tchar t = upper(c);
      CV[t] = 0;
      t = lower(c);
      CV[t] = 0;
    }
  else
    {
      CV[c] = 0;
    }
}

void CRegExpFilt::prepreprocessing(const QString& pat, bool insens)
{
  for (unsigned int p = 0; p < pat.length(); p++)
    {
#ifdef _WINDOWS
      switch (pat.at(p).unicode())
#else
      switch (pat[p].unicode())
#endif
	  {
	case '{':
	  {
	    break;
	  }
	case '}':
	  {
	    break;
	  }
	case '^':
	  {
	    break;
	  }
	case '.' :
	  {
	    break;
	  }
	case '#':
	  {
	    p++;
#ifdef _WINDOWS
	    while ('0' <= pat.at(p).unicode() && pat.at(p).unicode() <= '9')
#else
		while ('0' <= pat[p].unicode() && pat[p].unicode() <= '9')
#endif
	      {
	      }
	    p--;
	    break;
	  }
	case '\\' :
	  {
#ifdef _WINDOWS
	    tchar c = escapedchar(pat.at(++p).unicode());
#else
	    tchar c = escapedchar(pat[++p].unicode());
#endif
		regchar(c, insens);
	    break;
	  }

	case '[' :
	  {
	    tchar clast;
	    bool invert = false;
	    tchar c;
#ifdef _WINDOWS
	    if (pat.at(p+1).unicode() == '^')
#else
	    if (pat[p+1].unicode() == '^')
#endif
		{
		p++;
		invert = true;
	      }
#ifdef _WINDOWS
	    while ((c = pat.at(++p).unicode()) != ']')
#else
	    while ((c = pat[++p].unicode()) != ']')
#endif
		{
		if (c == '\\')
		  {
#ifdef _WINDOWS
		    c = escapedchar(pat.at(++p).unicode());
#else
		    c = escapedchar(pat[++p].unicode());
#endif
			if (c == ']') break;
		  }
		if (c == '-')
		  {
#ifdef _WINDOWS
		    c = pat.at(++p).unicode();
#else
		    c = pat[++p].unicode();
#endif
			for (tchar j = clast; j <= c; j++)
		      {
			regchar(j, insens);
		      }
		  }
		else
		  {
		    regchar(c, insens);
		  }
		clast = c;
	      }
	    break;
	  }
	default :
	  {
#ifdef _WINDOWS
	    regchar(pat.at(p).unicode(), insens);
#else
	    regchar(pat[p].unicode(), insens);
#endif
		break;
	  }
	}
    }
  /*
  for (iter i = CV.begin(); i != CV.end(); ++i)
    {
      printf("Pre: [%u]\n", i.first());
    }
  */
  CV[0] = 0;
}

unsigned int CRegExpFilt::preprocessing(const QString& pat, bool insens)
{
  prepreprocessing(pat, insens);
  qDebug("PrePreProcessing done");
  unsigned int p, m;
  bool inkeep = false;
  keep = 0;
  replace = 0;
  for (unsigned int j = 0; j < WORD_SIZE; j++)
    {
      bit[j] = (1 << (WORD_SIZE -j -1));
      lfcnt[j] = 0;
    }

  for (p = 0, m = 0; p < pat.length(); p++)
    {
      qDebug("m is %u", m);
      if (inkeep) keep |= bit[m];
#ifdef _WINDOWS
      switch (pat.at(p).unicode())
#else
      switch (pat[p].unicode())
#endif
	  {
	case '{':
	  {
	    inkeep = true;
	    break;
	  }
	case '}':
	  {
	    keep ^= bit[m];
	    inkeep = false;
	    break;
	  }
	case '^':
	  {
	    replace |= bit[m];
	    lfcnt[m]++;
	    break;
	  }
	case '.' :
	  {
	    for (iter j = CV.begin(); j != CV.end(); ++j) CV[j.first()] |= bit[m];
	    m++;
	    break;
	  }
	case '#':
	  {
	    if (m > 0)
	      {
		p++;
		int count = 0;
#ifdef _WINDOWS
		while ('0' <= pat.at(p).unicode() && pat.at(p).unicode() <= '9')
#else
		while ('0' <= pat[p].unicode() && pat[p].unicode() <= '9')
#endif
		{
#ifdef _WINDOWS
		    count = 10*count + pat.at(p++).unicode() - '0';
#else
		    count = 10*count + pat[p++].unicode() - '0';
#endif
		  }
		p--;
		count = count-1;
		unsigned int mask = 0;
		for (unsigned int i = m; i < m+count; i++)
		  {
		    mask |= bit[i];
		  }
	       
		for (iter it = CV.begin(); it != CV.end(); ++it)
		  {
		    if (CV[it.first()] & bit[m-1])
		      {
			CV[it.first()] |= mask;
		      }
		  }
		if (keep & bit[m-1]) keep |= mask;
		m += count;
	      }
	    else
	      {
		p++;
	      }
	    break;
	  }
	case '\\' :
	  {
#ifdef _WINDOWS
	    tchar c = escapedchar(pat.at(++p).unicode());
#else
	    tchar c = escapedchar(pat[++p].unicode());
#endif
		if (insens)
	      {
		CV[upper(c)] |= bit[m];
		CV[lower(c)] |= bit[m];
	      }
	    else
	      {
		CV[c] |= bit[m];
	      }
	    m++;
	    break;
	  }

	case '[' :
	  {
	    tchar c, clast;
	    bool invert = false;
#ifdef _WINDOWS
	    if (pat.at(p+1).unicode() == '^')
#else
	    if (pat[p+1].unicode() == '^')
#endif
		{
		p++;
		invert = true;
	      }
#ifdef _WINDOWS
	    while ((c = pat.at(++p).unicode()) != ']')
#else
	    while ((c = pat[++p].unicode()) != ']')
#endif
		{
		if (c == '\\')
		  {
#ifdef _WINDOWS
		    c = escapedchar(pat.at(++p).unicode());
#else
		    c = escapedchar(pat[++p].unicode());
#endif
			if (c == ']') break;
		  }
		if (c == '-')
		  {
#ifdef _WINDOWS
		    c = pat.at(++p).unicode();
#else
		    c = pat[++p].unicode();
#endif
			for (tchar j = clast; j <= c; j++)
		      {
			if (insens)
			  {
			    iter it;
			    if ((it = CV.find(upper(j))) != CV.end())
			      CV[it] |= bit[m];
			    else
			      CV[0] |= bit[m];
			    if ((it = CV.find(lower(j))) != CV.end())
			      CV[it] |= bit[m];
			    else
			      CV[0] |= bit[m];
			  }
			else
			  {
			    iter it;
			    if ((it = CV.find(j)) != CV.end())
			      CV[it] |= bit[m];
			    else
			      {
				CV[0] |= bit[m];
			      }
			  }
		      }
		  }
		else
		  {
		    if (insens)
		      {
			iter it;
			if ((it = CV.find(upper(c))) != CV.end())
			  CV[it] |= bit[m];
			else
			  CV[0] |= bit[m];
			if ((it = CV.find(lower(c))) != CV.end())
			  CV[it] |= bit[m];
			else
			  CV[0] |= bit[m];
		      }
		    else
		      {
			iter it;
			if ((it = CV.find(c)) != CV.end())
			  CV[it] |= bit[m];
			else
			  CV[0] |= bit[m];
		      }
		  }
		clast = c;
	      }
	    if (invert)
	      {
		for (iter i = CV.begin(); i != CV.end(); ++i)
		  {
		    CV[i.first()] ^= bit[m];
		  }
	      }
	    m++;
	    break;
	  }
	default :
	  {
#ifdef _WINDOWS
	    tchar c = pat.at(p).unicode();
#else
	    tchar c = pat[p].unicode();
#endif
		if (insens)
	      {
		CV[upper(c)] |= bit[m];
		CV[lower(c)] |= bit[m];
	      }
	    else CV[c] |= bit[m];
	    m++;
	    break;
	  }
	}
    }
  qDebug("Returning:%u",m);
  return m;
}

bool CRegExpFilt::empty()
{
  return m_outQueue.empty();
}

tchar CRegExpFilt::pop()
{
  return m_outQueue.pop();
}

bool CRegExpFilt::addch(tchar ch)
{
  word[cur] = ch;
  cur = (cur+1)%patlength;
  if (len < patlength) len++;

  unsigned int cv = 0;
  iter it;
  if ((it = CV.find(ch)) == CV.end())
    {
      cv = CV[0];
    }
  else
    {
      cv = CV[it];
    }

  R = ((R >> 1) | bit_0) & cv; /* Exact matches */
  if (R & endpos)
    {
      for (unsigned int i = 0; i < patlength; i++)
	{
	  if (replace & bit[i])
	    {
	      for (unsigned int j = 0; j < lfcnt[i]; j++)
		{
		  m_outQueue.push(10);
		}
	    }
	  if (keep & bit[i])
	    {
	      m_outQueue.push(word[(cur+i)%patlength]);
	      //		putchar('*');
	      //		putchar(i + '0');
	    }
	  len = 0;
	}
      return true;
    }
  else
    {
      if (len == patlength)
	{
	  tchar ch = word[cur];
	  if (ch == 10) ch = ' ';
	  m_outQueue.push(ch);
	}
      return false;
    }
}

void CRegExpFilt::restart()
{
  R = 0;
  len = 0;
}

CRegExpFilt::CRegExpFilt(const QString& pat, bool insensflag) : CV(300)
{
  cur = 0;
  patlength = preprocessing(pat, insensflag);
  qDebug("Preprocesing done:%u", patlength);
  endpos = bit[patlength-1];
  bit_0 = bit[0];

  restart();

  qDebug("Pattern: %s:%u", (const char*)pat, patlength);

}


CRegExpFilt::~CRegExpFilt()
{
}

#ifdef NOWAYISTHISDEFINED
void reportmatch(tchar *line, /*tchar *text,*/ unsigned int mtype, unsigned int lino)
{
  /*
    tchar *text = line + strlen(line);

    tchar *ptr = line;
    if (mtype == 0)
    printf("Exact match at line number %u.\n", lino);
    else
    printf("%u error match at line number %u.\n", mtype, lino);
    while (ptr < text) putchar(*ptr++);
    printf("%c[4m^%c[24m%s\n", 27, 27, ptr);
  */
}


void usage(void)
{
  printf("Usage: CRegExpFilt [-i] pattern/a file\n");
}

int getline(tchar *s,int lim,FILE *f)
{
  int c, i;
    
  for (i = 0; i < lim-1 && (c = getc(f)) != EOF && c != '\n'; )
    {
      s[i++] = (tchar)c;
    }
  s[i] = '\0';
  return ((c == EOF && i == 0) ? -1 : i);
}

#define BUF_SIZE	256

int main(int argc, char **argv)
{
  unsigned int lino = 0;
  unsigned int blino = 0;
  bool insens = false;
  int len;
  tchar line[BUF_SIZE];
  FILE *inf;

  /* Error checking of cmd ln args! */
  if (argc < 3)
    {usage(); return 10; }
  /* Corresponds to requiring a minimum of 3 matches */
  for (len = 1; len < argc-2; len++)
    {
      if (argv[len][0] != '-')
	{usage(); return 10; }
      else switch (argv[len][1])
	{
	case 'i' :
	  {
	    insens = true;
	    break;
	  }
	default :
	  {usage(); return 10;}
	}
    }

  tchar* pattern = new tchar[strlen(argv[argc-2])+1];

  for (int i = 0; (pattern[i] = argv[argc-2][i]) != 0; i++);



  CRegExpFilt test(pattern, insens);

  delete [] pattern;

  inf = fopen(argv[argc-1], "r");
  if (!inf)
    {
      printf("file not found\n");
      return 10;
    }

  while ((len = getline(line, BUF_SIZE, inf)) >= 0)
    {
      lino++;
      bool ret = false;
   
      {
	tchar *textend = line+len;
	tchar *text = line;
	while (text < textend)
	  {
	    ret |= test.addch(*text++);
	  }
	ret |= test.addch('\n');
	while (!test.empty())
	  {
	    putchar(test.pop());
	  }
      }
      //    inswt = test.addch(line, len);
      if (ret) reportmatch(line, 0, lino);
    }
  fclose(inf);
  //	CloseSTDLIB();
  return 0;
}
#endif
