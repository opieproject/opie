/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qwidgetstack.h>
#include <qmap.h>
#include <qpe/filemanager.h>

class QAction;
class QComboBox;
class FileSelectorView;
class FileSelector;
class QToolBar;
class ButtonMenu;

namespace Qt3 {

class QTextEdit;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget *parent = 0, const char *name = 0 );
    ~MainWindow();

protected:
    void closeEvent( QCloseEvent *e );

private slots:
    // new file functions
    void fileOpen();
    void fileRevert();
    void fileNew();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily( const QString &f );
    void textSize( const QString &p );
    void textStyle( int s );
    void textAlign( int );

    void fontChanged( const QFont &f );
    void colorChanged( const QColor &c );
    void alignmentChanged( int a );
    void editorChanged( QWidget * );

    // these are from textedit, we may need them
    void insertTable();
    void newFile( const DocLnk & );
    void openFile( const DocLnk & );
    void showEditTools();
    void hideEditTools();

private:
    void updateFontSizeCombo( const QFont &f );
    void setupActions();
    Qt3::QTextEdit *currentEditor() const;
    void doConnections( Qt3::QTextEdit *e );
    void updateCaption();
    void save();
    void clear();

    // added these from the textedit
    QWidgetStack *editorStack;
    FileSelector *fileSelector;
    QToolBar *tbMenu;
    QToolBar *tbEdit;
    QToolBar *tbFont;
    QToolBar *tbStyle;
    QAction *actionTextBold,
 	*actionTextUnderline,
 	*actionTextItalic;
    QComboBox *comboFont,
	*comboSize;
    ButtonMenu *alignMenu;
    DocLnk *doc;
    Qt3::QTextEdit* editor;
};


#endif
