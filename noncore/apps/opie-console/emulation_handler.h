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
class TEWidget;
class TEmulation;
class QFont;
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
    void setColor( const QColor& fore, const QColor& back );
signals:
    void send( const QByteArray& );
    void changeSize(int rows, int cols );


public slots:
    void recv( const QByteArray& );

private slots:
    void recvEmulation( const char*,  int len );
private:
    QFont font( int );
    QColor foreColor(int );
    QColor backColor(int );

private:
    TEWidget* m_teWid;
    TEmulation* m_teEmu;

};

#endif
