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

#ifndef _QIMPENINPUT_H_
#define _QIMPENINPUT_H_

#include "qimpenprofile.h"

#include <qpe/qdawg.h>

#include <qframe.h>
#include <qlist.h>
#include <qguardedptr.h>

class QPushButton;
class QTimer;
class QIMPenWidget;
class QIMPenSetup;
class QIMPenWordPick;
class QIMPenMatch;
class HandwritingHelp;

class QIMPenInput : public QFrame
{
    Q_OBJECT
public:
    QIMPenInput( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    virtual ~QIMPenInput();

    void resetState();

    QSize sizeHint() const;

signals:
    void key( ushort, ushort, ushort, bool, bool );
    
private slots:
    void wordPicked( const QString & );
    void selectCharSet( int );
    void beginStroke();
    void strokeEntered( QIMPenStroke *st );
    void matchedCharacters( const QIMPenCharMatchList &cl );
    void keypress( uint scan_uni );
    void erase();
    void help();
    void setup();
    void backspace();
    void enter();

private:
    void loadProfiles();
    void selectProfile( const QString &name );
    void handleExtended( const QString & );
    void updateWordMatch( QIMPenCharMatchList &ml );
    void matchWords();
    void scanDict( const QDawg::Node* n, int ipos, const QString& str, int error );

    enum Mode { Normal, Switch, SwitchLock };

private:
    Mode mode;
    QRect prefRect;
    QIMPenWidget *pw;
    QPushButton *helpBtn;
    QPushButton *setupBtn;
    QIMPenSetup *setupDlg;
    QIMPenMatch *matcher;
    QGuardedPtr<HandwritingHelp> helpDlg;
    QIMPenProfile *profile;
    QList<QIMPenProfile> profileList;
    QIMPenCharSet *shortcutCharSet;
    QIMPenCharSetList baseSets;
    int currCharSet;
    QIMPenWordPick *wordPicker;
};

#endif // _QIMPENINPUT_H_
