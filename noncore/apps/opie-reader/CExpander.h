#ifndef __CExpander_h
#define __CExpander_h

#include "my_list.h"
#include "config.h"

class Bkmk
{
	friend class BkmkFile;
  tchar* m_name;
  unsigned int m_position;
 public:
  Bkmk() : m_name(NULL), m_position(0) {};
  Bkmk(const tchar* _nm, unsigned int _p) : m_position(_p)
    {
      int len = ustrlen(_nm)+1;
      m_name = new tchar[len];
      for (int i = 0; i < len; i++) m_name[i] = _nm[i];
    }
  Bkmk(const Bkmk& rhs) : m_name(NULL)
    {
      *this = rhs;
    }
  ~Bkmk() { if (m_name != NULL) delete [] m_name; }
  unsigned int value() const { return m_position; }
  tchar *name() const { return m_name; }
  bool operator<(const Bkmk& rhs) { return (m_position < rhs.m_position); }
  Bkmk& operator=(const Bkmk& rhs)
  {
    if (m_name != NULL) delete [] m_name;
    if (rhs.m_name != NULL)
      {
	int len = ustrlen(rhs.m_name)+1;
	m_name = new tchar[len];
	for (int i = 0; i < len; i++) m_name[i] = rhs.m_name[i];
      }
    else
      m_name = NULL;
    m_position = rhs.m_position;
    return *this;
  }
  bool operator==(const Bkmk& rhs)
  {
    return (m_position == rhs.m_position && ustrcmp(m_name,rhs.m_name) == 0);
  }
};

class BkmkFile
{
	FILE* f;
	bool wt;
public:
	BkmkFile(const char *fnm, bool w = false)
		:
			wt(w)
	{
		if (w)
			f = fopen(fnm, "wb");
		else
			f = fopen(fnm, "rb");
	}
	~BkmkFile()
	{
		if (f != NULL) fclose(f);
	}
	void write(tchar* nm, const unsigned int& pos)
	{
		if (f != NULL)
		{
			unsigned short ln = ustrlen(nm);
			fwrite(&ln,sizeof(ln),1,f);
			fwrite(nm,sizeof(tchar),ln,f);
			fwrite(&pos,sizeof(pos),1,f);
		}
	}
	void write(const Bkmk& b) { write(b.name(), b.value()); }
	void write(CList<Bkmk>& bl)
	{
		if (f != NULL)
		{
			for (CList<Bkmk>::iterator i = bl.begin(); i != bl.end(); i++)
			{
				write(*i);
			}
		}
	}
	Bkmk* read()
	{
		Bkmk* b = NULL;
		if (f != NULL)
		{
			unsigned short ln;
			if (fread(&ln,sizeof(ln),1,f) == 1)
			{
				b = new Bkmk;
				b->m_name = new tchar[ln+1];
				fread(b->m_name,sizeof(tchar),ln,f);
				b->m_name[ln] = 0;
				fread(&b->m_position,sizeof(b->m_position),1,f);
			}
		}
		return b;
	}
	CList<Bkmk>* readall()
	{
		CList<Bkmk>* bl = NULL;
		if (f != NULL)
		{
			bl = new CList<Bkmk>;
			while (1)
			{
				Bkmk* b = read();
				if (b == NULL) break;
				bl->push_back(*b);
				delete b;
			}
		}
		return bl;
	}
};

class CCharacterSource
{
 public:
#ifdef _UNICODE
    virtual tchar getch() = 0;
#else
    virtual int getch() = 0;
#endif
};

class CExpander
{
 public:
    CExpander() {};
    virtual ~CExpander() {};
    virtual int openfile(const char *src) = 0;
    virtual unsigned int locate() = 0;
    virtual void locate(unsigned int n) = 0;
    virtual bool hasrandomaccess() = 0;
    virtual void sizes(unsigned long& file, unsigned long& text) = 0;
    virtual CList<Bkmk>* getbkmklist() { return NULL; }
    virtual int getch() = 0;
};
#endif
