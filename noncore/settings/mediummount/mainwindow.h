

#ifndef MediumMountMainwindow_h
#define MediumMountMainwindow_h

#include <qlist.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QTabWidget;

namespace MediumMountSetting {
  class MediumMountWidget;
  class MediumGlobalWidget;
  class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = 0, const char *name = 0 , WFlags = 0);
    ~MainWindow();

  private slots:
    void slotGlobalChanged(int );
    void slotCheckingChanged(int );
  private:
    void init();
    QTabWidget *m_tab;
    QVBoxLayout *m_lay;
    MediumGlobalWidget *m_global;
    QList<MediumMountWidget> m_mediums;
  };
};

#endif
