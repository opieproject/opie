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


#define NONE_TOKEN 0
#define NUMBER_TOKEN 1
#define VARIABLE_TOKEN 2
#define FUNCTION_TOKEN 3
#define SYMBOL_TOKEN 4
#define STRING_TOKEN 5

class Expression
{
public:
	QString Body;
	QList<QString> CompiledBody;
	QList<int> CompiledBodyType;
	QString SYMBOL;
	QString MATHSYMBOL;
	QArray<int> ArgsOfFunc;
	int FuncDepth;
	bool ErrorFound;
	int n; // holds the current parser position
	QString chunk; // the piece in the parser we are on
	int SymbGroup; // the current type

	QString InExpr;

	QChar chunk0(void); // retunrs the first char of expression;
	Expression(QString expr1);// constructor

	bool isSymbol(QChar ch);
	bool isMathSymbol(QChar ch);
	void GetNext();
	void First();
	void Third();
	void Fourth();
	void Fifth();
	void Sixth();
	void Seventh();
	void Eighth();
	void Ninth();

	bool Expression::Parse(); //parses the expression in RPN format;

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
  bool findRowColumn(const QString &variable, int *row, int *col, bool giveError=FALSE);
  QString findCellName(int row, int col);
  bool findRange(const QString &variable1, const QString &variable2, int *row1, int *col1, int *row2, int *col2);
  QString calculateVariable(const QString &variable);
  QString calculateFunction(const QString &func, const QString &parameters, int NumOfParams);
  QString getParameter(const QString &parameters, int paramNo, bool giveError=FALSE, const QString funcName="");
  QString dataParser(const QString &cell, const QString &data);
  QString dataParserHelper(const QString &data);
  typeCellData *createCellData(int row, int col);
  typeCellData *findCellData(int row, int col);


//LOGICAL / INFO
  double functionCountIf(const QString &param1, const QString &param2, const QString &param3);
  double functionSumSQ(const QString &param1, const QString &param2); //sum of squares
  QString functionIndex(const QString &param1, const QString &param2, int indx);
//math functions computations
  double BesselI0(double x);
  double BesselI(int n, double x);
  double BesselK0(double x);
  double BesselI1(double x);
  double BesselK1(double x);
  double BesselK(int n, double x);
  double BesselJ0(double x);
  double BesselY0(double x);
  double BesselJ1(double x);
  double BesselY1(double x);
  double BesselY(int n, double x);
  double BesselJ(int n, double x);
  double GammaLn(double xx);
  double Factorial(double n);
  double GammaP(double a, double x);
  double GammaQ(double a,double x);
  void GammaSeries(double *gamser, double a, double x, double *gln);
  void GammaContinuedFraction(double *gammcf, double a, double x, double *gln);
  double ErrorFunction(double x);
  double ErrorFunctionComplementary(double x);
  double Beta(double z, double w);
  double BetaContinuedFraction(double a, double b, double x);
  double BetaIncomplete(double a, double b, double x);
  double functionVariance(const QString &param1, const QString &param2);
  double functionVariancePopulation(const QString &param1, const QString &param2);
  double functionSkew(const QString &param1, const QString &param2);
  double functionKurt(const QString &param1, const QString &param2);

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

  public slots:
    void slotCellSelected(int row, int col);
    void slotCellChanged(int row, int col);

  public:
    Sheet(int numRows, int numCols, QWidget *parent);
    ~Sheet();
    void ReCalc(void);
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
