#include "CDrawBuffer.h"
#include "CFilter.h"

unsigned short striphtml::skip_ws()
{
    tchar ch;
    CStyle sty;
    do
    {
	parent->getch(ch, sty);
    }
    while (ch < 33);
    return ch;
}

unsigned short striphtml::skip_ws_end()
{
    tchar ch;
    CStyle sty;
    parent->getch(ch, sty);
    if (ch == ' ')
    {
	do
	{
	    parent->getch(ch, sty);
	}
	while (ch != '>');
    }
    return ch;
}

unsigned short striphtml::parse_m()
{
    tchar ch;
    CStyle sty;
    parent->getch(ch, sty);
    if (ch == 'm' || ch == 'M')
    {
	ch = skip_ws_end();
	if (ch == '>')
	{
	    return 0;
	}
    }
    return ch;
}

void striphtml::mygetch(tchar& ch, CStyle& sty)
{
    parent->getch(ch, sty);
    if (ch == 10) ch = ' ';
}

void striphtml::getch(tchar& ch, CStyle& sty)
{
    CStyle dummy;
    mygetch(ch, dummy);
    if (ch == 10) ch = ' ';
    while (ch == '<')
    {
	ch = skip_ws();

	switch (ch)
	{
	    case 'p':
	    case 'P':
		ch = skip_ws_end();
		if (ch == '>')
		{
		    ch = 10;
		    continue;
		}
		break;
	    case 'b':
	    case 'B':
		ch = skip_ws_end();
		if (ch == '>')
		{
		    currentstyle.setBold();
		    mygetch(ch, dummy);
		    continue;
		}
		else if (ch == 'r' || ch == 'R')
		{
		    ch = skip_ws_end();
		    if (ch == '>')
		    {
			ch = 10;
			continue;
		    }
		}
		break;
	    case 'i':
	    case 'I':
		ch = skip_ws_end();
		if (ch == '>')
		{
		    currentstyle.setItalic();
		    mygetch(ch, dummy);
		    continue;
		}
		break;
	    case 'e':
	    case 'E':
		if ((ch = parse_m()) == 0)
		{
		    currentstyle.setItalic();
		    mygetch(ch, dummy);
		    continue;
		}
		break;
	    case 'h':
	    case 'H':
		mygetch(ch, dummy);
		if ('0' < ch && ch <= '9')
		{
		    tchar hs = ch;
		    ch = skip_ws_end();
		    if (ch == '>')
		    {
			switch (hs)
			{
			    case '1':
//				currentstyle = ucBold | ucFontBase+2 | (ucAlignCentre << ucAlignShift);
				currentstyle.unset();
				currentstyle.setFontSize(2);
				currentstyle.setBold();
				currentstyle.setCentreJustify();
				break;
			    case '2':
//				currentstyle = ucBold | ucFontBase+1;
				currentstyle.unset();
				currentstyle.setFontSize(1);
				currentstyle.setBold();
				break;
			    default:
//				currentstyle = ucBold | ucFontBase;
				currentstyle.unset();
				currentstyle.setBold();
			}
			ch = 10;
//			mygetch(ch, dummy);
			continue;
		    }
		}
		break;
	    case '/':
		mygetch(ch, dummy);
		switch (ch)
		{
		    case 'b':
		    case 'B':
			ch = skip_ws_end();
			if (ch == '>')
			{
			    currentstyle.unsetBold();
			    mygetch(ch, dummy);
			    continue;
			}
			break;
		    case 'i':
		    case 'I':
			ch = skip_ws_end();
			if (ch == '>')
			{
			    currentstyle.unsetItalic();
			    mygetch(ch, dummy);
			    continue;
			}
			break;
		    case 'e':
		    case 'E':
			if ((ch = parse_m()) == 0)
			{
			    currentstyle.unsetItalic();
			    mygetch(ch, dummy);
			    continue;
			}
			break;
		    case 'h':
		    case 'H':
			mygetch(ch, dummy);
			if ('0' < ch && ch <= '9')
			{
			    ch = skip_ws_end();
			    if (ch == '>')
			    {
				currentstyle.unset();
				//mygetch(ch, dummy);
				ch = 10;
				continue;
			    }
			}
			break;
		    default:
			break;
		}
		break;
	    default:
		break;
	}
	while (ch != '>' && ch != UEOF)
	{
	    mygetch(ch, dummy);
	}
	mygetch(ch, dummy);
    }
    if (ch == '&')
    {
	mygetch(ch, dummy);
	if (ch == '#')
	{
	    int id = 0;
	    mygetch(ch, dummy);
	    while (ch != ';' && ch != UEOF)
	    {
		id = 10*id+ch-'0';
		mygetch(ch, dummy);
	    }
	    ch = id;
	}
    }
//    sty = (dummy == ucFontBase) ? currentstyle : dummy;
    sty = currentstyle;
    return;
}


void textfmt::mygetch(tchar& ch, CStyle& sty)
{
    if (uselast)
    {
	ch = lastchar;
	uselast = false;
    }
    else
    {
	parent->getch(ch, sty);
    }
}

void textfmt::getch(tchar& ch, CStyle& sty)
{
    mygetch(ch, sty);
    do
    {
	sty = currentstyle;
	switch (ch)
	{
	    case 10:
		currentstyle.unset();
		sty = currentstyle;
		break;
// Use this if you want to replace -- by em-dash
	    case '-':
//		parent->getch(ch, sty);
		mygetch(ch, sty);
		if (ch == '-')
		{
		    ch = 0x2014;
		}
		else
		{
		    lastchar = ch;
		    uselast = true;
		    ch = '-';
		}
		break;
	    case '*':
		if (currentstyle.isBold())
		{
// Already bold - time to turn it off?
// The next two lines ensure that * follows a character but it works better without
//		    QChar c(lastchar);
//		    if ((lastchar != '*') && (c.isPunct() || c.isLetterOrNumber()))
		    if (lastchar != '*')
		    {
			currentstyle.unsetBold();
			CStyle dummy;
//			parent->getch(ch, dummy);
			mygetch(ch, dummy);
		    }
		}
		else
		{
// not bold - time to turn it on?
		    CStyle dummy;
//		    parent->getch(ch, dummy);
		    mygetch(ch, dummy);
		    QChar c(ch);
		    if ((ch != '*') && (c.isPunct() || c.isLetterOrNumber()))
		    {
			currentstyle.setBold();
		    }
		    else
		    {
			lastchar = ch;
			uselast = true;
			ch = '*';
		    }
		    
		}
		break;
	    case '_':
		if (currentstyle.isItalic())
		{
// Already bold - time to turn it off?
// The next two lines ensure that * follows a character but it works better without
//		    QChar c(lastchar);
//		    if ((lastchar != '_') && (c.isPunct() || c.isLetterOrNumber()))
		    if (lastchar != '_')
		    {
			currentstyle.unsetItalic();
			CStyle dummy;
//			parent->getch(ch, dummy);
			mygetch(ch, dummy);
		    }
		}
		else
		{
// not bold - time to turn it on?
		    CStyle dummy;
//		    parent->getch(ch, dummy);
		    mygetch(ch, dummy);
		    QChar c(ch);
		    if ((ch != '_') && (c.isPunct() || c.isLetterOrNumber()))
		    {
			currentstyle.setItalic();
		    }
		    else
		    {
			lastchar = ch;
			uselast = true;
			ch = '_';
		    }
		    
		}
		break;
	}
    }
    while (sty != currentstyle);
    if (!uselast) lastchar = ch;
    return;
}

void remap::getch(tchar& ch, CStyle& sty)
{
    if (q[offset] != 0)
    {
	q[offset++];
	sty = currentstyle;
	return;
    }
    parent->getch(ch, sty);
    switch (ch)
    {
	case 0x201a:
	    ch = '\'';
	    break;
	case 0x0192:
	    ch = 'f';
	    break;
	case 0x201e:
	    ch = '"';
	    break;
	case 0x2026:
	    offset = 0;
	    q[0] = '.';
	    q[1] = '.';
	    q[2] = 0;
	    ch = '.'; // should be ...
	    break;
	case 0x0160:
	    ch = 'S';
	    break;
	case 0x2039:
	    ch = '<';
	    break;
	case 0x0152:		
	    offset = 0;
	    q[0] = 'E';
	    q[1] = 0;
	    ch = 'O';
	    break;
	case 0x017d:
	    ch = 'Z';
	    break;
	case 0x2018:
	    ch = '\'';
	    break;
	case 0x2019:
	    ch = '\'';
	    break;
	case 0x201c:
	    ch = '"';
	    break;
	case 0x201d:
	    ch = '"';
	    break;
	case 0x2022:
	    ch = '>';
	    break;
	case 0x2013:
	    ch = '-';
	    break;
	case 0x2014:
	    offset = 0;
	    q[0] = '-';
	    q[1] = 0;
	    ch = '-'; // should be --
	    break;
	case 0x02dc:
	    ch = '~';
	    break;
	case 0x0161:
	    ch = 's';
	    break;
	case 0x203a:
	    ch = '>';
	    break;
	case 0x0153:
	    offset = 0;
	    q[0] = 'e';
	    q[1] = 0;
	    ch = 'o';// should be oe
	    break;
	case 0x017e:
	    ch = 'z';
	    break;
	case 0x0178:
	    ch = 'Y';
	    break;
    }
    currentstyle = sty;
}

void PeanutFormatter::getch(tchar& ch, CStyle& sty)
{
    CStyle dummy;
    currentstyle.setColour(0,0,0);
    parent->getch(ch, dummy);
    while (ch == '\\')
    {
	parent->getch(ch, dummy);
	if (ch == '\\') break;
	switch(ch)
	{
	    case 'a':
	    {
		int code = 0;
		for (int i = 0; i < 3; i++)
		{
		    parent->getch(ch, dummy);
		    code = 10*code + ch - '0';
		}
		ch = code;
	    }
	    break;
	    case 'v':
	    {
		while (1)
		{
		    parent->getch(ch, dummy);
		    if (ch == '\\')
		    {
			parent->getch(ch, dummy);
			if (ch == 'v')
			{
			    parent->getch(ch, dummy);
			    break;
			}
		    }
		}
	    }
	    break;
	    case 's':
	    case 'n':
		currentstyle.setFontSize(0);
		parent->getch(ch,dummy);
		break;
	    case 'p':
		currentstyle.unset();
//		parent->getch(ch,dummy);
		ch = 10;
		break;
	    case 'l':
		if (currentstyle.getFontSize() == 1)
		{
		    currentstyle.setFontSize(0);
		}
		else
		{
		    currentstyle.setFontSize(1);
		}
		parent->getch(ch, dummy);
		break;
	    case 'x':
		if (currentstyle.getFontSize() == 0)
		{
//		    currentstyle.unset();
//		    currentstyle.setBold();
		    currentstyle.setFontSize(1);
		}
		else
		{
		    currentstyle.unset();
		}
//		parent->getch(ch, dummy);
		ch = 10;
		break;
	    case 'i':
		if (currentstyle.isItalic())
		{
		    currentstyle.unsetItalic();
		}
		else
		{
		    currentstyle.setItalic();
		}
		parent->getch(ch, dummy);
		break;
	    case 'b':
	    case 'B':
		if (currentstyle.isBold())
		{
		    currentstyle.unsetBold();
		}
		else
		{
		    currentstyle.setBold();
		}
		parent->getch(ch, dummy);
		break;
	    case 'c':
		if (currentstyle.getJustify() == m_AlignCentre)
		{
		    currentstyle.setLeftJustify();
		}
		else
		{
		    currentstyle.setCentreJustify();
		}
		parent->getch(ch, dummy);
		break;
	    case 'r':
		if (currentstyle.getJustify() == m_AlignRight)
		{
		    currentstyle.setLeftJustify();
		}
		else
		{
		    currentstyle.setRightJustify();
		}
		parent->getch(ch, dummy);
		break;
	    default:
		currentstyle.setColour(255,0,0);
	}
    }
    sty = currentstyle;
}

void OnePara::getch(tchar& ch, CStyle& sty)
{
    parent->getch(ch, sty);
    if (m_lastchar == 10)
    {
	while (ch == 10) parent->getch(ch, sty);
    }
    m_lastchar = ch;
}

#ifdef REPALM
void repalm::getch(tchar& ch, CStyle& sty)
{
    parent->getch(ch, sty);
    switch (ch)
    {
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
/*
	case 0x8e:
	    ch = 0x017d;
	    break;
*/
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
#endif
