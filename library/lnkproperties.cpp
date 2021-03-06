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

// WARNING: Do *NOT* define this yourself. The SL5xxx from SHARP does NOT
//      have this class.
#define QTOPIA_INTERNAL_FSLP
#include "lnkpropertiesbase_p.h"
#include "lnkproperties.h"
#include "ir.h"

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/categorywidget.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/filemanager.h>
#include <qpe/config.h>
#include <qpe/storage.h>
#include <qpe/qpemessagebox.h>
#include <qpe/mimetype.h>

#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qsize.h>
#include <qcombobox.h>
#include <qregexp.h>
#include <qbuttongroup.h>

#include <stdlib.h>

LnkProperties::LnkProperties( AppLnk* l, QWidget* parent )
    : QDialog( parent, 0, TRUE ), lnk(l), fileSize( 0 )
{
    setCaption( tr("Properties") );

    QVBoxLayout *vbox = new QVBoxLayout( this );
    d = new LnkPropertiesBase( this );
    vbox->add( d );

    // hide custom rotation feature for now, need a new implementation to fit quicklauch,
    // is confusing for the user and doubtable useful since life rotation
    d->rotate->hide();
    d->rotateButtons->hide();

    d->docname->setText(l->name());
    QString inf;
    if ( l->type().isEmpty() ) {
	d->type->hide();
	d->typeLabel->hide();
    } else {
	d->type->setText( l->type() );
    }

    if ( l->comment().isEmpty() ) {
	d->comment->hide();
	d->commentLabel->hide();
    } else {
	d->comment->setText( l->comment() );
    }

    connect(d->beam,SIGNAL(clicked()),this,SLOT(beamLnk()));
    if ( lnk->type().contains('/') ) { // A document? (#### better predicate needed)
	connect(d->unlink,SIGNAL(clicked()),this,SLOT(unlinkLnk()));
	connect(d->duplicate,SIGNAL(clicked()),this,SLOT(duplicateLnk()));

	d->docname->setReadOnly( FALSE );
	d->preload->hide();
	d->rotate->hide();
	d->rotateButtons->hide();
	d->labelspacer->hide();

	// ### THIS MUST GO, FIX WIERD BUG in QLAYOUT
	d->categoryEdit->kludge();

	d->categoryEdit->setCategories( lnk->categories(),
					"Document View",
					tr("Document View") );
	setupLocations();
    } else {
	d->unlink->hide();
	d->duplicate->hide();
	d->beam->hide();
	d->hline->hide();
 	d->locationLabel->hide();
	d->locationCombo->hide();

	// Can't edit categories, since the app .desktop files are global,
	// possibly read-only.
 	d->categoryEdit->hide();

	d->docname->setReadOnly( TRUE );

	if ( l->property("CanFastload") == "0" )
	    d->preload->hide();
	if ( !l->property("Rotation"). isEmpty ()) {
	    d->rotate->setChecked ( true );
            //don't use rotate buttons for now (see comment above)
	    //d->rotateButtons->setButton((l->rotation().toInt()%360)/90);
	}
	else {
	    d->rotateButtons->setEnabled(false);
	}

        if ( !l->property( "Arguments" ).isEmpty() )
            d->arguments->setText( l->property( "Arguments" ) );

	Config cfg("Launcher");
	cfg.setGroup("Preload");
	QStringList apps = cfg.readListEntry("Apps",',');
	d->preload->setChecked( apps.contains(l->exec()) );
	if ( Global::isBuiltinCommand(lnk->exec()) )
	    d->preload->hide(); // builtins are always fast

	currentLocation = 0; // apps not movable (yet)
    }
}

LnkProperties::~LnkProperties()
{
}

void LnkProperties::unlinkLnk()
{
    if ( QPEMessageBox::confirmDelete( this, tr("Delete"), lnk->name() ) ) {
	lnk->removeFiles();
	if ( QFile::exists(lnk->file()) ) {
	    QMessageBox::warning( this, tr("Delete"), tr("File deletion failed.") );
	} else {
	    reject();
	}
    }
}

void LnkProperties::setupLocations()
{
    QFileInfo fi( lnk->file() );
    fileSize = fi.size();
    StorageInfo storage;
    const QList<FileSystem> &fs = storage.fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString s;
    QString homeDir = getenv("HOME");
    QString hardDiskHome;
    QString hardDiskPath;
    int index = 0;
    currentLocation = -1;
    for ( ; it.current(); ++it ) {
	// we add 10k to the file size so we are sure we can also save the desktop file
	if ( (ulong)(*it)->availBlocks() * (ulong)(*it)->blockSize() > (ulong)fileSize + 10000 ) {
	    if ( (*it)->isRemovable() ||
		 (*it)->disk() == "/dev/mtdblock1" ||
		 (*it)->disk() == "/dev/mtdblock/1" ||
		 (*it)->disk().left(13) == "/dev/mtdblock" ||
		 (*it)->disk() == "/dev/mtdblock6" ||
		 (*it )->disk() == "/dev/root" ||
		 (*it)->disk() == "tmpfs" ) {
		d->locationCombo->insertItem( (*it)->name(), index );
		locations.append( ( ((*it)->isRemovable() ||
				    (*it)->disk() == "/dev/mtdblock6" ||
				    (*it)->disk() == "tmpfs" )
				    ? (*it)->path() : homeDir) );
		if ( lnk->file().contains( (*it)->path() ) ) {
		     d->locationCombo->setCurrentItem( index );
		     currentLocation = index;
		}
		index++;
	    } else if ( (*it)->name().contains( tr("Hard Disk") ) &&
			homeDir.contains( (*it)->path() ) &&
			(*it)->path().length() > hardDiskHome.length() ) {
		hardDiskHome = (*it)->name();
		hardDiskPath = (*it)->path();
	    }
	}
    }
    if ( !hardDiskHome.isEmpty() ) {
	d->locationCombo->insertItem( hardDiskHome );
	locations.append( hardDiskPath );
	if ( currentLocation == -1 ) { // assume it's the hard disk
	    d->locationCombo->setCurrentItem( index );
	    currentLocation = index;
	}
    }
}

void LnkProperties::duplicateLnk()
{
    // The duplicate takes the new properties.
    DocLnk newdoc( *((DocLnk *)lnk) );
    if ( d->docname->text() == lnk->name() )
	newdoc.setName(tr("Copy of ")+d->docname->text());
    else
	newdoc.setName(d->docname->text());

    if ( !copyFile( newdoc ) ) {
	QMessageBox::warning( this, tr("Duplicate"), tr("File copy failed.") );
	return;
    }
    reject();
}

bool LnkProperties::moveLnk()
{
    DocLnk newdoc( *((DocLnk *)lnk) );
    newdoc.setName(d->docname->text());

    if ( !copyFile( newdoc ) ) {
	QMessageBox::warning( this, tr("Details"), tr("Moving Document failed.") );
	return FALSE;
    }
    // remove old lnk
    lnk->removeFiles();

    return TRUE;
}

void LnkProperties::beamLnk()
{
    Ir ir;
    DocLnk doc( *((DocLnk *)lnk) );
    doc.setName(d->docname->text());
    reject();
    ir.send( doc, doc.comment() );
}

static bool createMimedir(const QString&base,const QString&mimetype)
{
    int pos = 0;
    int stage = 0;
	if (base.length()==0) return FALSE;
    QString _tname = base+"/Documents";
    QDir _dir(_tname+"/"+mimetype);
    if (_dir.exists()) return TRUE;
    pos = mimetype.find("/");
    _dir.setPath(_tname);
    while (stage<2) {
        if (!_dir.exists()) {
            if (!_dir.mkdir(_tname)) {
                qDebug( QString("Creation of dir %1 failed\n").arg(_tname));
                return FALSE;
	        }
        }
        switch(stage) {
        case 0:
            _tname+="/"+mimetype.left(pos);
            break;
        case 1:
            _tname+="/"+mimetype.right(pos-1);
            break;
        default:
            break;
        }
        _dir.setPath(_tname);
        ++stage;
    }
    return TRUE;
}

bool LnkProperties::copyFile( DocLnk &newdoc )
{
    const char *linkExtn = ".desktop";
    QString fileExtn;
    int extnPos = lnk->file().findRev( '.' );
    if ( extnPos > 0 )
	fileExtn = lnk->file().mid( extnPos );

    QString safename = newdoc.name();
    safename.replace(QRegExp("/"),"_");

    QString fn = locations[ d->locationCombo->currentItem() ]
		  + "/Documents/" + newdoc.type();
    if (!createMimedir(locations[ d->locationCombo->currentItem() ],newdoc.type())) {
        return FALSE;
    }
    fn+="/"+safename;
    if ( QFile::exists(fn + fileExtn) || QFile::exists(fn + linkExtn) ) {
	int n=1;
	QString nn = fn + "_" + QString::number(n);
	while ( QFile::exists(nn+fileExtn) || QFile::exists(nn+linkExtn) ) {
	    n++;
	    nn = fn + "_" + QString::number(n);
	}
	fn = nn;
    }
    newdoc.setFile( fn + fileExtn );
    newdoc.setLinkFile( fn + linkExtn );

    // Copy file
    FileManager fm;
    if ( !fm.copyFile( *lnk, newdoc ) )
	return FALSE;
    return TRUE;
}

void LnkProperties::done(int ok)
{
    if ( ok ) {
	bool changed=FALSE;
	bool reloadMime=FALSE;

	if ( lnk->name() != d->docname->text() ) {
	    lnk->setName(d->docname->text());
	    changed=TRUE;
	}
	if ( d->categoryEdit->isVisible() ) {
	    QArray<int> tmp = d->categoryEdit->newCategories();
	    if ( lnk->categories() != tmp ) {
		lnk->setCategories( tmp );
		changed = TRUE;
	    }
	}
	if ( !d->rotate->isHidden()) {
	    QString newrot;

	    if ( d->rotate->isChecked() ) {
		int rot=0;
		for(; rot<4; rot++) {
		    if (d->rotateButtons->find(rot)->isOn())
		        break;
		}
		newrot = QString::number((rot*90)%360);
	    }
	    if ( newrot != lnk->rotation() ) {
		lnk-> setRotation(newrot);
		changed = TRUE;
		reloadMime = TRUE;
	    }
	}
    if ( d->arguments->text() != lnk->property( "Arguments" ) ) {
        lnk->setProperty( "Arguments", d->arguments->text() );
        changed = TRUE;
    }
	if ( d->preload->isHidden() && d->locationCombo->currentItem() != currentLocation ) {
	    moveLnk();
	} else if ( changed ) {
	    lnk->writeLink();
	}

	if ( !d->preload->isHidden() ) {
	    Config cfg("Launcher");
	    cfg.setGroup("Preload");
	    QStringList apps = cfg.readListEntry("Apps",',');
	    QString exe = lnk->exec();
	    if ( apps.contains(exe) != d->preload->isChecked() ) {
		if ( d->preload->isChecked() ) {
		    apps.append(exe);
#ifndef QT_NO_COP
		    QCopEnvelope e("QPE/Application/"+exe.local8Bit(),
				   "enablePreload()");
#endif
		} else {
		    apps.remove(exe);
#ifndef QT_NO_COP
		    QCopEnvelope e("QPE/Application/"+exe.local8Bit(),
				   "quitIfInvisible()");
#endif
		}
		cfg.writeEntry("Apps",apps,',');
	    }
	}
	if ( reloadMime )
	    MimeType::updateApplications ( );
    }
    QDialog::done( ok );
}

