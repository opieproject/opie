/*
                             This file is part of the OPIE Project
                             
               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef INSTALLDLGIMPL_H
#define INSTALLDLGIMPL_H

using namespace std;

#include <qlist.h>
#include <qstring.h>
#include <qwidget.h>

class QComboBox;
class QLabel;
class QMultiLineEdit;
class QPushButton;

class DataManager;
class Destination;
class Ipkg;

class InstallData
{
public:
    QString option;            // I - install, D - delete, R- reinstall U - upgrade
    QString packageName;
    Destination *destination;
    bool recreateLinks;
};

class InstallDlgImpl : public QWidget
{
    Q_OBJECT
public:
    InstallDlgImpl( const QList<InstallData> &packageList, DataManager *dataManager, const char *title = 0 );
    InstallDlgImpl( Ipkg *ipkg, QString initialText, const char *title = 0 );
    ~InstallDlgImpl();

	bool upgradeServer( QString &server );

protected:

private:
    DataManager *dataMgr;
    QList<InstallData> packages;
	bool firstPackage;
    int flags;
    int infoLevel;
    Ipkg *pIpkg;
    bool upgradePackages;
    
    QComboBox      *destination;
    QPushButton    *btnInstall;
    QPushButton    *btnOptions;
    QMultiLineEdit *output;
    QLabel         *txtAvailableSpace;

    void init( bool );
    
    bool runIpkg( QString &option, const QString& package, const QString& dest, int flags );

signals:
    void reloadData( InstallDlgImpl * );
        
public slots:
    void optionsSelected();
    void installSelected();
    void displayText(const QString &text );
    void displayAvailableSpace( const QString &text);
	void ipkgFinished();
};

#endif
