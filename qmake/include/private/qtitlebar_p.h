/****************************************************************************
** $Id: qtitlebar_p.h,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Definition of some Qt private functions.
**
** Created : 2000-01-01
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QTITLEBAR_P_H
#define QTITLEBAR_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qworkspace.cpp and qdockwindow.cpp.  This header file may change
// from version to version without notice, or even be removed.
//
// We mean it.
//
//


#ifndef QT_H
#include "qbutton.h"
#include "qlabel.h"
#endif // QT_H

#if !defined(QT_NO_TITLEBAR)

class QToolTip;
class QTitleBarPrivate;
class QPixmap;

class Q_EXPORT QTitleBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( bool autoRaise READ autoRaise WRITE setAutoRaise )
    Q_PROPERTY( bool movable READ isMovable WRITE setMovable )

public:
    QTitleBar (QWidget* w, QWidget* parent, const char* name=0);
    ~QTitleBar();

    bool isActive() const;
    bool usesActiveColor() const;
    virtual QString visibleText() const;

    bool isMovable() const;
    void setMovable(bool);

    bool autoRaise() const;
    void setAutoRaise(bool);

    QWidget *window() const;

    QSize sizeHint() const;

#ifdef QT_NO_WIDGET_TOPEXTRA
    // We provide one, since titlebar is useless otherwise.
    QString caption() const;
#endif

public slots:
    void setActive( bool );
    void setCaption( const QString& title );
    void setIcon( const QPixmap& icon );

signals:
    void doActivate();
    void doNormal();
    void doClose();
    void doMaximize();
    void doMinimize();
    void doShade();
    void showOperationMenu();
    void popupOperationMenu( const QPoint& );
    void doubleClicked();

protected:
    bool event( QEvent *);
    void resizeEvent( QResizeEvent *);
    void contextMenuEvent( QContextMenuEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseDoubleClickEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void enterEvent( QEvent *e );
    void leaveEvent( QEvent *e );
    void paintEvent( QPaintEvent *p );

    virtual void cutText();

private:
    void readColors();

    QTitleBarPrivate *d;
#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    QTitleBar( const QTitleBar & );
    QTitleBar &operator=( const QTitleBar & );
#endif
};

#endif
#endif //QTITLEBAR_P_H
