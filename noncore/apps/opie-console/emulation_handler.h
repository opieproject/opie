#ifndef OPIE_EMULATION_HANDLER_H
#define OPIE_EMULATION_HANDLER_H

#include <qobject.h>
#include <qcolor.h>
#include <qcstring.h>

/*
 * Badly ibotty lacks the time to finish
 * his widget in time..
 * Never the less we've to have an EmulationWidget
 * This is why I'm taking the inferior not cleaned
 * up TE* KDE STUFF
 */

/**
 * This is the layer above the IOLayer*
 * This nice QObject here will get stuff from
 * got a slot and a signal
 * the signal for data
 * the slot for receiving
 * it'll set up the widget internally
 * and manage the communication between
 * the pre QByteArray world!
 */
class Profile;
class QWidget;
class QPushButton;
class TEWidget;
class TEmulation;
class QFont;
class Script;
class EmulationHandler : public QObject {
    Q_OBJECT
public:
    /**
     * simple c'tor the parent of the TEWdiget
     * and a name
     * and a Profile
     */
    EmulationHandler( const Profile&, QWidget* parent, const char* name = 0l );

    /**
     * delete all components
     */
    ~EmulationHandler();

    void load( const Profile& );
    QWidget* widget();
    TEmulation *emulation();
    void setColor( const QColor& fore, const QColor& back );
    QPushButton* cornerButton();

    /* Scripts */
    /* Create a new script and record all typed characters */
    void startRecording();

    /* Return whether we are currently recording a script */
    bool isRecording();

    /* Return the current script (or NULL) */
    Script *script();

    /* Stop recording and remove the current script from memory */
    void clearScript();

    /* Run a script by forwarding its keys to the EmulationLayer */
    void runScript(const Script *);

	/* Propagate change to widget */
	void setWrap(int columns);
signals:
    void send( const QByteArray& );
    void changeSize(int rows, int cols );


public slots:
    void recv( const QByteArray& );
    void paste();
    void copy();

private slots:
    void recvEmulation( const char*,  int len );
private:
    QFont font( int );
    QColor foreColor(int );
    QColor backColor(int );

private:
    TEWidget* m_teWid;
    TEmulation* m_teEmu;
    Script * m_script;
};

#endif
