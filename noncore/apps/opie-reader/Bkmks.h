#ifndef __Bkmks_h
#define __Bkmks_h

#include "config.h"
#include "Filedata.h"
#include <stdio.h>

template<class T>
class CList;

class Bkmk
{
    friend class BkmkFile;
    unsigned char* m_name;
    unsigned short m_namelen;
    unsigned char* m_anno;
    unsigned short m_annolen;
    unsigned int m_position;
    void init(const void*, unsigned short, const void*, unsigned short, unsigned int);
 public:
    Bkmk() : m_name(NULL), m_namelen(0), m_anno(NULL), m_annolen(0), m_position(0) {};
    Bkmk(const unsigned char* _nm, unsigned short _nmlen, const unsigned char* _anno, unsigned short _annolen, unsigned int _p);
    Bkmk(const tchar* _nm, const unsigned char* _anno, unsigned short _annolen, unsigned int _p);
    Bkmk(const tchar* _nm, const tchar* _anno, unsigned int _p);
    Bkmk(const Bkmk& rhs) : m_name(NULL), m_anno(NULL)
	{
	    *this = rhs;
	}
    ~Bkmk();
    unsigned int value() const { return m_position; }
    void value(unsigned int _v) { m_position = _v; }
    tchar *name() const { return (tchar*)m_name; }
    tchar *anno() const { return (tchar*)m_anno; }
    bool operator<(const Bkmk& rhs) { return (m_position < rhs.m_position); }
    Bkmk& operator=(const Bkmk& rhs);
    bool operator==(const Bkmk& rhs);
    void setAnno(tchar* t);
    void setAnno(unsigned char* t, unsigned short len);
    unsigned char* filedata()
	{
	    CFiledata fd(anno());
	    return m_anno+fd.length();
	}
    unsigned short filedatalen()
	{
	    CFiledata fd(anno());
	    return m_annolen - fd.length();
	}
};

class BkmkFile
{
    FILE* f;
    bool wt;
    bool isUpgraded;
    static const unsigned long magic;
 private:
    static Bkmk* read06(FILE*);
    static Bkmk* read05(FILE*);
    static Bkmk* read03(FILE*);
    CList<Bkmk>* readall00(Bkmk*(*fn)(FILE*));
    void write(const Bkmk& b);
 public:
    bool upgraded() { return isUpgraded; }
    BkmkFile(const char *fnm, bool w = false);
    ~BkmkFile();
    void write(CList<Bkmk>& bl);
    CList<Bkmk>* readall();
};
#endif
