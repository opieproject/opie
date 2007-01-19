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
    unsigned int m_position2;
    unsigned char m_red,m_green,m_blue, m_level;
    void init(const void*, unsigned short, const void*, unsigned short, unsigned int);
 public:
    Bkmk() : m_name(0), m_namelen(0), m_anno(0), m_annolen(0), m_position(0) {};
    Bkmk(const unsigned char* _nm, unsigned short _nmlen, const unsigned char* _anno, unsigned short _annolen, unsigned int _p);
    Bkmk(const tchar* _nm, const unsigned char* _anno, unsigned short _annolen, unsigned int _p);
    Bkmk(const tchar* _nm, const tchar* _anno, unsigned int _p);
    Bkmk(const tchar* _nm, const tchar* _anno, unsigned int _p, unsigned int _p2);
    Bkmk(const Bkmk& rhs);
    ~Bkmk();
    unsigned int value() const { return m_position; }
    void value(unsigned int _v) { m_position = _v; }
    unsigned int value2() const { return m_position2; }
    void value2(unsigned int _v) { m_position2 = _v; }
    unsigned char red() { return m_red; }
    unsigned char green() { return m_green; }
    unsigned char blue() { return m_blue; }
    void red(unsigned char _v) { m_red = _v; }
    void green(unsigned char _v) { m_green = _v; }
    void blue(unsigned char _v) { m_blue = _v; }
    unsigned char level() { return m_level; }
    void level(unsigned char _v) { m_level = _v; }
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
    bool isUpgraded, m_extras;
    static const unsigned long magic;
 private:
    static Bkmk* read07(BkmkFile*, FILE*);
    static Bkmk* read06(BkmkFile*, FILE*);
    static Bkmk* read05(BkmkFile*, FILE*);
    static Bkmk* read03(BkmkFile*, FILE*);
    CList<Bkmk>* readall00(Bkmk*(*fn)(BkmkFile*, FILE*));
    void write(const Bkmk& b);
 public:
    bool upgraded() { return isUpgraded; }
    BkmkFile(const char *fnm, bool w, bool _x);
    ~BkmkFile();
    void write(CList<Bkmk>& bl);
    CList<Bkmk>* readall();
};
#endif
