

#ifndef MediumMountMainwindow_h
#define MediumMountMainwindow_h

#include <qlist.h>
#include <qdialog.h>

class QVBoxLayout;
class QTabWidget;

namespace MediumMountSetting {
  class MediumMountWidget;
  class MediumGlobalWidget;
  class MainWindow : public QDialog {
    Q_OBJECT
  public:
    MainWindow(QWidget *parent = 0, const char *name = 0 ,  bool modal = FALSE, WFlags = 0);
    ~MainWindow();

  private slots:
    void slotGlobalChanged(int );
    void slotCheckingChanged(int );
    void accept();
  private:
    void init();
    QTabWidget *m_tab;
    QVBoxLayout *m_lay;
    MediumGlobalWidget *m_global;
    QList<MediumMountWidget> m_mediums;
  };
};

#endif
