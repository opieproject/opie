#include <qmessagebox.h>

#include "Bkmks.h"

#include "StyleConsts.h"
#include "Markups.h"
#include "my_list.h"
#include "version.h"

const unsigned long BkmkFile::magic = ((unsigned long)'q' << 24) | ((unsigned long)'t' << 16) | ((unsigned long)'r' << 8) | ((unsigned long)BKMKTYPE);


Bkmk::Bkmk(const tchar* _nm, const tchar* _anno, unsigned int _p) : m_position(_p)
{
    int len = ustrlen(_nm)+1;
    m_name = new tchar[len];
    for (int i = 0; i < len; i++) m_name[i] = _nm[i];

    if (_anno == NULL)
    {
	m_anno = new tchar[1];
	m_anno[0] = 0;
    }
    else
    {
	len = ustrlen(_anno)+1;
	m_anno = new tchar[len];
	for (int i = 0; i < len; i++) m_anno[i] = _anno[i];
    }
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
	int len = ustrlen(rhs.m_name)+1;
	m_name = new tchar[len];
	for (int i = 0; i < len; i++) m_name[i] = rhs.m_name[i];
    }
    else
	m_name = NULL;
    if (rhs.m_anno != NULL)
    {
	int len = ustrlen(rhs.m_anno)+1;
	m_anno = new tchar[len];
	for (int i = 0; i < len; i++) m_anno[i] = rhs.m_anno[i];
    }
    else
	m_anno = NULL;
    m_position = rhs.m_position;
    return *this;
}

bool Bkmk::operator==(const Bkmk& rhs)
{
    return (m_position == rhs.m_position && ustrcmp(m_name,rhs.m_name) == 0);
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
	int len = ustrlen(t)+1;
	m_anno = new tchar[len];
	for (int i = 0; i < len; i++) m_anno[i] = t[i];
    }
    else
	m_anno = NULL;
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

void BkmkFile::write(tchar* nm, tchar* an, const unsigned int& pos)
{
    if (f != NULL)
    {
	unsigned short ln = ustrlen(nm);
	fwrite(&ln,sizeof(ln),1,f);
	fwrite(nm,sizeof(tchar),ln,f);
	ln = ustrlen(an);
	fwrite(&ln,sizeof(ln),1,f);
	if (ln > 0) fwrite(an,sizeof(tchar),ln,f);
	fwrite(&pos,sizeof(pos),1,f);
    }
}

void BkmkFile::write(const Bkmk& b) { write(b.name(), b.anno(), b.value()); }

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

Bkmk* BkmkFile::read()
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

	    fread(&ln,sizeof(ln),1,f);
	    b->m_anno = new tchar[ln+1];
	    if (ln > 0) fread(b->m_anno,sizeof(tchar),ln,f);
	    b->m_anno[ln] = 0;
	    fread(&b->m_position,sizeof(b->m_position),1,f);
	}
    }
    return b;
}

CList<Bkmk>* BkmkFile::readall()
{
    CList<Bkmk>* bl = NULL;
    if (f != NULL)
    {
	unsigned long newmagic;
	fread(&newmagic, sizeof(newmagic), 1, f);
	if (newmagic != magic)
	{
	    if (QMessageBox::warning(NULL, "Old bookmark file!", "Which version of QTReader\ndid you upgrade from?", "0_4*", "Any other version") == 0)
	    {
		fseek(f,0,SEEK_SET);
		bl = readall04();
	    }
	    else
	    {
		fseek(f,0,SEEK_SET);
		bl = readall03();
	    }
	    isUpgraded = true;
	}
	else
	{
	    bl = readall04();
	}
    }
    return bl;
}

CList<Bkmk>* BkmkFile::readall04()
{
    CList<Bkmk>* bl = new CList<Bkmk>;
    while (1)
    {
	Bkmk* b = read();
	if (b == NULL) break;
	bl->push_back(*b);
	delete b;
    }
    return bl;
}

CList<Bkmk>* BkmkFile::readall03()
{
    CList<Bkmk>* bl = new CList<Bkmk>;
    while (1)
    {
	Bkmk* b = read03();
	if (b == NULL) break;
	bl->push_back(*b);
	delete b;
    }
    return bl;
}

Bkmk* BkmkFile::read03()
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

	    ln = 0;
	    b->m_anno = new tchar[ln+1];
	    b->m_anno[ln] = 0;

	    fread(&b->m_position,sizeof(b->m_position),1,f);
	}
    }
    return b;
}
