

#ifndef MediumGlobalWidget_H
#define MediumGlobalWidget_H

#include <qwidget.h>

class Config;
class QCheckBox;
class QGroupBox;
class QFrame;
class QLineEdit;
class QVBoxLayout;
class QGridLayout;
class QLabel;

namespace MediumMountSetting {
  enum States { GLOBAL_ENABLED = 0, GLOBAL_DISABLED };
  enum Checks { ENABLE_CHECKS  = 0, DISABLE_CHECKS  };

  class MediumGlobalWidget : public QWidget {
    Q_OBJECT 
  public:
    MediumGlobalWidget(QWidget *parent = 0, const char *name =0 );
    ~MediumGlobalWidget();
  signals:
    // the global status changed
    void globalStateChanged( int );
    void enableStateChanged( int );
  private slots:
    void slotGlobalChanged();
    void slotEnableChecking();
    void slotAllChanged();

  private:
    void initGUI();
    void readConfig();
    void writeConfig();
    Config *m_config;
    QCheckBox *m_check;
    QCheckBox *m_useglobal;

    QGroupBox *m_global;

    QCheckBox *m_all;
    QCheckBox *m_audio;
    QCheckBox *m_video;
    QCheckBox *m_text;
    QCheckBox *m_image;

    QFrame *m_frame;
    QGridLayout *m_frameLay;
    QVBoxLayout *m_layout;
    QVBoxLayout *m_box;
    QLabel *m_label;
  };
};

#endif
