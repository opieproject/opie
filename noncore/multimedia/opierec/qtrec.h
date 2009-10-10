/****************************************************************************
 ** Created: Thu Jan 17 11:19:45 2002
copyright 2002 by L.J. Potter ljp@llornkcor.com
****************************************************************************/
#ifndef QTREC_H
#define QTREC_H

#include <qpe/ir.h>

#include <qlineedit.h>
#include <qwidget.h>

#include <opie2/owavrecorder.h>
#include <opie2/owavplayer.h>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QIconView;
class QIconViewItem;
class QLabel;
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;
class QSlider;
class QTabWidget;
class QTimer;
class QVBoxLayout;
class QLineEdit;

//#define BUFSIZE 4096
// #define BUFSIZE 8182 //Z default buffer size
#define BUFSIZE 1024
//#define BUFSIZE 2048

using namespace Opie::MM;

enum playerMode { MODE_IDLE, MODE_PLAYING, MODE_PAUSED, MODE_STOPPING, MODE_RECORDING };

class QtRec : public QWidget, public OWavRecorderCallback, public OWavPlayerCallback
{
   Q_OBJECT

public:
    static QString appName() { return QString::fromLatin1("opierec"); }
    QtRec( QWidget* parent=0, const char* name=0, WFlags fl=0 );
    ~QtRec();
    QSlider *OutputSlider,*InputSlider;

signals:
    void stopRecording();
    void startRecording();
    void stopPlaying();
    void startPlaying();

public slots:

private:
    int secCount;
    QString m_dirPath;

    QLineEdit *renameBox;
    QTimer *t_timer;
    bool autoMute;

    bool eventFilter( QObject * , QEvent * );
    void okRename();
    void cancelRename();
    QString getStorage(const QString &);
    bool rec();
    int getCurrentSizeLimit();
    long checkDiskSpace(const QString &);
    void doMute(bool);
    void errorStop();
    void fillDirectoryCombo();
    void getInVol();
    void getOutVol();
    void init();
    void initConfig();
    void initConnections();
    void selectItemByName(const QString &);
    void start();
    void stop();
    void timerEvent( QTimerEvent *e );
    void setButtons();
    void fileSize(unsigned long size, QString &str);
    QString getSelectedFile();
    void recorderCallback(const char *buffer, const int bytes, const int totalbytes, bool &stopflag);
    void playerCallback(const char *buffer, const int bytes, int &position, bool &stopflag, bool &paused);

private slots:
    void endPlaying();
    void endRecording();

    void FastforwardPressed();
    void FastforwardReleased();

    void changeDirCombo(int);
    void changeSizeLimitCombo(int);
    void changeTimeSlider(int);
    void changebitrateCombo(int);
    void changeStereoCheck( bool);

    void changedInVolume();
    void changedOutVolume();
    void changesamplerateCombo(int);

    void cleanUp();
    void compressionSelected(bool);
    void deleteSound();
    void doBeam();
    void doMicMuting(bool);
    void doPlayBtn();
    void doRename();
    void doVolMuting(bool);
    void forwardTimerTimeout();
    void listPressed(int, QListViewItem *, const QPoint&, int);
    void newSound();
    void rewindPressed();
    void rewindReleased();
    void rewindTimerTimeout();
    void slotAutoMute(bool);
    void thisTab(QWidget*);
    void timeSliderPressed();
    void timeSliderReleased();
    void initIconView();
/*     void changedOutVolume(int); */
/*    void changedInVolume(int); */
    void receive( const QCString &msg, const QByteArray &data );

protected:

    OWavRecorder m_recorder;
    OWavPlayer m_player;
    QCheckBox *outMuteCheckBox, *inMuteCheckBox, *compressionCheckBox, *autoMuteCheckBox, *stereoCheckBox;
    QComboBox* sampleRateComboBox, * bitRateComboBox, *directoryComboBox, *sizeLimitCombo;
    QLabel *playLabel2;
    QLabel *TextLabel3, *TextLabel1, *TextLabel2;
    QListView *ListView1;
    QPushButton *Stop_PushButton, *Play_PushButton, *Rec_PushButton, *deleteSoundButton, *toBeginningButton, *toEndButton;
    QTabWidget *TabWidget;
    QTimer *rewindTimer, *forwardTimer;
    QVBoxLayout* Layout15;
    QVBoxLayout* Layout15b;
    QVBoxLayout* Layout18;
    QWidget *tab, *tab_3, *tab_5;
    int sliderPos, total;
    QGroupBox *sampleGroup, *bitGroup, *dirGroup, *sizeGroup;


    bool doPlay();
    bool openPlayFile();
    void setupFileName();
    bool setupAudio( bool b);
    void fileBeamFinished( Ir *ir);
    void keyPressEvent( QKeyEvent *e);
    void keyReleaseEvent( QKeyEvent *e);
    void hideEvent( QHideEvent * );
    void showListMenu(QListViewItem * );
    void playIt();

};

#endif // QTREC_H
