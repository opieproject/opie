/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Excel.h"
#include "sheet.h"

/* OPIE */
#include <qpe/applnk.h>
#include <qpe/fileselector.h>

/* QT */
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qlineedit.h>
#include <qbutton.h>
#include <qcombobox.h>
#include <qtoolbutton.h>

typedef struct typeSheet
{
    QString name;
    QList<typeCellData> data;
};

class MainWindow: public QMainWindow
{
    Q_OBJECT

    // QPE objects
    DocLnk* currentDoc;
    QMenuBar *menu;
    QToolBar *toolbarFunctions, *toolbarEdit, *toolbarStandard;
    FileSelector *fileSelector;
    FileSelector *ExcelSelector;

    // QT objects
    QPopupMenu *menuFile, *menuEdit, *menuInsert, *menuFormat, *menuData, *menuHelp,
    *submenuFunc, *submenuFuncStd, *submenuFuncStandard, *submenuFuncLogic,
    *submenuFuncTrig, *submenuFuncString, *submenuFuncScientific, *submenuFuncDistr,
    *submenuFuncStat,
    *submenuRow, *submenuCol, *submenuSheet;
    QAction *fileNew, *fileOpen, *fileSave, *fileSaveAs, *fileExcelImport, *fileQuit, *helpAbout, *editAccept, *editCancel, *formatCells,
    *funcPlus, *funcMinus, *funcCross, *funcDivide, *funcParanOpen, *funcParanClose, *funcComma, *funcEqual,
    *editCut, *editCopy, *editPaste, *editPasteContents, *editClear, *insertCols, *insertRows, *insertSheets, *insertCells,
    *rowHeight, *rowShow, *rowHide, *rowAdjust, *colWidth, *colShow, *colHide, *colAdjust, *sheetRename, *sheetRemove,
    *dataSort, *dataFindReplace, *editCellSelect, *helpGeneral;
    QLineEdit *editData;
    QButton *buttonUp, *buttonDown, *buttonLeft, *buttonRight;
    QComboBox *comboSheets;
    QToolButton *toolFunction;
    QList<typeSheet> listSheets;
    QString helpFile;

    // Other objects
    Sheet *sheet;

    // Variables
    bool documentModified;

    // Private functions
    void initMenu();
    void initActions();
    void initFunctionsToolbar();
    void initEditToolbar();
    void initStandardToolbar();
    void initSheet();
    void addToData(const QString &data);
    int saveCurrentFile(bool ask=TRUE);
    void documentOpen(const DocLnk &lnkDoc);
    void documentSave(DocLnk *lnkDoc);
    void closeEvent(QCloseEvent *e);
    void addFlyAction(const QString &text, const QString &menuText, const QString &tip, QWidget *w);
    typeSheet *createNewSheet();
    typeSheet *findSheet(const QString &name);

private slots:
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();

    void slotImportExcel(const DocLnk &lnkDoc);
    void slotImportExcelOpen();
    void ExcelSelectorHide();

    void slotHelpAbout();
    void slotEditAccept();
    void slotEditCancel();
    void slotEditPaste();
    void slotEditPasteContents();
    void slotFormatCells();
    void slotInsertCells();
    void slotInsertRows();
    void slotInsertCols();
    void slotInsertSheets();
    void slotDataSort();
    void slotDataFindReplace();
    void slotRowHeight();
    void slotRowAdjust();
    void slotRowShow();
    void slotRowHide();
    void slotColumnWidth();
    void slotColumnAdjust();
    void slotColumnShow();
    void slotColumnHide();
    void slotSheetRename();
    void slotSheetRemove();
    void slotFuncOutput();
    void slotCellSelect(bool lock);
    void slotCellClicked(const QString &cell);
    void slotSheetChanged(const QString &name);
    void slotDocModified();
    void selectorShow();
    void selectorHide();
    void selectorFileNew(const DocLnk &lnkDoc);
    void selectorFileOpen(const DocLnk &lnkDoc);

public:
static QString appName() { return QString::fromLatin1("sheetqt"); }
    MainWindow(QWidget *p, const char*, WFlags);
    ~MainWindow();

    void setHelpFile(const QString &help_filename)          { helpFile=help_filename; }

public slots:
    void setDocument(const QString &applnk_filename);
};

#endif
