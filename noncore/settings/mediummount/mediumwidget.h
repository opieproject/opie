
#ifndef MediumMountWidget_H
#define MediumMountWidget_H

//#include <qpixmap.h>
#include <qwidget.h>


class QLabel;
class QPixmap;
class Config;
class QGridLayout;
class QCheckBox;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class QVBox;
class QHBox;
class QLineEdit;

namespace MediumMountSetting {

  class MediumMountWidget : public QWidget {
    Q_OBJECT
  public:
    MediumMountWidget(const QString&, const QPixmap &, QWidget *parent, const char *name = 0 );
    ~MediumMountWidget();
    
  private slots:
    void slotAdd();
    void slotStateChanged();

  private:
    void readConfig();
    void initGUI();
    void writeConfig();

    bool m_dirty : 1;

    class MediumMountWidgetPrivate;
    MediumMountWidgetPrivate *d;

    QString m_path;

    Config *m_config;

    QHBox *m_infoBox;
    QLabel *m_label;
    QLabel *m_desc;

    QLineEdit *m_edit;
    QPushButton *m_add;

    QVBoxLayout *m_box;
    QGridLayout *m_checks;

    QGroupBox *m_group;
    QCheckBox *m_all;
    QCheckBox *m_audio;
    QCheckBox *m_image;
    QCheckBox *m_text;
    QCheckBox *m_video;

    QCheckBox *m_always;
    //QCheckBox *m_yesNo;

    QHBox *m_hboxAdd;

    QLabel *m_lblPath;
    ////////////////

   

  };
};
#endif
