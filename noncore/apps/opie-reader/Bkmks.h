#ifndef __Bkmks_h
#define __Bkmks_h

#include "config.h"
#include <stdio.h>

template<class T>
class CList;

class Bkmk
{
    friend class BkmkFile;
    tchar* m_name;
    tchar* m_anno;
    unsigned int m_position;
 public:
    Bkmk() : m_name(NULL), m_anno(NULL), m_position(0) {};
    Bkmk(const tchar* _nm, const tchar* _anno, unsigned int _p);
    Bkmk(const Bkmk& rhs) : m_name(NULL), m_anno(NULL)
	{
	    *this = rhs;
	}
    ~Bkmk();
    unsigned int value() const { return m_position; }
    tchar *name() const { return m_name; }
    tchar *anno() const { return m_anno; }
    bool operator<(const Bkmk& rhs) { return (m_position < rhs.m_position); }
    Bkmk& operator=(const Bkmk& rhs);
    bool operator==(const Bkmk& rhs);
    void setAnno(tchar* t);
};

class BkmkFile
{
    FILE* f;
    bool wt;
    bool isUpgraded;
    static const unsigned long magic;
 private:
    Bkmk* read();
    Bkmk* read03();
    CList<Bkmk>* readall03();
    CList<Bkmk>* readall04();
    void write(tchar* nm, tchar* an, const unsigned int& pos);
    void write(const Bkmk& b);
 public:
    bool upgraded() { return isUpgraded; }
    BkmkFile(const char *fnm, bool w = false);
    ~BkmkFile();
    void write(CList<Bkmk>& bl);
    CList<Bkmk>* readall();
};
#endif
