#ifndef __CHM_h
#define __CHM_h
#include <stdio.h>
#include <sys/stat.h>
#include "useqpe.h"
#include "CExpander.h"
#include "chm_lib.h"
#include "my_list.h"
#include "CExpander.h"

struct Ref {
    QString RefName;
    QString RefLoc;
    int beginpos;
    int endpos;
};

struct Pic {
    QString RefName;
    QString RefLoc;
    int beginpos;
    int endpos;
};

class CHM : public CExpander
{
  void start2endSection();
  struct chmUnitInfo m_ui;
  struct chmFile *chmFile;
  chmUnitInfo chmui;
  //alloc a meg to buffer
  QString chmHomeBuffer;
  QString chmBuffer;
  QString chmPath;
  QString chmHHCPath;
  unsigned int bufpos;
  unsigned long fsize;
  unsigned long m_homestart, m_homeend;
public:
    void suspend();
    void unsuspend();
    bool hasnavigation() { return true; }

    CHM();
    virtual ~CHM();
    int OpenFile(const char *src);
    int getch();
    unsigned int locate();
    void locate(unsigned int n);
    bool hasrandomaccess() { return true; }
    void sizes(unsigned long& _file, unsigned long& _text);
    void addContent(QString content);
    void getch(tchar& ch, CStyle& sty);
    void setPath(QString PathName) {chmPath = PathName;};
    void setHomePath(QString PathName) {chmHHCPath = PathName;};

    MarkupType PreferredMarkup() {
        return cCHM;
    }
    bool getFile(const QString& href);
    QImage *getPicture(const QString& href);
    QString about() { return QString("CHM codec (c) Bob Griffin\nchm_lib (c) Jed Wing\nLZX code (c) Stuart Cale"); }
private :
    bool FillBuffer();
    bool FillContent();
    void FillHomeContent();
};
#endif
