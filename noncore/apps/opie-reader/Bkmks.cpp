#include "name.h"
#include <qmessagebox.h>

#include "Bkmks.h"

#include "StyleConsts.h"
#include "Markups.h"
#include "my_list.h"
#include "version.h"

const unsigned long BkmkFile::magic = ((unsigned long)'q' << 24) | ((unsigned long)'t' << 16) | ((unsigned long)'r' << 8) | ((unsigned long)BKMKTYPE);

Bkmk::Bkmk(const unsigned char* _nm, unsigned short _nmlen, const unsigned char* _anno, unsigned short _annolen, unsigned int _p) : m_position(_p)
{
    init(_nm, _nmlen, _anno, _annolen, _p);
}

Bkmk::Bkmk(const tchar* _nm, const unsigned char* _anno, unsigned short annolen, unsigned int _p) : m_position(_p)
{
    init(_nm, sizeof(tchar)*(ustrlen(_nm)+1), _anno, annolen, _p);
}

Bkmk::Bkmk(const tchar* _nm, const tchar* _anno, unsigned int _p) : m_position(_p)
{

    if (_anno == NULL)
    {
	tchar t = 0;
	init(_nm, sizeof(tchar)*(ustrlen(_nm)+1), &t, sizeof(t), _p);
    }
    else
    {
	init(_nm, sizeof(tchar)*(ustrlen(_nm)+1), _anno, sizeof(tchar)*(ustrlen(_anno)+1), _p);
    }
}

void Bkmk::init(const void* _nm, unsigned short _nmlen, const void* _anno, unsigned short _annolen, unsigned int _p)
{
    m_namelen = _nmlen;
    if (m_namelen > 0)
    {
	m_name = new unsigned char[m_namelen];
	memcpy(m_name, _nm, m_namelen);
    }
    else
    {
	m_name = NULL;
    }

    m_annolen = _annolen;
    if (m_annolen > 0)
    {
	m_anno = new unsigned char[m_annolen];
	memcpy(m_anno, _anno, m_annolen);
    }
    else
    {
	m_anno = NULL;
    }
    m_position = _p;
}

Bkmk::~Bkmk()
{
    if (m_name != NULL) delete [] m_name;
    m_name = NULL;
    if (m_anno != NULL) delete [] m_anno;
    m_anno = NULL;
}
 
Bkmk& Bkmk::operator=(const Bkmk& rhs)
{
    if (m_name != NULL)
    {
	delete [] m_name;
	m_name = NULL;
    }
    if (m_anno != NULL)
    {
	delete [] m_anno;
	m_anno = NULL;
    }
    if (rhs.m_name != NULL)
    {
	m_namelen = rhs.m_namelen;
	m_name = new unsigned char[m_namelen];
	memcpy(m_name, rhs.m_name, m_namelen);
    }
    else
	m_name = NULL;
    if (rhs.m_anno != NULL)
    {
	m_annolen = rhs.m_annolen;
	m_anno = new unsigned char[m_annolen];
	memcpy(m_anno, rhs.m_anno, m_annolen);
    }
    else
	m_anno = NULL;
    m_position = rhs.m_position;
    return *this;
}

bool Bkmk::operator==(const Bkmk& rhs)
{
    return (m_position == rhs.m_position && (rhs.m_namelen == m_namelen) && memcmp(m_name,rhs.m_name,m_namelen) == 0);
}

void Bkmk::setAnno(unsigned char* t, unsigned short len)
{
    if (m_anno != NULL)
    {
	delete [] m_anno;
	m_anno = NULL;
    }
    if (t != NULL)
    {
	m_annolen = len;
	m_anno = new unsigned char[m_annolen];
	memcpy(m_anno, t, m_annolen);
    }
    else
    {
	m_annolen = sizeof(tchar);
	m_anno = new unsigned char[m_annolen];
	*((tchar*)m_anno) = 0;
    }
}

void Bkmk::setAnno(tchar* t)
{
    if (m_anno != NULL)
    {
	delete [] m_anno;
	m_anno = NULL;
    }
    if (t != NULL)
    {
	unsigned short len = ustrlen(t)+1;
	m_annolen = sizeof(tchar)*len;
	m_anno = new unsigned char[m_annolen];
	memcpy(m_anno, t, m_annolen);
    }
    else
    {
	m_annolen = sizeof(tchar);
	m_anno = new unsigned char[m_annolen];
	*((tchar*)m_anno) = 0;
    }
}

BkmkFile::BkmkFile(const char *fnm, bool w = false)
    :
    wt(w), isUpgraded(false)
{
    if (w)
    {
	f = fopen(fnm, "wb");
    }
    else
    {
	f = fopen(fnm, "rb");
    }
}

BkmkFile::~BkmkFile()
{
    if (f != NULL) fclose(f);
}

void BkmkFile::write(const Bkmk& b)
{
    if (f != NULL)
    {
	fwrite(&b.m_namelen, sizeof(b.m_namelen),1,f);
	fwrite(b.m_name,1,b.m_namelen,f);
	fwrite(&b.m_annolen, sizeof(b.m_annolen),1,f);
	fwrite(b.m_anno,1,b.m_annolen,f);
	fwrite(&b.m_position,sizeof(b.m_position),1,f);
    }
}

void BkmkFile::write(CList<Bkmk>& bl)
{
    if (f != NULL)
    {
	fwrite(&magic, sizeof(magic), 1, f);
	for (CList<Bkmk>::iterator i = bl.begin(); i != bl.end(); i++)
	{
	    write(*i);
	}
    }
}

CList<Bkmk>* BkmkFile::readall()
{
    CList<Bkmk>* bl = NULL;
    if (f != NULL)
    {
	unsigned long newmagic;
	fread(&newmagic, sizeof(newmagic), 1, f);
	if ((newmagic & 0xffffff00) != (magic & 0xffffff00))
	{
	    if (QMessageBox::warning(NULL, "Old bookmark file!", "Which version of " PROGNAME "\ndid you upgrade from?", "0_4*", "Any other version") == 0)
	    {
		fseek(f,0,SEEK_SET);
		bl = readall00(&read05);
	    }
	    else
	    {
		fseek(f,0,SEEK_SET);
		bl = readall00(&read03);
	    }
	    isUpgraded = true;
	}
	else
	{
	    switch(newmagic & 0xff)
	    {
		case 6:
		    isUpgraded = false;
		    bl = readall00(read06);
		    qDebug("Correct version!");
		    break;
		case 5:
		    isUpgraded = true;
		    bl = readall00(read05);
		    qDebug("Known version!");
		    break;
		default:
		    qDebug("Unknown version!");
		    isUpgraded = true;
		    bl = readall00(read05);
	    }
	}
    }
    return bl;
}

CList<Bkmk>* BkmkFile::readall00(Bkmk* (*readfn)(FILE*))
{
    CList<Bkmk>* bl = new CList<Bkmk>;
    while (1)
    {
	Bkmk* b = (*readfn)(f);
	if (b == NULL) break;
	bl->push_back(*b);
	delete b;
    }
    return bl;
}

Bkmk* BkmkFile::read03(FILE* f)
{
    Bkmk* b = NULL;
    if (f != NULL)
    {
	unsigned short ln;
	if (fread(&ln,sizeof(ln),1,f) == 1)
	{
	    tchar* name = new tchar[ln+1];
	    fread(name,sizeof(tchar),ln,f);
	    name[ln] = 0;

	    ln = 0;
	    tchar* anno = new tchar[ln+1];
	    anno[ln] = 0;

	    unsigned int pos;
	    fread(&pos,sizeof(pos),1,f);
	    b = new Bkmk(name,anno,pos);
	}
    }
    return b;
}

Bkmk* BkmkFile::read05(FILE* f)
{
    Bkmk* b = NULL;
    if (f != NULL)
    {
	unsigned short ln;
	if (fread(&ln,sizeof(ln),1,f) == 1)
	{
	    tchar* nm = new tchar[ln+1];
	    fread(nm,sizeof(tchar),ln,f);
	    nm[ln] = 0;
	    fread(&ln,sizeof(ln),1,f);
	    tchar* anno = new tchar[ln+1];
	    if (ln > 0) fread(anno,sizeof(tchar),ln,f);
	    anno[ln] = 0;
	    unsigned int pos;
	    fread(&pos,sizeof(pos),1,f);
	    b = new Bkmk(nm,anno,pos);
	}
    }
    return b;
}

Bkmk* BkmkFile::read06(FILE* f)
{
    Bkmk* b = NULL;
    if (f != NULL)
    {
	unsigned short ln;
	if (fread(&ln,sizeof(ln),1,f) == 1)
	{
	    b = new Bkmk;
	    b->m_namelen = ln;
	    b->m_name = new unsigned char[b->m_namelen];
	    fread(b->m_name,1,b->m_namelen,f);

	    fread(&(b->m_annolen),sizeof(b->m_annolen),1,f);
	    if (b->m_annolen > 0)
	    {
		b->m_anno = new unsigned char[b->m_annolen];
		fread(b->m_anno,1,b->m_annolen,f);
	    }
	    fread(&(b->m_position),sizeof(b->m_position),1,f);
	}
    }
    return b;
}
