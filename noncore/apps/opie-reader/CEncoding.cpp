#include <stdio.h>
#include "CEncoding.h"

void CUtf8::getch(tchar& ch, CStyle& sty)
{
    int iret;
    parent->getch(iret, sty);
    if (iret == EOF)
    {
	ch = UEOF;
	return;
    }
    tchar ret = iret;
    int count = 0;
    if (ret & (1 << 7))
    {
	unsigned char flags = ret << 1;
	while ((flags & (1 << 7)) != 0)
	{
	    ret <<= 6;
	    parent->getch(iret, sty);
	    ret += iret & 0x3f;
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
    ch = ret;
    return;
}

void CUcs16be::getch(tchar& ch, CStyle& sty)
{
    int iret;
    parent->getch(iret, sty);
    if (iret == EOF)
    {
	ch = UEOF;
	return;
    }
    tchar ret = iret;
    parent->getch(iret, sty);
    ch = (ret << 8) + iret;
}

void CUcs16le::getch(tchar& ch, CStyle& sty)
{
    int iret;
    parent->getch(iret, sty);
    if (iret == EOF)
    {
	ch = UEOF;
	return;
    }
    tchar ret = iret;
    parent->getch(iret, sty);
    ch = ret + (iret << 8);
}

void Ccp1252::getch(tchar& ch, CStyle& sty)
{
    int iret;
    parent->getch(iret, sty);
    ch = iret;
    switch (ch)
    {
	case EOF:
	    ch = UEOF;
	    break;
	case 0x80:
	    ch = 0x20ac;
	    break;
	case 0x82:
	    ch = 0x201a;
	    break;
	case 0x83:
	    ch = 0x0192;
	    break;
	case 0x84:
	    ch = 0x201e;
	    break;
	case 0x85:
	    ch = 0x2026;
	    break;
	case 0x86:
	    ch = 0x2020;
	    break;
	case 0x87:
	    ch = 0x2021;
	    break;
	case 0x88:
	    ch = 0x02c6;
	    break;
	case 0x89:
	    ch = 0x2030;
	    break;
	case 0x8a:
	    ch = 0x0160;
	    break;
	case 0x8b:
	    ch = 0x2039;
	    break;
	case 0x8c:
	    ch = 0x0152;
	    break;
	case 0x8e:
	    ch = 0x017d;
	    break;
	case 0x91:
	    ch = 0x2018;
	    break;
	case 0x92:
	    ch = 0x2019;
	    break;
	case 0x93:
	    ch = 0x201c;
	    break;
	case 0x94:
	    ch = 0x201d;
	    break;
	case 0x95:
	    ch = 0x2022;
	    break;
	case 0x96:
	    ch = 0x2013;
	    break;
	case 0x97:
	    ch = 0x2014;
	    break;
	case 0x98:
	    ch = 0x02dc;
	    break;
	case 0x99:
	    ch = 0x2122;
	    break;
	case 0x9a:
	    ch = 0x0161;
	    break;
	case 0x9b:
	    ch = 0x203a;
	    break;
	case 0x9c:
	    ch = 0x0153;
	    break;
	case 0x9e:
	    ch = 0x017e;
	    break;
	case 0x9f:
	    ch = 0x0178;
	    break;
	default:
	    break;
    }
}

void CPalm::getch(tchar& ch, CStyle& sty)
{
    Ccp1252::getch(ch, sty);
    switch (ch)
    {
	case 0x18:
	    ch = 0x2026;
	    break;
	case 0x19:
	    ch = 0x2007;
	    break;
	case 0x8d:
	    ch = 0x2662;
	    break;
	case 0x8e:
	    ch = 0x2663;
	    break;
	case 0x8f:
	    ch = 0x2661;
	    break;
	case 0x90:
	    ch = 0x2660;
	    break;
	default:
	    break;
    }
}

void CAscii::getch(tchar& ch, CStyle& sty)
{
    int iret;
    parent->getch(iret, sty);
    if (iret == EOF)
    {
	ch = UEOF;
    }
    else
    {
	ch = iret;
    }
}
