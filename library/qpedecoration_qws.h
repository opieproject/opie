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
#ifndef QPE_DECORATION_QWS_H__
#define QPE_DECORATION_QWS_H__


#ifdef QWS
#include <qwsdefaultdecoration_qws.h>
#include <qimage.h>
#include <qdatetime.h>
#include <qguardedptr.h>
#include "windowdecorationinterface.h"


#ifndef QT_NO_QWS_QPE_WM_STYLE

class QPEManager;
class QTimer;

class QPEDecoration : public QWSDefaultDecoration
{
public:
    QPEDecoration();
    QPEDecoration( const QString &plugin );
    virtual ~QPEDecoration();

    virtual QRegion region(const QWidget *, const QRect &rect, Region);
    virtual void paint(QPainter *, const QWidget *);
    virtual void paintButton(QPainter *, const QWidget *, Region, int state);

    void maximize( QWidget * );
    void minimize( QWidget * );
    virtual void help( QWidget * );

    enum QPERegion { Help=LastRegion+1 };
    void buttonClicked( QPERegion r );
    
protected:
    virtual int getTitleHeight(const QWidget *);
    virtual const char **menuPixmap();
    virtual const char **closePixmap();
    virtual const char **minimizePixmap();
    virtual const char **maximizePixmap();
    virtual const char **normalizePixmap();

private:
    void windowData( const QWidget *w, WindowDecorationInterface::WindowData &wd ) const;

protected:
    QImage imageOk;
    QImage imageClose;
    QImage imageHelp;
    QString helpFile;
    bool helpExists;
    QPEManager *qpeManager;
};


class QPEManager : public QObject
{
    Q_OBJECT
    friend class QPEDecoration;
public:
    QPEManager( QPEDecoration *d, QObject *parent=0 );

    void updateActive();
    const QWidget *activeWidget() const { return (const QWidget *)active; }
    const QWidget *whatsThisWidget() const { return (const QWidget *)whatsThis; }

protected:
    int pointInQpeRegion( QWidget *w, const QPoint &p );
    virtual bool eventFilter( QObject *, QEvent * );
    void drawButton( QWidget *w, QPEDecoration::QPERegion r, int state );
    void drawTitle( QWidget *w );

protected slots:
    void whatsThisTimeout();

protected:
    QPEDecoration *decoration;
    QGuardedPtr<QWidget> active;
    int helpState;
    QTime pressTime;
    QTimer *wtTimer;
    bool inWhatsThis;
    QGuardedPtr<QWidget> whatsThis;
};


#endif // QT_NO_QWS_QPE_WM_STYLE


#endif // QPE_DECORATION_QWS_H__
#endif // QWS
