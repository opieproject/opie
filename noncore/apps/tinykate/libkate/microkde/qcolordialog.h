/****************************************************************************
** $Id: qcolordialog.h,v 1.1 2003-06-14 17:39:27 llornkcor Exp $
**
** Definition of KColorDialog class
**
** Created : 990222
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
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

#ifndef KColorDialog_H
#define KColorDialog_H

#ifndef QT_H
#include <qdialog.h>
#endif // QT_H

class KColorDialogPrivate;

class Q_EXPORT KColorDialog : public QDialog
{
    Q_OBJECT

public:
    static QColor getColor( QColor, QWidget *parent=0, const char* name=0 ); // ### 3.0: make const QColor&
    static QRgb getRgba( QRgb, bool* ok = 0,
			 QWidget *parent=0, const char* name=0 );


    static int customCount();
    static QRgb customColor( int );
    static void setCustomColor( int, QRgb );

private:
    ~KColorDialog();

    KColorDialog( QWidget* parent=0, const char* name=0, bool modal=FALSE );
    void setColor( QColor ); // ### 3.0: make const QColor&
    QColor color() const;

private:
    void setSelectedAlpha( int );
    int selectedAlpha() const;

    void showCustom( bool=TRUE );
private:
    KColorDialogPrivate *d;
    friend class KColorDialogPrivate;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    KColorDialog( const KColorDialog & );
    KColorDialog& operator=( const KColorDialog & );
#endif
};

#endif //KColorDialog_H
