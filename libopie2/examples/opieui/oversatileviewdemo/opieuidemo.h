/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef OPIEUIDEMO_H
#define OPIEUIDEMO_H

#include <qmainwindow.h>

class OVersatileView;
class QTabWidget;
class QVBox;

class OpieUIDemo : public QMainWindow {
    Q_OBJECT

public:

    OpieUIDemo( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~OpieUIDemo();

    void demoOCompletionBox();
    void demoOLineEdit();
    void demoOComboBox();
    void demoOEditListBox();
    void demoOSelector();
    
public slots:
    void demo( int );
    void messageBox( const QString& text );

protected:
    void build();
    void buildVV( QVBox* b );
    
private:
    QTabWidget* main;
    
    OVersatileView* vv;

};



#endif
