#include <qstring.h>

#ifdef _UNICODE
inline size_t ustrlen(const tchar* _p)
{
    if (_p == NULL) return 0;
    const tchar *p = _p;
    while (*p != 0)
    {
	p++;
/*
	if (p - _p == 20)
	{
	    printf("ustrlen::String too long:");
	    for (int i = 0; i < 20; i++) printf("%c",_p[i]);
	    printf("\n");
	}
*/
    }
    return p - _p;
}

inline int ustrcmp(const tchar* _p1, const tchar* _p2)
{
    if (_p1 == 0) return 1;
    if (_p2 == 0) return -1;
    const tchar* p1 = _p1, *p2 = _p2;
    while (*p1 != 0)
    {
/*
	if (p1 - _p1 == 20)
	{
	    printf("ustrcmp::String too long:");
	    for (int i = 0; i < 20; i++) printf("%c",_p1[i]);
	    printf("\n");
	}
*/
	if (*p1 < *p2) return -1;
	if (*p1 > *p2) return 1;
	if (*p2 == 0) return 1;
	p1++, p2++;
    }
    if (*p2 != 0) return -1;
    return 0;
}

inline QString toQString(tchar *_p)
{
    if (_p == NULL) return 0;
    int i = 0;
    tchar *p = _p;
    QString ret;
    while (*p != 0) ret[i++] = *(p++);
    return ret;
}

inline QString toQString(tchar *_p, unsigned int len)
{
    if (_p == NULL) return 0;
    unsigned int i = 0;
    tchar *p = _p;
    QString ret;
    while (*p != 0 && i < len) ret[i++] = *(p++);
    return ret;
}
#else

inline size_t ustrlen(const tchar* _p) { return strlen(_p); }
inline int ustrcmp(const tchar* _p1, const tchar* _p2) { return strcmp(_p1, _p2); }

#endif
