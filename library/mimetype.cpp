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

#include "mimetype.h"
#include "applnk.h"
#include "resource.h"
#include "qpeapplication.h"
#include <qfile.h>
#include <qdict.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qmap.h>
 
class MimeTypeData {
public:
    MimeTypeData(const QString& i, const AppLnk& lnk, const QString& icon ) :
	id(i),
	desc(lnk.name()+" document"),
	app(lnk)
    {
	if ( icon.isEmpty() ) {
	    regIcon = lnk.pixmap();
	    bigIcon = lnk.bigPixmap();
	} else {
	    QImage unscaledIcon = Resource::loadImage( icon );
	    regIcon.convertFromImage( unscaledIcon.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
	    bigIcon.convertFromImage( unscaledIcon.smoothScale( AppLnk::bigIconSize(), AppLnk::bigIconSize() ) );
	}
    }
    QString id;
    QString desc;
    QPixmap regIcon;
    QPixmap bigIcon;
    AppLnk app;
};

class MimeType::Dict : public QDict<MimeTypeData> {
public:
    Dict() {}
};

MimeType::Dict* MimeType::d=0;
static QMap<QString,QString> *typeFor = 0;

MimeType::Dict& MimeType::dict()
{
    if ( !d ) {
	d = new Dict;
	d->setAutoDelete(TRUE);
    }
    return *d;
}

MimeType::MimeType( const QString& ext_or_id )
{
    init(ext_or_id);
}

MimeType::MimeType( const DocLnk& lnk )
{
    init(lnk.type());
}

QString MimeType::id() const
{
    return i;
}

QString MimeType::description() const
{
    MimeTypeData* d = data(i);
    return d ? d->desc : QString::null;
}

QPixmap MimeType::pixmap() const
{
    MimeTypeData* d = data(i);
    return d ? d->regIcon : QPixmap();
}

QPixmap MimeType::bigPixmap() const
{
    MimeTypeData* d = data(i);
    return d ? d->bigIcon : QPixmap();
}

const AppLnk* MimeType::application() const
{
    MimeTypeData* d = data(i);
    return d ? &d->app : 0;
}

void MimeType::registerApp( const AppLnk& lnk )
{
    QStringList list = lnk.mimeTypes();
    QStringList icons = lnk.mimeTypeIcons();
    QStringList::ConstIterator icon = icons.begin();
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it, ++icon) {
	MimeTypeData *item = new MimeTypeData( *it, lnk, *icon );
        dict().replace( *it, item );
    }
}

void MimeType::clear()
{
    delete d;
    d = 0;
}

void MimeType::loadExtensions()
{
    if ( !typeFor ) {
	typeFor = new QMap<QString,QString>;
	loadExtensions("/etc/mime.types");
	loadExtensions(QPEApplication::qpeDir()+"etc/mime.types");
    }
}

void MimeType::loadExtensions(const QString& filename)
{
    QFile file(filename);
    if ( file.open(IO_ReadOnly) ) {
	QTextStream in(&file);
	QRegExp space("[ \t]+");
	while (!in.atEnd()) {
	    QStringList tokens = QStringList::split(space, in.readLine());
	    QStringList::ConstIterator it = tokens.begin();
	    if ( it != tokens.end() ) {
		QString id = *it; ++it;
		while (it != tokens.end()) {
		    (*typeFor)[*it] = id;
		    ++it;
		}
	    }
	}
    }
}

void MimeType::init( const QString& ext_or_id )
{
    if ( ext_or_id[0] != '/' && ext_or_id.contains('/') ) {
	i = ext_or_id.lower();
    } else {
	loadExtensions();
	int dot = ext_or_id.findRev('.');
	QString ext = dot >= 0 ? ext_or_id.mid(dot+1) : ext_or_id;
	i = (*typeFor)[ext.lower()];
	if ( i.isNull() )
	    i = "application/octet-stream";
    }
    static bool appsUpdated = FALSE;
    if ( !appsUpdated ) {
	appsUpdated = TRUE;
	updateApplications();
    }
}

MimeTypeData* MimeType::data(const QString& id)
{
    MimeTypeData* d = dict()[id];
    if ( !d ) {
	int s = id.find('/');
	QString idw = id.left(s)+"/*";
	d = dict()[idw];
    }
    return d;
}

QString MimeType::appsFolderName()
{
    return QPEApplication::qpeDir() + "apps";
}

void MimeType::updateApplications()
{
    clear();
    AppLnkSet apps( appsFolderName() );
    updateApplications(&apps);
}

void MimeType::updateApplications(AppLnkSet* folder)
{
    for ( QListIterator<AppLnk> it( folder->children() ); it.current(); ++it ) {
	registerApp(*it.current());
    }
}
