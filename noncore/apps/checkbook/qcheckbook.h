#include "qcheckview.h"
#include "qcheckgraph.h"
#include "qcheckmainmenu.h"

#include <qpe/resource.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qstring.h>
#include "config.h"
#include <qpopupmenu.h>

class QCheckBook : public QMainWindow
{
  Q_OBJECT
  public:
    QCheckBook();
  private slots:
    void newCheck(const QString &filename);
    void deleteCheck(const QString &filename);
    void newGraph();
    void close();
    void initMainMenus();
  private:
    QCheckView *m_view;
    QCheckGraph *m_view2;
    QCheckMainMenu *m_view3;
    QToolBar *bar;
    QMenuBar *mbar;
    int filemenuid;
    QString m_filename;
    QToolButton *nb1;
    QToolButton *nb2;
    QToolButton *nb3;
    QToolButton *nb4;
    bool initCheck;
    bool initMM;
    QPopupMenu *popup;
};
