#include <qwidget.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qmessagebox.h>

#include "qcheckmmbase.h"

class QCheckMainMenu : public QCheckMMBase
{
  Q_OBJECT
  public:
    QCheckMainMenu(QWidget *);
  signals:
    void itemSelected(const QString &);
  private:
    void init();
  private slots:
    void slotSelected(QListBoxItem *);
  public slots:
    void newClicked();
    void deleteClicked();
};

