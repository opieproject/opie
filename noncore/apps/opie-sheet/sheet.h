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

#ifndef SHEET_H
#define SHEET_H

#include <qtable.h>
#include <qstack.h>

typedef struct typeCellBorders
{
  QPen right, bottom;
};

typedef struct typeCellData
{
  int col, row;
  typeCellBorders borders;
  QBrush background;
  Qt::AlignmentFlags alignment;
  QColor fontColor;
  QFont font;
  QString data;
};

class Sheet: public QTable
{
  Q_OBJECT

  // Variables
  bool clicksLocked;
  int selectionNo;
  typeCellBorders defaultBorders;
  typeCellData defaultCellData;

  // QT objects
  QList<typeCellData> sheetData, clipboardData;
  QString pressedCell, releasedCell, sheetName;
  QStringList listDataParser;

  // Private functions
  int getOperatorPriority(char oper);
  bool findRowColumn(const QString &variable, int *row, int *col, bool giveError=FALSE);
  QString findCellName(int row, int col);
  bool findRange(const QString &variable1, const QString &variable2, int *row1, int *col1, int *row2, int *col2);
  double calculateVariable(const QString &variable);
  double calculateFunction(const QString &function, const QString &parameters);
  QChar popCharStack(QStack<QChar> *stackChars);
  QString popStringStack(QStack<QString> *stackStrings);
  QString getParameter(const QString &parameters, int paramNo, bool giveError=FALSE, const QString funcName="");
  QString dataParser(const QString &cell, const QString &data);
  QString dataParserHelper(const QString &data);
  typeCellData *createCellData(int row, int col);
  typeCellData *findCellData(int row, int col);
  void pushCharStack(QStack<QChar> *stackChars, const QChar &character);
  void pushStringStack(QStack<QString> *stackStrings, const QString &string);

  // Sheet/Qt parser functions
  double functionSum(const QString &param1, const QString &param2);
  double functionAvg(const QString &param1, const QString &param2);
  double functionMax(const QString &param1, const QString &param2);
  double functionMin(const QString &param1, const QString &param2);
  double functionCount(const QString &param1, const QString &param2);

  // Reimplemented QTable functions
  void paintCell(QPainter *p, int row, int col, const QRect & cr, bool selected);
  void viewportMousePressEvent(QMouseEvent *e);
  void viewportMouseMoveEvent(QMouseEvent *e);
  void viewportMouseReleaseEvent(QMouseEvent *e);

  private slots:
    void slotCellSelected(int row, int col);
    void slotCellChanged(int row, int col);

  public:
    Sheet(int numRows, int numCols, QWidget *parent);
    ~Sheet();

    void setData(const QString &data);
    QString getData();

    void setName(const QString &data);
    QString getName();

    void setPen(int row, int col, int vertical, const QPen &pen);
    QPen getPen(int row, int col, int vertical);

    void setBrush(int row, int col, const QBrush &brush);
    QBrush getBrush(int row, int col);

    void setTextAlign(int row, int col, Qt::AlignmentFlags flags);
    Qt::AlignmentFlags getAlignment(int row, int col);

    void setTextFont(int row, int col, const QFont &font, const QColor &color);
    QFont getFont(int row, int col);
    QColor getFontColor(int row, int col);

    void lockClicks(bool lock=TRUE);
    void copySheetData(QList<typeCellData> *destSheetData);
    void setSheetData(QList<typeCellData> *srcSheetData);
    void getSelection(int *row1, int *col1, int *row2, int *col2);

    void insertRows(int no=1, bool allColumns=TRUE);
    void insertColumns(int no=1, bool allRows=TRUE);

    void dataFindReplace(const QString &find, const QString &replace, bool matchCase=TRUE, bool allCells=TRUE, bool entireCell=FALSE, bool replace=FALSE, bool replaceAll=FALSE);

    // Static functions
    static int getHeaderColumn(const QString &section);
    static QString getHeaderString(int section);

  public slots:
    void editCut();
    void editCopy();
    void editPaste(bool onlyContents=FALSE);
    void editClear();
    void swapCells(int row1, int col1, int row2, int col2);

  signals:
    void currentDataChanged(const QString &data);
    void cellClicked(const QString &cell);
    void sheetModified();
};

#endif
