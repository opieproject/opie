#ifndef __STATEDATA_H
#define __STATEDATA_H

struct statedata
{
    bool bstripcr/*:1*/;
    bool btextfmt/*:1*/;
    bool bautofmt/*:1*/;
    bool bstriphtml/*:1*/;
    bool bpeanut/*:1*/;
    bool bdehyphen/*:1*/;
    bool bonespace/*:1*/;
    bool bunindent/*:1*/;
    bool brepara/*:1*/;
    bool bdblspce/*:1*/;
    bool m_bpagemode/*:1*/;
    bool m_navkeys/*:1*/;
    bool m_bMonoSpaced/*:1*/;
    bool bremap/*:1*/;
    bool bmakebold/*:1*/;
    bool Continuous/*:1*/;
#ifdef REPALM
    bool brepalm/*:1*/;
#endif
    int bindenter;
    int m_textsize;
    int m_encd;
    int m_charpc;
    char m_fontname[1];
};

#endif
