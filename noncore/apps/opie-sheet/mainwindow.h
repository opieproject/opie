/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qpe/applnk.h>
#include <qpe/fileselector.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qlineedit.h>
#include <qbutton.h>
#include <qcombobox.h>
#include <qtoolbutton.h>

#include "sheet.h"

typedef struct typeSheet
{
  QString name;
  QList<typeCellData> data;
};

class MainWindow: public QMainWindow
{
  Q_OBJECT

  // QPE objects
  DocLnk currentDoc;
  QPEMenuBar *menu;
  QPEToolBar *toolbarFunctions, *toolbarEdit, *toolbarStandard;
  FileSelector *fileSelector;

  // QT objects
  QPopupMenu *menuFile, *menuEdit, *menuInsert, *menuFormat, *menuData, *menuHelp,
             *submenuFunc, *submenuFuncStd, *submenuFuncMath, *submenuFuncStat,
             *submenuRow, *submenuCol, *submenuSheet;
  QAction *fileNew, *fileOpen, *fileSave, *fileSaveAs, *fileQuit, *helpAbout, *editAccept, *editCancel, *formatCells,
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
  void copyDocLnk(const DocLnk &source, DocLnk &target);
  void documentSave(DocLnk &lnkDoc);
  void closeEvent(QCloseEvent *e);
  void addFlyAction(const QString &text, const QString &menuText, const QString &tip, QWidget *w);
  typeSheet *createNewSheet();
  typeSheet *findSheet(const QString &name);

  private slots:
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();
    void slotHelpAbout();
    void slotHelpGeneral();
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
    MainWindow();
    ~MainWindow();

    void setHelpFile(const QString &help_filename)          { helpFile=help_filename; }

  public slots:
    void setDocument(const QString &applnk_filename);
};

#endif
