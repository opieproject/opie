/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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


//
//   DO NOT ATTEMPT TO USE THIS CLASS
//



#ifndef LNKPROPERTIES_H
#define LNKPROPERTIES_H
#include <qstringlist.h>
#include <qdialog.h>

class AppLnk;
class QListViewItem;
class DocLnk;

class LnkPropertiesBase;

class LnkProperties : public QDialog
{
    Q_OBJECT

public:
// WARNING: Do *NOT* define this yourself. The SL5xxx from SHARP does NOT
//      have this class.
#ifdef QTOPIA_INTERNAL_FSLP
    LnkProperties( AppLnk* lnk, QWidget* parent = 0 );
    ~LnkProperties();
#endif

    void done(int);
private slots:
    void beamLnk();
    void unlinkLnk();
    void unlinkIcon();
    void duplicateLnk();

signals:
    void select(const AppLnk *);

private:
    void setupLocations();
    bool moveLnk();
    bool copyFile( DocLnk &newdoc );

    AppLnk* lnk;
    int fileSize;
    int currentLocation;
    QStringList locations;
    LnkPropertiesBase *d;
};

#endif // LNKPROPERTIES_H
