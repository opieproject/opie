#include "sheet.h"

#include <qmainwindow.h>
#include <qmessagebox.h>
#include <math.h>

#define DEFAULT_COL_WIDTH 50

Sheet::Sheet(int numRows, int numCols, QWidget *parent)
      :QTable(numRows, numCols, parent)
{
  defaultBorders.right=defaultBorders.bottom=QPen(Qt::gray, 1, Qt::SolidLine);
  defaultCellData.data="";
  defaultCellData.background=QBrush(Qt::white, Qt::SolidPattern);
  defaultCellData.alignment=(Qt::AlignmentFlags)(Qt::AlignLeft | Qt::AlignTop);
  defaultCellData.fontColor=Qt::black;
  defaultCellData.font=font();
  defaultCellData.borders=defaultBorders;

  selectionNo=-1;
  setSelectionMode(QTable::Single);

  sheetData.setAutoDelete(TRUE);
  clipboardData.setAutoDelete(TRUE);
  for (int i=0; i<numCols; ++i)
    horizontalHeader()->setLabel(i, getHeaderString(i+1), DEFAULT_COL_WIDTH);

  connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(slotCellSelected(int, int)));
  connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(slotCellChanged(int, int)));
}

Sheet::~Sheet()
{
}

typeCellData *Sheet::findCellData(int row, int col)
{
  typeCellData *tempCellData;
  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->row==row && tempCellData->col==col)
      return tempCellData;
  return NULL;
}

void Sheet::slotCellSelected(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (cellData)
    emit currentDataChanged(cellData->data);
  else
    emit currentDataChanged("");
}

typeCellData *Sheet::createCellData(int row, int col)
{
  if (row<0 || col<0) return NULL;
  typeCellData *cellData=new typeCellData;
  cellData->row=row;
  cellData->col=col;
  cellData->data=defaultCellData.data;
  cellData->borders=defaultCellData.borders;
  cellData->alignment=defaultCellData.alignment;
  cellData->font=defaultCellData.font;
  cellData->fontColor=defaultCellData.fontColor;
  cellData->background=defaultCellData.background;
  sheetData.append(cellData);
  return cellData;
}

void Sheet::slotCellChanged(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData) cellData->data=text(row, col);
  for (cellData=sheetData.first(); cellData; cellData=sheetData.next())
    setText(cellData->row, cellData->col, dataParser(cellData->data));
  emit sheetModified();
}

void Sheet::swapCells(int row1, int col1, int row2, int col2)
{
  typeCellData *cellData1=findCellData(row1, col1), *cellData2=findCellData(row2, col2);
  if (!cellData1) cellData1=createCellData(row1, col1);
  if (!cellData2) cellData2=createCellData(row2, col2);
  if (cellData1 && cellData2)
  {
    QString tempData(cellData1->data);
    cellData1->data=cellData2->data;
    cellData2->data=tempData;
    setText(cellData1->row, cellData1->col, dataParser(cellData1->data));
    setText(cellData2->row, cellData2->col, dataParser(cellData2->data));
    emit sheetModified();
  }
}

QString Sheet::getParameter(const QString &parameters, int paramNo, bool giveError=FALSE, const QString funcName="")
{
  QString params(parameters);
  int position;
  for (int i=0; i<paramNo; ++i)
  {
    position=params.find(',');
    if (position<0)
    {
      if (giveError) QMessageBox::critical(this, tr("Error"), tr("Too few arguments to function '"+funcName+'\''));
      return QString();
    }
    params=params.mid(position+1);
  }
  position=params.find(',');
  if (position<0) return params;
  return params.left(position);
}

bool Sheet::findRange(const QString &variable1, const QString &variable2, int *row1, int *col1, int *row2, int *col2)
{
  int row, col;
  if (!findRowColumn(variable1, row1, col1, TRUE) || !findRowColumn(variable2, row2, col2, TRUE)) return FALSE;
  if (*row1>*row2)
  {
    row=*row1;
    *row1=*row2;
    *row2=row;
  }
  if (*col1>*col2)
  {
    col=*col1;
    *col1=*col2;
    *col2=col;
  }
  return TRUE;
}

bool Sheet::findRowColumn(const QString &variable, int *row, int *col, bool giveError=FALSE)
{
  int position=variable.find(QRegExp("\\d"));
  if (position<1)
  {
    if (giveError) QMessageBox::critical(this, tr("Error"), tr("Invalid variable: '"+variable+'\''));
    return FALSE;
  }
  *row=variable.mid(position).toInt()-1;
  *col=getHeaderColumn(variable.left(position))-1;
  return TRUE;
}

double Sheet::calculateVariable(const QString &variable)
{
  bool ok;
  double tempResult=variable.toDouble(&ok);
  if (ok) return tempResult;

  int row, col;
  return (findRowColumn(variable, &row, &col, TRUE) ? text(row, col).toDouble() : 0);
}

double Sheet::functionSum(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;

  double result=0, tempResult;
  bool ok;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempResult=text(row, col).toDouble(&ok);
      if (ok) result+=tempResult;
    }

  return result;
}

double Sheet::functionMin(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;

  double min=0, tempMin;
  bool ok, init=FALSE;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempMin=text(row, col).toDouble(&ok);
      if (ok && (!init || tempMin<min))
      {
        min=tempMin;
        init=TRUE;
      }
    }

  return min;
}

double Sheet::functionMax(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;

  double max=0, tempMax;
  bool ok, init=FALSE;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempMax=text(row, col).toDouble(&ok);
      if (ok && (!init || tempMax>max))
      {
        max=tempMax;
        init=TRUE;
      }
    }

  return max;
}

double Sheet::functionAvg(const QString &param1, const QString &param2)
{
  double resultSum=functionSum(param1, param2), resultCount=functionCount(param1, param2);
  return (resultCount>0 ? resultSum/resultCount : 0);
}

double Sheet::functionCount(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;

  int divider=0;
  bool ok;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      text(row, col).toDouble(&ok);
      if (ok) ++divider;
    }

  return divider;
}

double Sheet::calculateFunction(const QString &function, const QString &parameters)
{
  if (function=="+")
    return calculateVariable(getParameter(parameters, 0))+calculateVariable(getParameter(parameters, 1));
  if (function=="-")
    return calculateVariable(getParameter(parameters, 0))-calculateVariable(getParameter(parameters, 1));
  if (function=="*")
    return calculateVariable(getParameter(parameters, 0))*calculateVariable(getParameter(parameters, 1));
  if (function=="/")
    return calculateVariable(getParameter(parameters, 0))/calculateVariable(getParameter(parameters, 1));
  if (function=="SUM")
    return functionSum(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
  if (function=="COUNT")
    return functionCount(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
  if (function=="MIN")
    return functionMin(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
  if (function=="MAX")
    return functionMax(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
  if (function=="AVG")
    return functionAvg(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
  if (function=="ABS")
    return fabs(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="SIN")
    return sin(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="COS")
    return cos(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="TAN")
    return tan(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="ATAN")
    return atan(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="ATAN2")
    return atan2(calculateVariable(getParameter(parameters, 0, TRUE, function)), calculateVariable(getParameter(parameters, 1, TRUE, function)));
  if (function=="ASIN")
    return asin(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="ACOS")
    return acos(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="EXP")
    return exp(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="LOG")
    return log(calculateVariable(getParameter(parameters, 0, TRUE, function)));
  if (function=="POW")
    return pow(calculateVariable(getParameter(parameters, 0, TRUE, function)), calculateVariable(getParameter(parameters, 1, TRUE, function)));
  return 0;
}

int Sheet::getOperatorPriority(char oper)
{
  switch (oper)
  {
    case '+':
    case '-':
      return 1;

    case '*':
    case '/':
      return 2;
  }
  return 0;
}

void Sheet::pushCharStack(QStack<QChar> *stackChars, const QChar &character)
{
  QChar *temp=new QChar(character);
  stackChars->push(temp);
}

void Sheet::pushStringStack(QStack<QString> *stackStrings, const QString &string)
{
  QString *temp=new QString(string);
  stackStrings->push(temp);
}

QChar Sheet::popCharStack(QStack<QChar> *stackChars)
{
  if (stackChars->isEmpty())
  {
    QMessageBox::critical(this, tr("Error"), tr("Syntax error!"));
    return '0';
  }

  QChar *temp=stackChars->pop();
  QChar temp2(*temp);
  delete temp;
  return temp2;
}

QString Sheet::popStringStack(QStack<QString> *stackStrings)
{
  if (stackStrings->isEmpty())
  {
    QMessageBox::critical(this, tr("Error"), tr("Syntax error!"));
    return "0";
  }

  QString *temp=stackStrings->pop();
  QString temp2(*temp);
  delete temp;
  return temp2;
}

QString Sheet::dataParserHelper(const QString &data)
{
  QStack<QString> stackElements;
  QStack<QChar> stackOperators;
  QString tempElement(""), temp2Element, firstElement, secondElement;
  int paranCount;

  for (unsigned int i=0; i<data.length(); ++i)
  {
    if (data[i]=='+' || data[i]=='-' || data[i]=='*' || data[i]=='/')
    {
      pushStringStack(&stackElements, tempElement);
      tempElement="";
      if (!stackOperators.isEmpty() && getOperatorPriority(*stackOperators.top())>getOperatorPriority(data[i]))
      {
        secondElement=popStringStack(&stackElements);
        firstElement=popStringStack(&stackElements);
        pushStringStack(&stackElements, QString::number(calculateFunction(popCharStack(&stackOperators), firstElement+","+secondElement)));
      }
      pushCharStack(&stackOperators, data[i]);
    }
    else
    if (data[i]==',')
    {
      if (!tempElement.isEmpty()) pushStringStack(&stackElements, tempElement);
      while (!stackOperators.isEmpty())
      {
        secondElement=popStringStack(&stackElements);
        firstElement=popStringStack(&stackElements);
        pushStringStack(&stackElements, QString::number(calculateFunction(popCharStack(&stackOperators), firstElement+","+secondElement)));
      }
      tempElement="";
    }
    else
    if (data[i]=='(')
    {
      paranCount=1;
      temp2Element="";
      for (++i; paranCount>0; ++i)
      {
        temp2Element+=data[i];
        if (data[i]=='(') ++paranCount;
        if (data[i]==')') --paranCount;
      }
      temp2Element=dataParserHelper(temp2Element.left(temp2Element.length()-1));
      if (tempElement.isEmpty())
        tempElement=temp2Element;
      else
        tempElement.setNum(calculateFunction(tempElement, temp2Element));
      --i;
    }
    else
      tempElement+=data[i];
  }
  if (!tempElement.isEmpty()) pushStringStack(&stackElements, tempElement);
  while (!stackOperators.isEmpty())
  {
    secondElement=popStringStack(&stackElements);
    firstElement=popStringStack(&stackElements);
    pushStringStack(&stackElements, QString::number(calculateFunction(popCharStack(&stackOperators), firstElement+","+secondElement)));
  }

  if (!stackElements.isEmpty())
    tempElement=popStringStack(&stackElements);
  while (!stackElements.isEmpty())
    tempElement.prepend(popStringStack(&stackElements)+",");
  return tempElement;
}

QString Sheet::dataParser(const QString &data)
{
  QString strippedData(data);
  strippedData.replace(QRegExp("\\s"), "");
  if (strippedData.isEmpty() || strippedData[0]!='=') return data;
  strippedData=dataParserHelper(strippedData.remove(0, 1).upper().replace(QRegExp(":"), ","));

  int i=0;
  QString tempParameter(getParameter(strippedData, i)), result="";
  do
  {
    result+=","+QString::number(calculateVariable(tempParameter));
    tempParameter=getParameter(strippedData, ++i);
  }
  while (!tempParameter.isNull());
  return result.mid(1);
}

void Sheet::setData(const QString &data)
{
  setText(currentRow(), currentColumn(), data);
  slotCellChanged(currentRow(), currentColumn());
  activateNextCell();
}

QString Sheet::getData()
{
  typeCellData *cellData=findCellData(currentRow(), currentColumn());
  if (cellData)
    return cellData->data;
  return "";
}

void Sheet::lockClicks(bool lock=TRUE)
{
  clicksLocked=lock;
}

void Sheet::paintCell(QPainter *p, int row, int col, const QRect & cr, bool selected)
{
  if (selected && row==currentRow() && col==currentColumn()) selected=FALSE;

  int sheetDataCurrent=sheetData.at();
  typeCellData *cellData=findCellData(row, col);
  if (sheetDataCurrent>=0) sheetData.at(sheetDataCurrent);
  if (!cellData) cellData=&defaultCellData;
  if (selected)
    p->fillRect(0, 0, cr.width(), cr.height(), colorGroup().highlight());
  else
  {
    p->fillRect(0, 0, cr.width(), cr.height(), colorGroup().base());
    p->fillRect(0, 0, cr.width(), cr.height(), cellData->background);
  }

  QTableItem *cellItem=item(row, col);
  if (cellItem)
  {
    p->setPen(selected ? colorGroup().highlightedText() : cellData->fontColor);
    p->setFont(cellData->font);
    p->drawText(2, 2, cr.width()-4, cr.height()-4, cellData->alignment, cellItem->text());
  }

  int rx=cr.width()-1, ry=cr.height()-1;
  QPen pen(p->pen());
  p->setPen(cellData->borders.right);
  p->drawLine(rx, 0, rx, ry);
  p->setPen(cellData->borders.bottom);
  p->drawLine(0, ry, rx, ry);
  p->setPen(pen);
}

void Sheet::viewportMousePressEvent(QMouseEvent *e)
{
  QMouseEvent ce(e->type(), viewportToContents(e->pos()), e->globalPos(), e->button(), e->state());
  if (clicksLocked)
  {
    if (selectionNo<0)
    {
      clearSelection();
      QTableSelection newSelection;
      newSelection.init(rowAt(ce.pos().y()), columnAt(ce.pos().x()));
      newSelection.expandTo(newSelection.anchorRow(), newSelection.anchorCol());
      selectionNo=addSelection(newSelection);
    }
  }
  else
    QTable::contentsMousePressEvent(&ce);
}

void Sheet::viewportMouseMoveEvent(QMouseEvent *e)
{
  QMouseEvent ce(e->type(), viewportToContents(e->pos()), e->globalPos(), e->button(), e->state());
  if (clicksLocked)
  {
    if (selectionNo>=0)
    {
      QTableSelection oldSelection(selection(selectionNo));
      oldSelection.expandTo(rowAt(ce.pos().y()), columnAt(ce.pos().x()));
      if (!(oldSelection==selection(selectionNo)))
      {
        removeSelection(selectionNo);
        selectionNo=addSelection(oldSelection);
      }
    }
  }
  else
    QTable::contentsMouseMoveEvent(&ce);
}

void Sheet::viewportMouseReleaseEvent(QMouseEvent *e)
{
  QMouseEvent ce(e->type(), viewportToContents(e->pos()), e->globalPos(), e->button(), e->state());
  if (clicksLocked && selectionNo>=0)
  {
    QTableSelection oldSelection(selection(selectionNo));
    oldSelection.expandTo(rowAt(ce.pos().y()), columnAt(ce.pos().x()));
    removeSelection(selectionNo);
    selectionNo=-1;
    if (oldSelection.topRow()==oldSelection.bottomRow() && oldSelection.leftCol()==oldSelection.rightCol())
      emit cellClicked(getHeaderString(oldSelection.leftCol()+1)+QString::number(oldSelection.topRow()+1));
    else
      emit cellClicked(getHeaderString(oldSelection.leftCol()+1)+QString::number(oldSelection.topRow()+1)+','+getHeaderString(oldSelection.rightCol()+1)+QString::number(oldSelection.bottomRow()+1));
  }
  else
    QTable::contentsMouseReleaseEvent(&ce);
}

void Sheet::copySheetData(QList<typeCellData> *destSheetData)
{
  typeCellData *tempCellData, *newCellData;
  destSheetData->clear();

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
  {
    newCellData=new typeCellData;
    *newCellData=*tempCellData;
    destSheetData->append(newCellData);
  }
}

void Sheet::setSheetData(QList<typeCellData> *srcSheetData)
{
  typeCellData *tempCellData, *newCellData;

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
  {
    clearCell(tempCellData->row, tempCellData->col);
    updateCell(tempCellData->row, tempCellData->col);
  }
  sheetData.clear();

  for (tempCellData=srcSheetData->first(); tempCellData; tempCellData=srcSheetData->next())
  {
    newCellData=new typeCellData;
    *newCellData=*tempCellData;
    sheetData.append(newCellData);
    setText(newCellData->row, newCellData->col, dataParser(newCellData->data));
  }
  emit sheetModified();
}

void Sheet::setName(const QString &name)
{
  sheetName=name;
  emit sheetModified();
}

QString Sheet::getName()
{
  return sheetName;
}

void Sheet::setBrush(int row, int col, const QBrush &brush)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    cellData->background=brush;
    emit sheetModified();
  }
}

QBrush Sheet::getBrush(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->background;
}

void Sheet::setTextAlign(int row, int col, Qt::AlignmentFlags flags)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    cellData->alignment=flags;
    emit sheetModified();
  }
}

Qt::AlignmentFlags Sheet::getAlignment(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->alignment;
}

void Sheet::setTextFont(int row, int col, const QFont &font, const QColor &color)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    cellData->font=font;
    cellData->fontColor=color;
    emit sheetModified();
  }
}

QFont Sheet::getFont(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->font;
}

QColor Sheet::getFontColor(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->fontColor;
}

void Sheet::setPen(int row, int col, int vertical, const QPen &pen)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    if (vertical)
      cellData->borders.right=pen;
    else
      cellData->borders.bottom=pen;
    emit sheetModified();
  }
}

QPen Sheet::getPen(int row, int col, int vertical)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return (vertical ? cellData->borders.right : cellData->borders.bottom);
}

void Sheet::getSelection(int *row1, int *col1, int *row2, int *col2)
{
  int selectionNo=currentSelection();
  if (selectionNo>=0)
  {
    QTableSelection selection(selection(selectionNo));
    *row1=selection.topRow();
    *row2=selection.bottomRow();
    *col1=selection.leftCol();
    *col2=selection.rightCol();
  }
  else
  {
    *row1=*row2=currentRow();
    *col1=*col2=currentColumn();
  }
}

void Sheet::editClear()
{
  int row1, row2, col1, col2;
  getSelection(&row1, &col1, &row2, &col2);

  int row, col;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      setText(row, col, "");
      slotCellChanged(row, col);
    }
}

void Sheet::editCopy()
{
  clipboardData.clear();

  int row1, row2, col1, col2;
  getSelection(&row1, &col1, &row2, &col2);

  typeCellData *cellData, *newCellData;
  int row, col;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      cellData=findCellData(row, col);
      if (cellData)
      {
        newCellData=new typeCellData;
        *newCellData=*cellData;
        newCellData->row-=row1;
        newCellData->col-=col1;
        clipboardData.append(newCellData);
      }
    }
}

void Sheet::editCut()
{
  editCopy();
  editClear();
}

void Sheet::editPaste(bool onlyContents=FALSE)
{
  int row1=currentRow(), col1=currentColumn();
  typeCellData *cellData, *tempCellData;

  for (tempCellData=clipboardData.first(); tempCellData; tempCellData=clipboardData.next())
  {
    cellData=findCellData(tempCellData->row+row1, tempCellData->col+col1);
    if (!cellData) cellData=createCellData(tempCellData->row+row1, tempCellData->col+col1);
    if (cellData)
    {
      if (onlyContents)
        cellData->data=tempCellData->data;
      else
      {
        *cellData=*tempCellData;
        cellData->row+=row1;
        cellData->col+=col1;
      }
      setText(cellData->row, cellData->col, dataParser(cellData->data));
      emit sheetModified();
    }
  }
}

void Sheet::insertRows(int no=1, bool allColumns=TRUE)
{
  setNumRows(numRows()+no);

  typeCellData *tempCellData;
  int row=currentRow(), col=currentColumn();

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->row>=row && (allColumns || tempCellData->col==col))
    {
      clearCell(tempCellData->row, tempCellData->col);
      tempCellData->row+=no;
    }
  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->row>=row && (allColumns || tempCellData->col==col))
    {
      updateCell(tempCellData->row-no, tempCellData->col);
      setText(tempCellData->row, tempCellData->col, dataParser(tempCellData->data));
    }
 emit sheetModified();
}

void Sheet::insertColumns(int no=1, bool allRows=TRUE)
{
  int noCols=numCols();
  int newCols=noCols+no;
  setNumCols(newCols);
  for (int i=noCols; i<newCols; ++i)
    horizontalHeader()->setLabel(i, getHeaderString(i+1), DEFAULT_COL_WIDTH);

  typeCellData *tempCellData;
  int col=currentColumn(), row=currentRow();

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->col>=col && (allRows || tempCellData->row==row))
    {
      clearCell(tempCellData->row, tempCellData->col);
      tempCellData->col+=no;
    }
  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->col>=col && (allRows || tempCellData->row==row))
    {
      updateCell(tempCellData->row, tempCellData->col-no);
      setText(tempCellData->row, tempCellData->col, dataParser(tempCellData->data));
    }
  emit sheetModified();
}

void Sheet::dataFindReplace(const QString &findStr, const QString &replaceStr, bool matchCase=TRUE, bool allCells=TRUE, bool entireCell=FALSE, bool replace=FALSE, bool replaceAll=FALSE)
{
  typeCellData *tempCellData;
  int row1, col1, row2, col2;
  getSelection(&row1, &col1, &row2, &col2);
  bool found=FALSE;

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (allCells || (tempCellData->row>=row1 && tempCellData->row<=row2 && tempCellData->col>=col1 && tempCellData->col<=col2))
    {
      QTableItem *cellItem=item(tempCellData->row, tempCellData->col);
      if (cellItem && (entireCell ? (matchCase ? cellItem->text()==findStr : cellItem->text().upper()==findStr.upper()) : cellItem->text().find(findStr, 0, matchCase)>=0))
      {
        if (!found)
        {
          found=TRUE;
          clearSelection();
        }
        setCurrentCell(tempCellData->row, tempCellData->col);
        if (replace)
        {
          tempCellData->data=cellItem->text().replace(QRegExp(findStr, matchCase), replaceStr);
          setText(tempCellData->row, tempCellData->col, dataParser(tempCellData->data));
        }
        if (!replace || !replaceAll) break;
      }
    }

 if (found)
 {
   if (replace)
     slotCellChanged(currentRow(), currentColumn());
 }
 else
   QMessageBox::warning(this, tr("Error"), tr("Search key not found!"));
}

//
// Static functions
//

QString Sheet::getHeaderString(int section)
{
  if (section<1) return "";
  return getHeaderString((section-1)/26)+QChar('A'+(section-1)%26);
}

int Sheet::getHeaderColumn(const QString &section)
{
  if (section.isEmpty()) return 0;
  return (section[section.length()-1]-'A'+1)+getHeaderColumn(section.left(section.length()-1))*26;
}
