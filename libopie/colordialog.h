/****************************************************************************
** $Id: colordialog.h,v 1.2 2002-04-30 01:09:14 drw Exp $
**
** Definition of OColorDialog class
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

#ifndef OColorDialog_H
#define OColorDialog_H

#ifndef QT_H
#include <qdialog.h>
#endif // QT_H

class OColorDialogPrivate;

class Q_EXPORT OColorDialog : public QDialog
{
    Q_OBJECT

public:
    static QColor getColor( QColor, QWidget *parent=0, const char* name=0 ); // ### 3.0: make const QColor&
    static QRgb getRgba( QRgb, bool* ok = 0,
			 QWidget *parent=0, const char* name=0 );

private:
    ~OColorDialog();

    OColorDialog( QWidget* parent=0, const char* name=0, bool modal=FALSE );
    void setColor( QColor ); // ### 3.0: make const QColor&
    QColor color() const;

private:
    void setSelectedAlpha( int );
    int selectedAlpha() const;
private:
    OColorDialogPrivate *d;
    friend class OColorDialogPrivate;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    OColorDialog( const OColorDialog & );
    OColorDialog& operator=( const OColorDialog & );
#endif
};

#endif
