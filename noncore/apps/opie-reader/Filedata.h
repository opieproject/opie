#ifndef __FILEDATA_H
#define __FILEDATA_H

#include <time.h>

class CFiledata
{
    unsigned char* data;
    bool m_own;
 public:
    CFiledata(tchar* d)
	{
	    data = (unsigned char*)d;
	    m_own = false;
	}
    CFiledata(time_t dt, tchar* nm)
	{
	    int nlen = ustrlen(nm)+1;
	    data = new unsigned char[sizeof(time_t)+sizeof(tchar)*nlen];
	    *((time_t *)data) = dt;
	    memcpy(data+sizeof(time_t), nm, sizeof(tchar)*nlen);
	    m_own = true;
	}
    ~CFiledata()
	{
	    if (m_own && data != NULL)
		{
		    delete [] data;
		qDebug("~Filedata: deleting");
		}
	    else
	    {
		qDebug("~Filedata: not deleting");
	    }
	}
    tchar* name() const { return (tchar*)(data+sizeof(time_t)); }
    time_t date() { return *((time_t *)data); }
    void setdate(time_t _t) { *((time_t *)data) = _t; }
    unsigned char* content() { return data; }
    size_t length() const { return sizeof(time_t)+sizeof(tchar)*(ustrlen(name())+1); }
    bool operator==(const CFiledata& rhs)
	{
	    return ((length() == rhs.length()) && (memcmp(data, rhs.data, length()) == 0));
	}
    bool samename(const CFiledata& rhs)
	{
	    return (ustrcmp((tchar *)(data+sizeof(time_t)),(tchar *)(rhs.data+sizeof(time_t))) == 0);
	}
};

#endif
