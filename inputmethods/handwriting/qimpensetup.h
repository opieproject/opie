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

#include <qdialog.h>
#include <qlist.h>
#include "qimpenprofile.h"

class QListBox;
class QPushButton;
class QComboBox;
class QIMPenWidget;
class QIMPenEdit;
class QIMPenPrefBase;

class QIMPenSetup : public QDialog
{
    Q_OBJECT
public:
    QIMPenSetup( QIMPenProfile *p, QWidget *parent=0,
		const char *name=0, bool modal=FALSE, int WFlags=0 );

    QIMPenEdit *editor() { return edit; }

protected:
    void loadProfiles();
    virtual void accept();

private slots:
    void styleClicked( int );
    void multiTimeoutChanged( int );
    void selectProfile( const QString &p );

private:
    QComboBox *profileCombo;
    QIMPenEdit *edit;
    QIMPenPrefBase *pref;
    int style;
    int multiTimeout;
    QIMPenProfile *profile;
    QList<QIMPenProfile> profileList;
};

class QIMPenInputCharDlg : public QDialog
{
    Q_OBJECT
public:
    QIMPenInputCharDlg( QWidget *parent = 0, const char *name = 0,
	    bool modal = FALSE, int WFlags = 0 );

    unsigned int unicode() const { return uni; }

protected:
    void addSpecial( QComboBox *cb );

protected slots:
    void setSpecial( int sp );
    void setCharacter( const QString &string );

protected:
    uint uni;
};

class QIMPenEdit : public QWidget
{
    Q_OBJECT
public:
    QIMPenEdit( QIMPenProfile *p, QWidget *parent=0,
		const char *name=0 );

    void setProfile( QIMPenProfile *p );
    void selectCharSet( QIMPenCharSet *c );

protected:
    void fillCharList();
    void enableButtons();
    QIMPenChar *findPrev();
    QIMPenChar *findNext();
    void setCurrentChar( QIMPenChar * );

protected slots:
    void prevChar();
    void nextChar();
    void clearChar();
    void selectChar( int );
    void selectCharSet( int );
    void addChar();
    void addNewChar();
    void removeChar();
    void defaultChars();
    void newStroke( QIMPenStroke * );

protected:
    QIMPenWidget *pw;
    QComboBox *charSetCombo;
    QListBox *charList;
    QPushButton *newBtn;
    QPushButton *addBtn;
    QPushButton *removeBtn;
    QPushButton *prevBtn;
    QPushButton *nextBtn;
    uint currentCode;
    QIMPenChar *currentChar;
    QIMPenChar *inputChar;
    QIMPenCharSet *currentSet;
    QIMPenProfile *profile;
};

