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

class MediumMountGui : public QDialog
{ 
    Q_OBJECT

public:
    MediumMountGui( const QString &path = "/tmp/", QWidget* parent = 0, const char* name = 0, bool modal = FALSE    ,WFlags fl = 0 );
    ~MediumMountGui();

protected:    
    QPushButton* quit;
    QPushButton* quit_2;
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
    QCheckBox* AskBox;

public:
    bool check();
    QStringList mimeTypes();
    QStringList dirs();
private:
    void startGui();
    void readConfig();
    void writeConfig();	

private slots:
	void yesPressed();
	void noPressed();
private:
    QString mediumPath;

    bool checkagain; 
    bool checkmimeaudio;
    bool checkmimeimage;
    bool checkmimetext;
    bool checkmimevideo;
QString limittodirs;
QStringList mimeTypeList;

};

#endif 