/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
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
#ifndef Addressbook_H
#define Addressbook_H

// Remove this for OPIE releae 1.0 !
#define __DEBUG_RELEASE

#include <qmainwindow.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include "ofloatbar.h"
#include "abview.h"
#include "abconfig.h"

class ContactEditor;
class AbLabel;
class AbTable;
class QPEToolBar;
class QPopupMenu;
class QToolButton;
class QDialog;
class Ir;
class QAction;
class LetterPicker;

class AddressbookWindow: public QMainWindow
{
	Q_OBJECT
public:
	enum EntryMode { NewEntry=0, EditEntry };

	AddressbookWindow( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
	~AddressbookWindow();

protected:
	void resizeEvent( QResizeEvent * e );

	void editPersonal();
	void editEntry( EntryMode );
	void closeEvent( QCloseEvent *e );
	bool save();

public slots:
	void flush();
	void reload();
	void appMessage(const QCString &, const QByteArray &);
	void setDocument( const QString & );
#ifdef __DEBUG_RELEASE
	void slotSave();
#endif

private slots:
	void importvCard();
	void slotListNew();
/* 	void slotListView(); */
	void slotListDelete();
	void slotViewBack();
	void slotViewEdit();
	void slotPersonalView();
	void listIsEmpty( bool );
/*  	void slotSettings();  */
	void writeMail();
	void slotBeam();
	void beamDone( Ir * );
	void slotSetCategory( int );
	void slotSetLetter( char );
	void slotUpdateToolbar();
	void slotSetFont(int);

	void slotFindOpen();
	void slotFindClose();
	void slotFind();
	void slotNotFound();
	void slotWrapAround();

	void slotViewSwitched( int );

	void slotConfig();

private:
	void initFields();  // inititialize our fields...
	// AbLabel *abView();
	void populateCategories();

	QPopupMenu *catMenu;
	QPEToolBar *listTools;
	QToolButton *deleteButton;
	QValueList<int> allFields, orderedFields;
	QStringList slOrderedFields;
	enum Panes { paneList=0, paneView, paneEdit };
	ContactEditor *abEditor;
	LetterPicker *pLabel;
	AbView* m_abView;
	QWidget *listContainer;

	// Searching stuff
	OFloatBar* searchBar;
	QLineEdit* searchEdit;

	QAction *actionNew, *actionEdit, *actionTrash, *actionFind, *actionBeam, *actionPersonal, *actionMail;

	int viewMargin;

	bool syncing;
	QFont *defaultFont;
	int m_curFontSize;

	bool isLoading;

	AbConfig m_config;
};

#endif
