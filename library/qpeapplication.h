/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef __QPE_APPLICATION_H__
#define __QPE_APPLICATION_H__

#include <stdlib.h> // for setenv()

#include <qglobal.h>
#include <qapplication.h>
#include <qdialog.h>
#include <qwsdisplay_qws.h>
#if defined(_WS_QWS_) && !defined(Q_WS_QWS)
#define Q_WS_QWS
#endif
#include "qpedecoration_qws.h"
#include "timestring.h"

class QCopChannel;
class QPEApplicationData;
class QWSEvent;
class QWSKeyEvent;


class QPEApplication : public QApplication
{
    Q_OBJECT
public:
    QPEApplication( int& argc, char **argv, Type=GuiClient );
    ~QPEApplication();

    static QString qpeDir();
    static QString documentDir();
    void applyStyle();
    static int defaultRotation();
    static void setDefaultRotation(int r);
    static void setCurrentRotation(int r);
    static void grabKeyboard();
    static void ungrabKeyboard();

    enum StylusMode {
        LeftOnly,
        RightOnHold
        // RightOnHoldLeftDelayed, etc.
    };
    static void setStylusOperation( QWidget*, StylusMode );
    static StylusMode stylusOperation( QWidget* );

    enum InputMethodHint {
        Normal,
        AlwaysOff,
        AlwaysOn
    };

    enum screenSaverHint {
        Disable = 0,
        DisableLightOff = 1,
        DisableSuspend = 2,
        Enable = 100
    };

    static void setInputMethodHint( QWidget *, InputMethodHint );
    static InputMethodHint inputMethodHint( QWidget * );

    void showMainWidget( QWidget*, bool nomax=FALSE );
    void showMainDocumentWidget( QWidget*, bool nomax=FALSE );
    static void showDialog( QDialog*, bool nomax=FALSE );
    static int execDialog( QDialog*, bool nomax=FALSE );
    /* Merge setTempScreenSaverMode */
#ifdef QTOPIA_INTERNAL_INITAPP
    void initApp( int argv, char **argv );
#endif

    static void setKeepRunning();
    bool keepRunning() const;

    bool keyboardGrabbed() const;

    int exec();

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
    /* linkChanged signal */

private slots:
    void systemMessage( const QCString &msg, const QByteArray &data );
    void pidMessage( const QCString &msg, const QByteArray &data );
    void removeSenderFromStylusDict();
    void hideOrQuit();

private:
    inline QWidget *nextWidget( QWidgetList*, QWidget* );

protected:
    bool qwsEventFilter( QWSEvent * );
    void internalSetStyle( const QString &style );
    void prepareForTermination(bool willrestart);
    virtual void restart();
    virtual void shutdown();
    bool eventFilter( QObject *, QEvent * );
    void timerEvent( QTimerEvent * );
    bool raiseAppropriateWindow();
    virtual void tryQuit();
#if QT_VERSION > 233
    virtual void polish ( QWidget * ); // this is actually implemented in qt_override.cpp (!)
#endif
private:
#ifndef QT_NO_TRANSLATION
    void installTranslation( const QString& baseName );
#endif
    void mapToDefaultAction( QWSKeyEvent *ke, int defKey );
    void processQCopFile();

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopChannel *sysChannel;
    QCopChannel *pidChannel;
#endif
    QPEApplicationData *d;

    bool reserved_sh;



};

inline void QPEApplication::showDialog( QDialog* d, bool nomax )
{
    QSize sh = d->sizeHint();
    int w = QMAX(sh.width(),d->width());
    int h = QMAX(sh.height(),d->height());
    if ( !nomax
            && ( w > qApp->desktop()->width()*3/4
                 || h > qApp->desktop()->height()*3/4 ) )
    {
        d->showMaximized();
    } else {
        d->resize(w,h);
        d->show();
    }
}

inline int QPEApplication::execDialog( QDialog* d, bool nomax )
{
    showDialog(d,nomax);
    return d->exec();
}

enum Transformation { Rot0, Rot90, Rot180, Rot270 }; /* from qgfxtransformed_qws.cpp */

inline int TransToDeg ( Transformation t )
{
	int d = static_cast<int>( t );
	return d * 90;
}

inline Transformation DegToTrans ( int d )
{
	Transformation t = static_cast<Transformation>( d / 90 );
	return t;
}

/*
 * Set current rotation of Opie, and rotation for newly started apps.
 * Differs from setDefaultRotation in that 1) it rotates currently running apps,
 * and 2) does not set deforient or save orientation to qpe.conf.
 */

inline void QPEApplication::setCurrentRotation( int r )
{
    // setTransformation has been introduced in Qt/Embedded 2.3.4 snapshots
    // for compatibility with the SharpROM use fallback to setDefaultTransformation()
#if QT_VERSION > 233
    Transformation e = DegToTrans( r );
    ::setenv( "QWS_DISPLAY", QString( "Transformed:Rot%1:0" ).arg( r ).latin1(), 1 );
    qApp->desktop()->qwsDisplay()->setTransformation( e );
#else
    setDefaultRotation( r );
#endif
}


#endif
