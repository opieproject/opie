#ifndef MEDIUMMOUNTGUI_H
#define MEDIUMMOUNTGUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class Config;

class MediumMountGui : public QDialog {
    Q_OBJECT

public:
    MediumMountGui( Config *cfg, const QString &path = "tmp/", QWidget* parent = 0, const char* name = 0, bool modal = FALSE  ,WFlags fl = 0 );
    ~MediumMountGui();

protected:
    QLabel* DirSelectText_2;
    QLineEdit* LineEdit1;
    QPushButton* PushButton3;
    QLabel* DirSelectText;
    QLabel* Text_2;
    QLabel* Text;
    QGroupBox* GroupBox1;
    QCheckBox* CheckBoxAudio;
    QCheckBox* CheckBoxImage;
    QCheckBox* CheckBoxText;
    QCheckBox* CheckBoxVideo;
    QCheckBox* CheckBoxAll;
    QCheckBox* CheckBoxLink;
    QCheckBox* AskBox;

public:
    bool check();
    QStringList mimeTypes();
    QStringList dirs();
private:
    void startGui();
    void readConfig();
    void writeConfig( bool checkagain );

private slots:
    void accept();
    void reject();
    void deactivateOthers();
 private:
    QString mediumPath;
    bool checkagain:1;
    bool checkmimeaudio:1;
    bool checkmimeimage:1;
    bool checkmimetext:1;
    bool checkmimevideo:1;
    bool checkmimeall:1;
    QString limittodirs;
    QStringList mimeTypeList;
    Config *m_cfg;

};

#endif
