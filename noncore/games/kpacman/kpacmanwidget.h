#ifndef KPACMANWIDGET_H
#define KPACMANWIDGET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#endif

#include <qwidget.h>
#include <qregexp.h>

#include "score.h"
#include "referee.h"
#include "status.h"
#include "painter.h"

class KpacmanWidget : public QWidget
{
    Q_OBJECT
public:
    KpacmanWidget ( QWidget *parent = 0, const char *name = 0);
    virtual ~KpacmanWidget();

    void setScheme(int scheme=-1, int mode=-1);
    Score *score;
    Referee *referee;
    Status *status;

protected:
    void confScheme();
    void confMisc(bool defGroup=TRUE);
    void resizeEvent( QResizeEvent * );

private:
    Bitfont *bitfont;
    uchar bitfontFirstChar;
    uchar bitfontLastChar;

    QString fontName;

    int scheme;
    int mode;
};

#endif // KPACMANWIDGET_H
