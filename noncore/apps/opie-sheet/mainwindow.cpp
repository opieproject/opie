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

#include "mainwindow.h"

#include <qpe/filemanager.h>
#include <qpe/qcopenvelope_qws.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qradiobutton.h>
#include "cellformat.h"
#include "numberdlg.h"
#include "textdlg.h"
#include "sortdlg.h"
#include "finddlg.h"

#include "file-new.xpm"
#include "file-open.xpm"
#include "file-save.xpm"
#include "edit-cancel.xpm"
#include "edit-accept.xpm"
#include "help-general.xpm"
#include "func-plus.xpm"
#include "func-minus.xpm"
#include "func-cross.xpm"
#include "func-divide.xpm"
#include "func-paran-open.xpm"
#include "func-paran-close.xpm"
#include "func-comma.xpm"
#include "func-func.xpm"
#include "func-equal.xpm"
#include "cell-select.xpm"

#define DEFAULT_NUM_ROWS 199
#define DEFAULT_NUM_COLS (26*3)
#define DEFAULT_NUM_SHEETS 3

MainWindow::MainWindow()
           :QMainWindow()
{
  // initialize variables
  documentModified=FALSE;

  // construct objects
  currentDoc=0;
  fileSelector=new FileSelector("application/sheet-qt", this, QString::null);
  connect(fileSelector, SIGNAL(closeMe()), this, SLOT(selectorHide()));
  connect(fileSelector, SIGNAL(newSelected(const DocLnk &)), this, SLOT(selectorFileNew(const DocLnk &)));
  connect(fileSelector, SIGNAL(fileSelected(const DocLnk &)), this, SLOT(selectorFileOpen(const DocLnk &)));

  listSheets.setAutoDelete(TRUE);

  initActions();
  initMenu();
  initEditToolbar();
  initFunctionsToolbar();
  initStandardToolbar();
  initSheet();

  // set window title
  setCaption(tr("Opie Sheet"));

  // create sheets
  selectorFileNew(DocLnk());
}

MainWindow::~MainWindow()
{
  if (currentDoc) delete currentDoc;
}

void MainWindow::documentSave(DocLnk *lnkDoc)
{
  FileManager fm;
  QByteArray streamBuffer;
  QDataStream stream(streamBuffer, IO_WriteOnly);

  typeSheet *currentSheet=findSheet(sheet->getName());
  if (!currentSheet)
  {
    QMessageBox::critical(this, tr("Error"), tr("Inconsistency error!"));
    return;
  }
  sheet->copySheetData(&currentSheet->data);
  stream.writeRawBytes("SQT100", 6);
  stream << (Q_UINT32)listSheets.count();
  for (typeSheet *tempSheet=listSheets.first(); tempSheet; tempSheet=listSheets.next())
  {
    stream << tempSheet->name << (Q_UINT32)tempSheet->data.count();
    for (typeCellData *tempCell=tempSheet->data.first(); tempCell; tempCell=tempSheet->data.next())
      stream << (Q_UINT32)tempCell->col << (Q_UINT32)tempCell->row << tempCell->borders.right << tempCell->borders.bottom << tempCell->background << (Q_UINT32)tempCell->alignment << tempCell->fontColor << tempCell->font << tempCell->data;
  }

  lnkDoc->setType("application/sheet-qt");
  if (!fm.saveFile(*lnkDoc, streamBuffer))
  {
    QMessageBox::critical(this, tr("Error"), tr("File cannot be saved!"));
    return;
  }
  documentModified=FALSE;
}

void MainWindow::documentOpen(const DocLnk &lnkDoc)
{
  FileManager fm;
  QByteArray streamBuffer;
  if (!lnkDoc.isValid() || !fm.loadFile(lnkDoc, streamBuffer))
  {
    QMessageBox::critical(this, tr("Error"), tr("File cannot be opened!"));
    documentModified=FALSE;
    selectorFileNew(DocLnk());
    return;
  }
  QDataStream stream(streamBuffer, IO_ReadOnly);

  Q_UINT32 countSheet, countCell, i, j, row, col, alignment;
  typeSheet *newSheet;
  typeCellData *newCell;

  char fileFormat[7];
  stream.readRawBytes(fileFormat, 6);
  fileFormat[6]=0;
  if ((QString)fileFormat!="SQT100")
  {
    QMessageBox::critical(this, tr("Error"), tr("Invalid file format!"));
    documentModified=FALSE;
    selectorFileNew(DocLnk());
    return;
  }

  stream >> countSheet;
  for (i=0; i<countSheet; ++i)
  {
    newSheet=new typeSheet;
    newSheet->data.setAutoDelete(TRUE);
    stream >> newSheet->name >> countCell;
    comboSheets->insertItem(newSheet->name);

    for (j=0; j<countCell; ++j)
    {
      newCell=new typeCellData;
      stream >> col >> row >> newCell->borders.right >> newCell->borders.bottom >> newCell->background >> alignment >> newCell->fontColor >> newCell->font >> newCell->data;
      newCell->col=col;
      newCell->row=row;
      newCell->alignment=(Qt::AlignmentFlags)alignment;
      newSheet->data.append(newCell);
    }
    listSheets.append(newSheet);

    if (i==0)
    {
      sheet->setName(newSheet->name);
      sheet->setSheetData(&newSheet->data);
    }
  }
}

int MainWindow::saveCurrentFile(bool ask)
{
  if (ask)
  {
    int result=QMessageBox::information(this, tr("Save File"), tr("Do you want to save the current file?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    if (result!=QMessageBox::Yes) return result;
  }

  if (!currentDoc->isValid())
  {
    TextDialog dialogText(this);
    if (dialogText.exec(tr("Save File"), tr("&File Name:"), tr("UnnamedFile"))!=QDialog::Accepted || dialogText.getValue().isEmpty()) return QMessageBox::Cancel;

    currentDoc->setName(dialogText.getValue());
    currentDoc->setFile(QString::null);
    currentDoc->setLinkFile(QString::null);
  }

  documentSave(currentDoc);
  return QMessageBox::Yes;
}

void MainWindow::selectorFileNew(const DocLnk &lnkDoc)
{
  selectorHide();

  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) return;
  if (currentDoc) delete currentDoc;
  currentDoc = new DocLnk(lnkDoc);
  listSheets.clear();
  comboSheets->clear();

  typeSheet *newSheet=createNewSheet();
  newSheet->data.setAutoDelete(TRUE);
  sheet->setName(newSheet->name);
  sheet->setSheetData(&newSheet->data);
  for (int i=1; i<DEFAULT_NUM_SHEETS; ++i)
    createNewSheet();
  documentModified=FALSE;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) e->ignore();
  else e->accept();
}

void MainWindow::selectorFileOpen(const DocLnk &lnkDoc)
{
  selectorHide();

  if (documentModified && saveCurrentFile()==QMessageBox::Cancel) return;
  if (currentDoc) delete currentDoc;
  currentDoc = new DocLnk(lnkDoc);
  listSheets.clear();
  comboSheets->clear();

  documentOpen(lnkDoc);
  documentModified=FALSE;
}

void MainWindow::selectorShow()
{
  sheet->hide();
  setCentralWidget(fileSelector);
  fileSelector->show();
  fileSelector->reread();
}

void MainWindow::selectorHide()
{
  fileSelector->hide();
  setCentralWidget(sheet);
  sheet->show();
}

void MainWindow::slotFileNew()
{
  selectorFileNew(DocLnk());
}

void MainWindow::slotFileOpen()
{
  selectorShow();
}

void MainWindow::slotFileSave()
{
  saveCurrentFile(FALSE);
}

void MainWindow::setDocument(const QString &applnk_filename)
{
  selectorFileOpen(DocLnk(applnk_filename));
}

void MainWindow::initActions()
{
  fileNew=new QAction(tr("New File"), QPixmap(file_new_xpm), tr("&New"), 0, this);
  connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));
  fileOpen=new QAction(tr("Open File"), QPixmap(file_open_xpm), tr("&Open"), 0, this);
  connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));
  fileSave=new QAction(tr("Save File"), QPixmap(file_save_xpm), tr("&Save"), 0, this);
  connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));
  fileSaveAs=new QAction(tr("Save File As"), QPixmap(file_save_xpm), tr("Save &As"), 0, this);
  connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSaveAs()));
  fileQuit=new QAction(tr("Quit"), tr("&Quit"), 0, this);
  connect(fileQuit, SIGNAL(activated()), this, SLOT(close()));

  helpGeneral=new QAction(tr("General Help"), QPixmap(help_general_xpm), tr("&General"), 0, this);
  connect(helpGeneral, SIGNAL(activated()), this, SLOT(slotHelpGeneral()));
  helpAbout=new QAction(tr("About Opie Sheet"), tr("&About"), 0, this);
  connect(helpAbout, SIGNAL(activated()), this, SLOT(slotHelpAbout()));

  editAccept=new QAction(tr("Accept"), QPixmap(edit_accept_xpm), tr("&Accept"), 0, this);
  connect(editAccept, SIGNAL(activated()), this, SLOT(slotEditAccept()));
  editCancel=new QAction(tr("Cancel"), QPixmap(edit_cancel_xpm), tr("&Cancel"), 0, this);
  connect(editCancel, SIGNAL(activated()), this, SLOT(slotEditCancel()));
  editCellSelect=new QAction(tr("Cell Selector"), QPixmap(cell_select_xpm), tr("Cell &Selector"), 0, this);
  editCellSelect->setToggleAction(TRUE);
  connect(editCellSelect, SIGNAL(toggled(bool)), this, SLOT(slotCellSelect(bool)));
  editCut=new QAction(tr("Cut Cells"), tr("Cu&t"), 0, this);
  editCopy=new QAction(tr("Copy Cells"), tr("&Copy"), 0, this);
  editPaste=new QAction(tr("Paste Cells"), tr("&Paste"), 0, this);
  connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));
  editPasteContents=new QAction(tr("Paste Contents"), tr("Paste Cont&ents"), 0, this);
  connect(editPasteContents, SIGNAL(activated()), this, SLOT(slotEditPasteContents()));
  editClear=new QAction(tr("Clear Cells"), tr("C&lear"), 0, this);

  insertCells=new QAction(tr("Insert Cells"), tr("C&ells"), 0, this);
  connect(insertCells, SIGNAL(activated()), this, SLOT(slotInsertCells()));
  insertRows=new QAction(tr("Insert Rows"), tr("&Rows"), 0, this);
  connect(insertRows, SIGNAL(activated()), this, SLOT(slotInsertRows()));
  insertCols=new QAction(tr("Insert Columns"), tr("&Columns"), 0, this);
  connect(insertCols, SIGNAL(activated()), this, SLOT(slotInsertCols()));
  insertSheets=new QAction(tr("Add Sheets"), tr("&Sheets"), 0, this);
  connect(insertSheets, SIGNAL(activated()), this, SLOT(slotInsertSheets()));

  formatCells=new QAction(tr("Cells"), tr("&Cells"), 0, this);
  connect(formatCells, SIGNAL(activated()), this, SLOT(slotFormatCells()));

  rowHeight=new QAction(tr("Row Height"), tr("H&eight"), 0, this);
  connect(rowHeight, SIGNAL(activated()), this, SLOT(slotRowHeight()));
  rowAdjust=new QAction(tr("Adjust Row"), tr("&Adjust"), 0, this);
  connect(rowAdjust, SIGNAL(activated()), this, SLOT(slotRowAdjust()));
  rowShow=new QAction(tr("Show Row"), tr("&Show"), 0, this);
  connect(rowShow, SIGNAL(activated()), this, SLOT(slotRowShow()));
  rowHide=new QAction(tr("Hide Row"), tr("&Hide"), 0, this);
  connect(rowHide, SIGNAL(activated()), this, SLOT(slotRowHide()));

  colWidth=new QAction(tr("Column Width"), tr("&Width"), 0, this);
  connect(colWidth, SIGNAL(activated()), this, SLOT(slotColumnWidth()));
  colAdjust=new QAction(tr("Adjust Column"), tr("&Adjust"), 0, this);
  connect(colAdjust, SIGNAL(activated()), this, SLOT(slotColumnAdjust()));
  colShow=new QAction(tr("Show Column"), tr("&Show"), 0, this);
  connect(colShow, SIGNAL(activated()), this, SLOT(slotColumnShow()));
  colHide=new QAction(tr("Hide Column"), tr("&Hide"), 0, this);
  connect(colHide, SIGNAL(activated()), this, SLOT(slotColumnHide()));

  sheetRename=new QAction(tr("Rename Sheet"), tr("&Rename"), 0, this);
  connect(sheetRename, SIGNAL(activated()), this, SLOT(slotSheetRename()));
  sheetRemove=new QAction(tr("Remove Sheet"), tr("R&emove"), 0, this);
  connect(sheetRemove, SIGNAL(activated()), this, SLOT(slotSheetRemove()));

  dataSort=new QAction(tr("Sort Data"), tr("&Sort"), 0, this);
  connect(dataSort, SIGNAL(activated()), this, SLOT(slotDataSort()));
  dataFindReplace=new QAction(tr("Find && Replace"), tr("&Find && Replace"), 0, this);
  connect(dataFindReplace, SIGNAL(activated()), this, SLOT(slotDataFindReplace()));

  funcEqual=new QAction(tr("Equal To"), QPixmap(func_equal_xpm), tr("&Equal To"), 0, this);
  funcEqual->setToolTip("=");
  connect(funcEqual, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcPlus=new QAction(tr("Addition"), QPixmap(func_plus_xpm), tr("&Addition"), 0, this);
  funcPlus->setToolTip("+");
  connect(funcPlus, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcMinus=new QAction(tr("Subtraction"), QPixmap(func_minus_xpm), tr("&Subtraction"), 0, this);
  funcMinus->setToolTip("-");
  connect(funcMinus, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcCross=new QAction(tr("Multiplication"), QPixmap(func_cross_xpm), tr("&Multiplication"), 0, this);
  funcCross->setToolTip("*");
  connect(funcCross, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcDivide=new QAction(tr("Division"), QPixmap(func_divide_xpm), tr("&Division"), 0, this);
  funcDivide->setToolTip("/");
  connect(funcDivide, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcParanOpen=new QAction(tr("Open ParanthesistempCellData->row+row1, tempCellData->col+col1"), QPixmap(func_paran_open_xpm), tr("&Open Paranthesis"), 0, this);
  funcParanOpen->setToolTip("(");
  connect(funcParanOpen, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcParanClose=new QAction(tr("Close Paranthesis"), QPixmap(func_paran_close_xpm), tr("&Close Paranthesis"), 0, this);
  funcParanClose->setToolTip(")");
  connect(funcParanClose, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  funcComma=new QAction(tr("Comma"), QPixmap(func_comma_xpm), tr("&Comma"), 0, this);
  funcComma->setToolTip(",");
  connect(funcComma, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
}

void MainWindow::initMenu()
{
  menu=new QPEMenuBar(this);

  menuFile=new QPopupMenu;
  fileNew->addTo(menuFile);
  fileOpen->addTo(menuFile);
  fileSave->addTo(menuFile);
  fileSaveAs->addTo(menuFile);
  menuFile->insertSeparator();
  fileQuit->addTo(menuFile);
  menu->insertItem(tr("&File"), menuFile);

  menuEdit=new QPopupMenu;
  editAccept->addTo(menuEdit);
  editCancel->addTo(menuEdit);
  editCellSelect->addTo(menuEdit);
  menuEdit->insertSeparator();
  editCut->addTo(menuEdit);
  editCopy->addTo(menuEdit);
  editPaste->addTo(menuEdit);
  editPasteContents->addTo(menuEdit);
  editClear->addTo(menuEdit);
  menu->insertItem(tr("&Edit"), menuEdit);

  menuInsert=new QPopupMenu;
  menu->insertItem(tr("&Insert"), menuInsert);

  menuFormat=new QPopupMenu;
  formatCells->addTo(menuFormat);
  menu->insertItem(tr("&Format"), menuFormat);

  menuData=new QPopupMenu;
  dataSort->addTo(menuData);
  dataFindReplace->addTo(menuData);
  menu->insertItem(tr("&Data"), menuData);

  menuHelp=new QPopupMenu;
  helpGeneral->addTo(menuHelp);
  helpAbout->addTo(menuHelp);
  menu->insertItem(tr("&Help"), menuHelp);

  submenuRow=new QPopupMenu;
  rowHeight->addTo(submenuRow);
  rowAdjust->addTo(submenuRow);
  rowShow->addTo(submenuRow);
  rowHide->addTo(submenuRow);
  menuFormat->insertItem(tr("&Row"), submenuRow);

  submenuCol=new QPopupMenu;
  colWidth->addTo(submenuCol);
  colAdjust->addTo(submenuCol);
  colShow->addTo(submenuCol);
  colHide->addTo(submenuCol);
  menuFormat->insertItem(tr("Colum&n"), submenuCol);

  submenuSheet=new QPopupMenu;
  sheetRename->addTo(submenuSheet);
  sheetRemove->addTo(submenuSheet);
  menuFormat->insertItem(tr("&Sheet"), submenuSheet);

  submenuFunc=new QPopupMenu;
  menuInsert->insertItem(tr("&Function"), submenuFunc);

  submenuFuncStd=new QPopupMenu;
  funcPlus->addTo(submenuFuncStd);
  funcMinus->addTo(submenuFuncStd);
  funcCross->addTo(submenuFuncStd);
  funcDivide->addTo(submenuFuncStd);
  submenuFunc->insertItem(tr("&Standard"), submenuFuncStd);

  submenuFuncMath=new QPopupMenu;
  addFlyAction(tr("Summation"), tr("&Summation"), "SUM(", submenuFuncMath);
  addFlyAction(tr("Absolute Value"), tr("&Absolute"), "ABS(", submenuFuncMath);
  submenuFuncMath->insertSeparator();
  addFlyAction(tr("Sine"), tr("Si&ne"), "SIN(", submenuFuncMath);
  addFlyAction(tr("Arc Sine"), tr("A&rc Sine"), "ASIN(", submenuFuncMath);
  addFlyAction(tr("Cosine"), tr("&Cosine"), "COS(", submenuFuncMath);
  addFlyAction(tr("ArcCosine"), tr("Arc Cos&ine"), "COS(", submenuFuncMath);
  addFlyAction(tr("Tangent"), tr("&Tangent"), "TAN(", submenuFuncMath);
  addFlyAction(tr("Arc Tangent"), tr("Arc Tan&gent"), "ATAN(", submenuFuncMath);
  addFlyAction(tr("Arc Tangent of Coordinates"), tr("C&oor. Arc Tangent"), "ATAN2(", submenuFuncMath);
  submenuFuncMath->insertSeparator();
  addFlyAction(tr("Exponential"), tr("&Exponential"), "EXP(", submenuFuncMath);
  addFlyAction(tr("Logarithm"), tr("&Logarithm"), "LOG(", submenuFuncMath);
  addFlyAction(tr("Power"), tr("&Power"), "POW(", submenuFuncMath);
  submenuFunc->insertItem(tr("&Mathematical"), submenuFuncMath);

  submenuFuncStat=new QPopupMenu;
  addFlyAction(tr("Average"), tr("&Average"), "AVG(", submenuFuncStat);
  addFlyAction(tr("Maximum"), tr("Ma&ximum"), "MAX(", submenuFuncStat);
  addFlyAction(tr("Minimum"), tr("&Minimum"), "MIN(", submenuFuncStat);
  addFlyAction(tr("Count"), tr("&Count"), "COUNT(", submenuFuncStat);
  submenuFunc->insertItem(tr("&Statistical"), submenuFuncStat);

  menuInsert->insertSeparator();
  insertCells->addTo(menuInsert);
  insertRows->addTo(menuInsert);
  insertCols->addTo(menuInsert);
  insertSheets->addTo(menuInsert);
}

void MainWindow::initStandardToolbar()
{
  toolbarStandard=new QPEToolBar(this);
  toolbarStandard->setHorizontalStretchable(TRUE);
  moveToolBar(toolbarStandard, Top);

  fileNew->addTo(toolbarStandard);
  fileOpen->addTo(toolbarStandard);
  fileSave->addTo(toolbarStandard);

  comboSheets=new QComboBox(toolbarStandard);
  toolbarStandard->setStretchableWidget(comboSheets);
  connect(comboSheets, SIGNAL(activated(const QString &)), this, SLOT(slotSheetChanged(const QString &)));
}

void MainWindow::initFunctionsToolbar()
{
  toolbarFunctions=new QPEToolBar(this);
  toolbarFunctions->setHorizontalStretchable(TRUE);
  moveToolBar(toolbarFunctions, Bottom);

  funcEqual->addTo(toolbarFunctions);
  funcPlus->addTo(toolbarFunctions);
  funcMinus->addTo(toolbarFunctions);
  funcCross->addTo(toolbarFunctions);
  funcDivide->addTo(toolbarFunctions);
  funcParanOpen->addTo(toolbarFunctions);
  funcParanClose->addTo(toolbarFunctions);
  funcComma->addTo(toolbarFunctions);

  toolFunction=new QToolButton(toolbarFunctions);
  toolFunction->setPixmap(func_func_xpm);
  toolFunction->setTextLabel(tr("Functions"));
  toolFunction->setPopup(submenuFunc);
  toolFunction->setPopupDelay(0);
}

void MainWindow::initEditToolbar()
{
  toolbarEdit=new QPEToolBar(this);
  toolbarEdit->setHorizontalStretchable(TRUE);
  moveToolBar(toolbarEdit, Bottom);

  editAccept->addTo(toolbarEdit);
  editCancel->addTo(toolbarEdit);

  editData=new QLineEdit(toolbarEdit);
  toolbarEdit->setStretchableWidget(editData);
  connect(editData, SIGNAL(returnPressed()), this, SLOT(slotEditAccept()));

  editCellSelect->addTo(toolbarEdit);
}

void MainWindow::slotHelpGeneral()
{
  if (QFile::exists(helpFile))
  {
    QCopEnvelope e("QPE/Application/helpbrowser", "showFile(QString)");
    e << helpFile;
  }
  else
    QMessageBox::critical(this, tr("Error"), tr("Help file not found!"));
}

void MainWindow::slotHelpAbout()
{
  QDialog dialogAbout(this, 0, TRUE);
  dialogAbout.resize(width()-40, height()-80);
  dialogAbout.setCaption(tr("About Opie Sheet"));

  QLabel label(tr("Opie Sheet\nSpreadsheet Software for Opie\nQWDC Beta Winner (as Sheet/Qt)\n\nDeveloped by: Serdar Ozler\nRelease 1.0.1\nRelease Date: July 04, 2002\n\nThis product is licensed under GPL. It is freely distributable. If you want to get the latest version and also the source code, please visit the web site.\n\nhttp://qtopia.sitebest.com"), &dialogAbout);
  label.setGeometry(dialogAbout.rect());
  label.setAlignment(Qt::AlignCenter | Qt::WordBreak);

  dialogAbout.exec();
}

void MainWindow::initSheet()
{
  sheet=new Sheet(DEFAULT_NUM_ROWS, DEFAULT_NUM_COLS, this);
  setCentralWidget(sheet);

  connect(sheet, SIGNAL(currentDataChanged(const QString &)), editData, SLOT(setText(const QString &)));
  connect(sheet, SIGNAL(cellClicked(const QString &)), this, SLOT(slotCellClicked(const QString &)));
  connect(sheet, SIGNAL(sheetModified()), this, SLOT(slotDocModified()));

  connect(editCut, SIGNAL(activated()), sheet, SLOT(editCut()));
  connect(editCopy, SIGNAL(activated()), sheet, SLOT(editCopy()));
  connect(editClear, SIGNAL(activated()), sheet, SLOT(editClear()));
}

void MainWindow::slotEditAccept()
{
  sheet->setData(editData->text());
}

void MainWindow::slotEditCancel()
{
  editData->setText(sheet->getData());
}

void MainWindow::slotCellSelect(bool lock)
{
  sheet->lockClicks(lock);
}

void MainWindow::addToData(const QString &data)
{
  editData->setText(editData->text().insert(editData->cursorPosition(), data));
}

void MainWindow::slotFuncOutput()
{
  if (sender()->isA("QAction"))
    addToData(((QAction *)sender())->toolTip());
}

void MainWindow::slotInsertRows()
{
  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Insert Rows"), tr("&Number of rows:"))==QDialog::Accepted)
    sheet->insertRows(dialogNumber.getValue());
}

void MainWindow::slotInsertCols()
{
  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Insert Columns"), tr("&Number of columns:"))==QDialog::Accepted)
    sheet->insertColumns(dialogNumber.getValue());
}

void MainWindow::slotInsertSheets()
{
  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Add Sheets"), tr("&Number of sheets:"))==QDialog::Accepted)
    for (int i=dialogNumber.getValue(); i>0; --i) createNewSheet();
}

void MainWindow::slotCellClicked(const QString &cell)
{
  editCellSelect->setOn(FALSE);
  addToData(cell);
}

typeSheet *MainWindow::createNewSheet()
{
  typeSheet *newSheet=new typeSheet;
  int currentNo=1, tempNo;
  bool ok;

  for (typeSheet *tempSheet=listSheets.first(); tempSheet; tempSheet=listSheets.next())
    if (tempSheet->name.startsWith(tr("Sheet")) && (tempNo=tempSheet->name.mid(tr("Sheet").length()).toInt(&ok))>=currentNo && ok)
      currentNo=tempNo+1;

  newSheet->name=tr("Sheet")+QString::number(currentNo);
  newSheet->data.setAutoDelete(TRUE);

  comboSheets->insertItem(newSheet->name);
  listSheets.append(newSheet);
  return newSheet;
}

typeSheet *MainWindow::findSheet(const QString &name)
{
  for (typeSheet *tempSheet=listSheets.first(); tempSheet; tempSheet=listSheets.next())
    if (tempSheet->name==name)
      return tempSheet;
  return NULL;
}

void MainWindow::slotSheetChanged(const QString &name)
{
  sheet->copySheetData(&findSheet(sheet->getName())->data);
  sheet->setName(name);
  sheet->setSheetData(&findSheet(name)->data);
}

void MainWindow::addFlyAction(const QString &text, const QString &menuText, const QString &tip, QWidget *w)
{
  QAction *action=new QAction(text, menuText, 0, this);
  action->setToolTip(tip);
  connect(action, SIGNAL(activated()), this, SLOT(slotFuncOutput()));
  action->addTo(w);
}

void MainWindow::slotFormatCells()
{
  CellFormat dialogCellFormat(this);
  dialogCellFormat.showMaximized();
  dialogCellFormat.exec(sheet);
}

void MainWindow::slotEditPaste()
{
  sheet->editPaste();
}

void MainWindow::slotEditPasteContents()
{
  sheet->editPaste(TRUE);
}

void MainWindow::slotRowHeight()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Row Height"), tr("&Height of each row:"), sheet->rowHeight(row1))==QDialog::Accepted)
  {
    int newHeight=dialogNumber.getValue(), row;
    for (row=row1; row<=row2; ++row)
      sheet->setRowHeight(row, newHeight);
  }
}

void MainWindow::slotRowAdjust()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int row=row1; row<=row2; ++row)
    sheet->adjustRow(row);
}

void MainWindow::slotRowShow()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int row=row1; row<=row2; ++row)
    sheet->showRow(row);
}

void MainWindow::slotRowHide()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int row=row1; row<=row2; ++row)
    sheet->hideRow(row);
}

void MainWindow::slotColumnWidth()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  NumberDialog dialogNumber(this);
  if (dialogNumber.exec(tr("Column Width"), tr("&Width of each column:"), sheet->columnWidth(col1))==QDialog::Accepted)
  {
    int newWidth=dialogNumber.getValue(), col;
    for (col=col1; col<=col2; ++col)
      sheet->setColumnWidth(col, newWidth);
  }
}

void MainWindow::slotColumnAdjust()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int col=col1; col<=col2; ++col)
    sheet->adjustColumn(col);
}

void MainWindow::slotColumnShow()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int col=col1; col<=col2; ++col)
    sheet->showColumn(col);
}

void MainWindow::slotColumnHide()
{
  int row1, row2, col1, col2;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  for (int col=col1; col<=col2; ++col)
    sheet->hideColumn(col);
}

void MainWindow::slotFileSaveAs()
{
  TextDialog dialogText(this);
  if (dialogText.exec(tr("Save File As"), tr("&File Name:"), currentDoc->name())!=QDialog::Accepted || dialogText.getValue().isEmpty()) return;

  currentDoc->setName(dialogText.getValue());
  currentDoc->setFile(QString::null);
  currentDoc->setLinkFile(QString::null);
  documentSave(currentDoc);
}

void MainWindow::slotSheetRename()
{
  TextDialog dialogText(this);
  if (dialogText.exec(tr("Rename Sheet"), tr("&Sheet Name:"), sheet->getName())!=QDialog::Accepted || dialogText.getValue().isEmpty()) return;
  QString newName=dialogText.getValue();

  typeSheet *tempSheet=findSheet(newName);
  if (tempSheet)
  {
    QMessageBox::critical(this, tr("Error"), tr("There is already a sheet named '"+newName+'\''));
    return;
  }

  tempSheet=findSheet(sheet->getName());
  for (int i=0; i<comboSheets->count(); ++i)
    if (comboSheets->text(i)==tempSheet->name)
    {
      comboSheets->changeItem(newName, i);
      break;
    }
  tempSheet->name=newName;
  sheet->setName(newName);
}

void MainWindow::slotSheetRemove()
{
  if (comboSheets->count()<2)
  {
    QMessageBox::warning(this, tr("Error"), tr("There is only one sheet!"));
    return;
  }
  if (QMessageBox::information(this, tr("Remove Sheet"), tr("Are you sure?"), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
  {
    typeSheet *tempSheet=findSheet(sheet->getName());
    for (int i=0; i<comboSheets->count(); ++i)
      if (comboSheets->text(i)==tempSheet->name)
      {
        comboSheets->removeItem(i);
        break;
      }
    comboSheets->setCurrentItem(0);
    slotSheetChanged(comboSheets->currentText());
    listSheets.remove(tempSheet);
  }
}

void MainWindow::slotDataSort()
{
  SortDialog dialogSort(this);
  dialogSort.showMaximized();
  dialogSort.exec(sheet);
}

void MainWindow::slotDocModified()
{
  documentModified=TRUE;
}

void MainWindow::slotInsertCells()
{
  QDialog dialogInsert(this, 0, TRUE);
  dialogInsert.resize(180, 130);
  dialogInsert.setCaption(tr("Insert Cells"));

  QVButtonGroup *group=new QVButtonGroup(tr("&Type"), &dialogInsert);
  group->setGeometry(10, 10, 160, 110);
  QRadioButton *radio=new QRadioButton(tr("Shift cells &down"), group);
  radio=new QRadioButton(tr("Shift cells &right"), group);
  radio=new QRadioButton(tr("Entire ro&w"), group);
  radio=new QRadioButton(tr("Entire &column"), group);
  group->setButton(0);

  if (dialogInsert.exec()==QDialog::Accepted)
    switch (group->id(group->selected()))
    {
      case 0: sheet->insertRows(1, FALSE); break;
      case 1: sheet->insertColumns(1, FALSE); break;
      case 2: sheet->insertRows(1, TRUE); break;
      case 3: sheet->insertColumns(1, TRUE); break;
    }
}

void MainWindow::slotDataFindReplace()
{
  FindDialog dialogFind(this);
  dialogFind.showMaximized();
  dialogFind.exec(sheet);
}
