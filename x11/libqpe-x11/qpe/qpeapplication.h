#ifndef OPIE_QPE_APPLICATION_H
#define OPIE_QPE_APPLICATION_H

/**
 * LGPLed
 */

#include <qapplication.h>
#include <qevent.h>

#include <qpe/timestring.h>

class QCopChannel;
class QPEApplication : public QApplication {
    Q_OBJECT
public:
    QPEApplication(int& argc,  char** argv, Type=GuiClient );
    ~QPEApplication();


    static QString qpeDir();
    static QString documentDir();
    void applyStyle();

    static int defaultRotation();
    static void setDefaultRotation( int r );
    static void grabKeyboard();
    static void ungrabKeyboard();

    enum StylusMode {
        LeftOnly,
        RightOnHold
    };
    static void setStylusOperation( QWidget*, StylusMode );
    static StylusMode stylusOperation( QWidget* );

    enum InputMethodHint {
	Normal,
	AlwaysOff,
	AlwaysOn
    };
    static void setInputMethodHint( QWidget*, InputMethodHint );
    static InputMethodHint inputMethodHint( QWidget* );

    void showMainWidget( QWidget*, bool nomax = FALSE );
    void showMainDocumentWidget( QWidget*, bool nomax = FALSE );

    static void showDialog( QDialog*, bool nomax = FALSE );
    static int execDialog( QDialog*, bool nomax = FALSE );

    static void setKeepRunning();
    bool keepRunning()const;

    bool keyboardGrabbed()const;
    int exec();

// QWS bits
    enum screenSaverHint {
	Disable = 0,
	DisableLightOff = 1,
	DisableSuspend = 2,
	Enable = 100
    };
    /* reimplemented for internal purposes */
    int x11ClientMessage( QWidget*, XEvent*, bool );

signals:
    void clientMoused();
    void timeChanged();
    void clockChanged( bool pm );
    void micChanged( bool muted );
    void volumeChanged( bool muted );
    void appMessage( const QCString& msg, const QByteArray& data);
    void weekChanged( bool startOnMonday );
    void dateFormatChanged( DateFormat );
    void flush();
    void reload();

private:
    void initTranslations();
    void internalSetStyle(const QString&);

private slots:
    void hideOrQuit();
    void systemMessage( const QCString&, const QByteArray& );
    void pidMessage( const QCString&, const QByteArray& );
    void removeSenderFromStylusDict();
protected:
    virtual void restart();
    virtual void shutdown();
    void prepareForTermination(bool willrestart);
    bool eventFilter( QObject*, QEvent* );
    void timerEvent( QTimerEvent* );
    void raiseAppropriateWindow();
    virtual void tryQuit();

private:
    class Private;
    Private* d;
    QCopChannel *m_sys;
    QCopChannel *m_pid;
};

#endif
