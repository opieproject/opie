#ifndef KPACMAN_H
#define KPACMAN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "portable.h"

#include <qmainwindow.h>
class QMenuBar;
#include <qregexp.h>

#include "kpacmanwidget.h"

#include <qpopmenu.h>

#include <qlist.h>
#include <qfileinf.h>
#include <qlayout.h>

#include "referee.h"
#include "status.h"
#include "score.h"
#include "keys.h"

class Kpacman : public KTMainWindow
{
    Q_OBJECT
public:
    Kpacman(QWidget *parent = 0, const char *name = 0);
    virtual ~Kpacman();

public slots:
    void forcedHallOfFame(bool);

private slots:
    void newKpacman();
    void pauseKpacman();
    void toggleHallOfFame();
    void toggleNew();
    void togglePaused();
    void quitKpacman();

    void schemeChecked(int);
    void toggleFocusOutPause();
    void toggleFocusInContinue();
    void toggleHideMouseCursor();
    void confKeys();

protected:

private:
    KpacmanWidget *view;

    void menu();

    int lookupSchemes();

	QGridLayout *m_layout;
	QWidget *m_view;
    KMenuBar *_menuBar;
    QPopupMenu *gamePopup;
    QPopupMenu *optionsPopup;
    QPopupMenu *modesPopup;
    QList<QPopupMenu> *schemesPopup;

    int newID;
    int pauseID;
    int hofID;
    QArray<int> modeID;
    QArray<int> schemeID;
    QArray<int> schemeMode;
    int focusOutPauseID;
    int focusInContinueID;
    int hideMouseCursorID;

    bool focusOutPause;
    bool focusInContinue;
    bool hideMouseCursor;
};

#endif // KPACMAN_H
