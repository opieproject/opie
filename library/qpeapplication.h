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


#include <qapplication.h>
#include <qdialog.h>
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

private slots:
    void systemMessage( const QCString &msg, const QByteArray &data );
    void pidMessage( const QCString &msg, const QByteArray &data );
    void removeSenderFromStylusDict();
    void hideOrQuit();

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

private:
    void mapToDefaultAction( QWSKeyEvent *ke, int defKey );

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


#endif

