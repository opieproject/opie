#include <stdio.h>
#include "CEncoding.h"

tchar CUtf8::getch()
{
    int iret = parent->getch();
    if (iret == EOF) return UEOF;
    tchar ret = iret;
    int count = 0;
    if (ret & (1 << 7))
    {
	unsigned char flags = ret << 1;
	while ((flags & (1 << 7)) != 0)
	{
	    ret <<= 6;
	    ret += parent->getch() & 0x3f;
	    flags <<= 1;
	    count++;
	}
	switch (count)
	{
	    case 0:
		break;
	    case 1:
		ret &= 0x07ff;
		break;
	    case 2:
		break;
	    case 3:
	    case 4:
	    case 5:
	    default:
		printf("Only 16bit unicode supported...");
	}
    }
    return ret;
}


tchar CUcs16be::getch()
{
    int iret = parent->getch();
    if (iret == EOF) return UEOF;
    tchar ret = iret;
    return (ret << 8) + parent->getch();
}

tchar CUcs16le::getch()
{
    int iret = parent->getch();
    if (iret == EOF) return UEOF;
    tchar ret = iret;
    return ret + (parent->getch() << 8);
}

tchar Ccp1252::getch()
{
    int iret = parent->getch();
    switch (iret)
    {
	case EOF:
	    return UEOF;
	case 0x80:
	    return 0x20ac;
	case 0x82:
	    return 0x201a;
	case 0x83:
	    return 0x0192;
	case 0x84:
	    return 0x201e;
	case 0x85:
	    return 0x2026;
	case 0x86:
	    return 0x2020;
	case 0x87:
	    return 0x2021;
	case 0x88:
	    return 0x02c6;
	case 0x89:
	    return 0x2030;
	case 0x8a:
	    return 0x0160;
	case 0x8b:
	    return 0x2039;
	case 0x8c:
	    return 0x0152;
	case 0x8e:
	    return 0x017d;
	case 0x91:
	    return 0x2018;
	case 0x92:
	    return 0x2019;
	case 0x93:
	    return 0x201c;
	case 0x94:
	    return 0x201d;
	case 0x95:
	    return 0x2022;
	case 0x96:
	    return 0x2013;
	case 0x97:
	    return 0x2014;
	case 0x98:
	    return 0x02dc;
	case 0x99:
	    return 0x2122;
	case 0x9a:
	    return 0x0161;
	case 0x9b:
	    return 0x203a;
	case 0x9c:
	    return 0x0153;
	case 0x9e:
	    return 0x017e;
	case 0x9f:
	    return 0x0178;
	default:
	    return iret;
    }
}

tchar CPalm::getch()
{
    tchar iret = Ccp1252::getch();
    switch (iret)
    {
	case 0x18:
	    return 0x2026;
	case 0x19:
	    return 0x2007;
	case 0x8d:
	    return 0x2662;
	case 0x8e:
	    return 0x2663;
	case 0x8f:
	    return 0x2661;
	case 0x90:
	    return 0x2660;
	default:
	    return iret;
    }
}

tchar CAscii::getch()
{
    int iret = parent->getch();
    if (iret == EOF) return UEOF;
    return iret;
}

