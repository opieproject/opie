#include "cellformat.h"

#include <qlistbox.h>
#include <qlabel.h>

#define COMBO_WIDTHS 155
#define COMBO_HEIGHTS 21

#define COLOR_COUNT 17
#define STYLE_COUNT 14
#define HALIGN_COUNT 3
#define VALIGN_COUNT 3

QColor qtColors[COLOR_COUNT]={Qt::black, Qt::white, Qt::darkGray, Qt::gray, Qt::lightGray, Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};
Qt::BrushStyle brushStyles[STYLE_COUNT]={Qt::SolidPattern, Qt::Dense1Pattern, Qt::Dense2Pattern, Qt::Dense3Pattern, Qt::Dense4Pattern, Qt::Dense5Pattern, Qt::Dense6Pattern, Qt::Dense7Pattern, Qt::HorPattern, Qt::VerPattern, Qt::CrossPattern, Qt::BDiagPattern, Qt::FDiagPattern, Qt::DiagCrossPattern};
QString namesHAlign[HALIGN_COUNT]={"Left", "Right", "Center"};
QString namesVAlign[VALIGN_COUNT]={"Top", "Bottom", "Center"};
Qt::AlignmentFlags flagsHAlign[HALIGN_COUNT]={Qt::AlignLeft, Qt::AlignRight, Qt::AlignHCenter};
Qt::AlignmentFlags flagsVAlign[VALIGN_COUNT]={Qt::AlignTop, Qt::AlignBottom, Qt::AlignVCenter};

CellFormat::CellFormat(QWidget *parent=0)
           :QDialog(parent, 0, TRUE)
{
  // Main widget
  tabs=new QTabWidget(this);
  widgetBorders=new QWidget(tabs);
  widgetBackground=new QWidget(tabs);
  widgetFont=new QWidget(tabs);
  widgetAlignment=new QWidget(tabs);
  tabs->addTab(widgetBorders, tr("&Borders"));
  tabs->addTab(widgetBackground, tr("Back&ground"));
  tabs->addTab(widgetFont, tr("&Font"));
  tabs->addTab(widgetAlignment, tr("&Alignment"));

  fontDB.loadRenderers();
  changedFont=changedAlign=changedBrush=FALSE;

  // Borders tab
  borderEditor=new BorderEditor(widgetBorders);
  borderEditor->setGeometry(10, 10, 215, 145);
  connect(borderEditor, SIGNAL(clicked(BorderEditor::BorderArea)), this, SLOT(borderClicked(BorderEditor::BorderArea)));

  comboBordersWidth=createCombo(COMBO_WIDTH, widgetBorders, tr("&Width:"), 165);
  comboBordersColor=createCombo(COMBO_COLOR, widgetBorders, tr("&Color:"), 165+(COMBO_HEIGHTS+10));

  buttonBordersDefaults=new QPushButton(tr("&Default Borders"), widgetBorders);
  buttonBordersDefaults->setGeometry(115, 165+2*(COMBO_HEIGHTS+10), 110, COMBO_HEIGHTS);
  connect(buttonBordersDefaults, SIGNAL(clicked()), this, SLOT(slotBordersDefaults()));

  // Background tab
  frameBackground=new QFrame(widgetBackground);
  frameBackground->setGeometry(10, 10, 215, 145);
  frameBackground->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

  comboBackgroundStyle=createCombo(COMBO_STYLE, widgetBackground, tr("&Style:"), 165);
  connect(comboBackgroundStyle, SIGNAL(activated(int)), this, SLOT(backgroundClicked(int)));
  comboBackgroundColor=createCombo(COMBO_COLOR, widgetBackground, tr("&Color:"), 165+(COMBO_HEIGHTS+10));
  connect(comboBackgroundColor, SIGNAL(activated(int)), this, SLOT(backgroundClicked(int)));

  buttonBackgroundDefaults=new QPushButton(tr("&Default Background"), widgetBackground);
  buttonBackgroundDefaults->setGeometry(115, 165+2*(COMBO_HEIGHTS+10), 110, COMBO_HEIGHTS);
  connect(buttonBackgroundDefaults, SIGNAL(clicked()), this, SLOT(slotBackgroundDefaults()));

  // Font tab
  frameFont=new QFrame(widgetFont);
  frameFont->setGeometry(10, 10, 215, 125);
  frameFont->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

  comboFontFamily=createCombo(COMBO_FONT, widgetFont, tr("&Font:"), 145);
  connect(comboFontFamily, SIGNAL(activated(int)), this, SLOT(fontClicked(int)));
  comboFontSize=createCombo(COMBO_SIZE, widgetFont, tr("&Size:"), 145+(COMBO_HEIGHTS+10));
  connect(comboFontSize, SIGNAL(activated(int)), this, SLOT(fontClicked(int)));
  comboFontColor=createCombo(COMBO_COLOR, widgetFont, tr("&Color:"), 145+2*(COMBO_HEIGHTS+10));
  connect(comboFontColor, SIGNAL(activated(int)), this, SLOT(fontClicked(int)));

  checkFontBold=new QCheckBox(tr("&Bold"), widgetFont);
  checkFontBold->setGeometry(10, 145+3*(COMBO_HEIGHTS+10), 40, COMBO_HEIGHTS);
  connect(checkFontBold, SIGNAL(toggled(bool)), this, SLOT(fontClicked(bool)));
  checkFontItalic=new QCheckBox(tr("&Italic"), widgetFont);
  checkFontItalic->setGeometry(60, 145+3*(COMBO_HEIGHTS+10), 40, COMBO_HEIGHTS);
  connect(checkFontItalic, SIGNAL(toggled(bool)), this, SLOT(fontClicked(bool)));

  buttonFontDefaults=new QPushButton(tr("&Default Font"), widgetFont);
  buttonFontDefaults->setGeometry(115, 145+3*(COMBO_HEIGHTS+10), 110, COMBO_HEIGHTS);
  connect(buttonFontDefaults, SIGNAL(clicked()), this, SLOT(slotFontDefaults()));

  // Alignment tab
  frameAlignment=new QFrame(widgetAlignment);
  frameAlignment->setGeometry(10, 10, 215, 145);
  frameAlignment->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

  comboAlignmentVertical=createCombo(COMBO_VALIGN, widgetAlignment, tr("&Vertical:"), 165);
  connect(comboAlignmentVertical, SIGNAL(activated(int)), this, SLOT(alignClicked(int)));
  comboAlignmentHorizontal=createCombo(COMBO_HALIGN, widgetAlignment, tr("&Horizontal:"), 165+(COMBO_HEIGHTS+10));
  connect(comboAlignmentHorizontal, SIGNAL(activated(int)), this, SLOT(alignClicked(int)));

  checkAlignmentWrap=new QCheckBox(tr("&Word Wrap"), widgetAlignment);
  checkAlignmentWrap->setGeometry(10, 165+2*(COMBO_HEIGHTS+10), 90, COMBO_HEIGHTS);
  connect(checkAlignmentWrap, SIGNAL(toggled(bool)), this, SLOT(alignClicked(bool)));

  buttonAlignmentDefaults=new QPushButton(tr("&Default Alignment"), widgetAlignment);
  buttonAlignmentDefaults->setGeometry(115, 165+2*(COMBO_HEIGHTS+10), 110, COMBO_HEIGHTS);
  connect(buttonAlignmentDefaults, SIGNAL(clicked()), this, SLOT(slotAlignmentDefaults()));

  // Main widget
  box=new QVBoxLayout(this);
  box->addWidget(tabs);

  setCaption(tr("Format Cells"));
}

CellFormat::~CellFormat()
{
}

int CellFormat::findColorIndex(const QColor &color)
{
  for (int i=0; i<COLOR_COUNT; ++i)
    if (qtColors[i]==color)
      return i;
  return 0;
}

int CellFormat::findVAlignIndex(Qt::AlignmentFlags flag)
{
  for (int i=0; i<VALIGN_COUNT; ++i)
    if (flagsVAlign[i] & flag)
      return i;
  return 0;
}

int CellFormat::findHAlignIndex(Qt::AlignmentFlags flag)
{
  for (int i=0; i<HALIGN_COUNT; ++i)
    if (flagsHAlign[i] & flag)
      return i;
  return 0;
}

int CellFormat::findBrushStyleIndex(Qt::BrushStyle style)
{
  for (int i=0; i<STYLE_COUNT; ++i)
    if (brushStyles[i]==style)
      return i;
  return 0;
}

void CellFormat::setBrushBackground(const QBrush &brush)
{
  comboBackgroundColor->setCurrentItem(findColorIndex(brush.color()));
  comboBackgroundStyle->setCurrentItem(findBrushStyleIndex(brush.style()));

  QPixmap pix(frameBackground->contentsRect().width(), frameBackground->contentsRect().height());
  QPainter p(&pix);
  pix.fill();
  p.fillRect(pix.rect(), brush);
  frameBackground->setBackgroundPixmap(pix);

  brushBackground=brush;
}

void CellFormat::setTextFont(const QFont &font, const QColor &color)
{
  comboFontColor->setCurrentItem(findColorIndex(color));
  comboFontFamily->setCurrentItem(findComboItemIndex(comboFontFamily, font.family()));
  comboFontSize->setCurrentItem(findComboItemIndex(comboFontSize, QString::number(font.pointSize())));
  checkFontBold->setChecked(font.weight()==QFont::Bold);
  checkFontItalic->setChecked(font.italic());

  QPixmap pix(frameFont->contentsRect().width(), frameFont->contentsRect().height());
  QPainter p(&pix);
  pix.fill();
  p.fillRect(pix.rect(), Qt::white);
  p.setFont(font);
  p.setPen(color);
  p.drawText(pix.rect(), Qt::AlignCenter, tr("Opie Sheet"));
  frameFont->setBackgroundPixmap(pix);

  fontFont=font;
  fontColor=color;
}

void CellFormat::setTextAlign(Qt::AlignmentFlags flags)
{
  comboAlignmentVertical->setCurrentItem(findVAlignIndex(flags));
  comboAlignmentHorizontal->setCurrentItem(findHAlignIndex(flags));
  checkAlignmentWrap->setChecked(flags & Qt::WordBreak);

  QPixmap pix(frameAlignment->contentsRect().width(), frameAlignment->contentsRect().height());
  QPainter p(&pix);
  pix.fill();
  p.fillRect(pix.rect(), Qt::white);
  p.drawText(10, 10, pix.width()-20, pix.height()-20, flags, tr("Opie Sheet"));
  frameAlignment->setBackgroundPixmap(pix);

  textAlignment=flags;
}

void CellFormat::slotFontDefaults()
{
  changedFont=TRUE;
  setTextFont(font(), Qt::black);
}

void CellFormat::slotAlignmentDefaults()
{
  changedAlign=TRUE;
  setTextAlign((Qt::AlignmentFlags)(Qt::AlignLeft | Qt::AlignTop));
}

void CellFormat::slotBackgroundDefaults()
{
  changedBrush=TRUE;
  setBrushBackground(Qt::white);
}

void CellFormat::slotBordersDefaults()
{
  QPen defaultPen(Qt::gray, 1, Qt::SolidLine);
  borderEditor->setPen(defaultPen, BorderEditor::Top);
  borderEditor->setPen(defaultPen, BorderEditor::Bottom);
  borderEditor->setPen(defaultPen, BorderEditor::Left);
  borderEditor->setPen(defaultPen, BorderEditor::Right);
  borderEditor->setPen(defaultPen, BorderEditor::Vert);
  borderEditor->setPen(defaultPen, BorderEditor::Horz);
}

void CellFormat::backgroundClicked(int index)
{
  changedBrush=TRUE;
  setBrushBackground(QBrush(qtColors[comboBackgroundColor->currentItem()], brushStyles[comboBackgroundStyle->currentItem()]));
}

void CellFormat::fontClicked(bool on)
{
  fontClicked(0);
}

void CellFormat::fontClicked(int index)
{
  changedFont=TRUE;
  setTextFont(QFont(comboFontFamily->currentText(), comboFontSize->currentText().toInt(), checkFontBold->isChecked() ? QFont::Bold : QFont::Normal, checkFontItalic->isChecked(), QFont::AnyCharSet), qtColors[comboFontColor->currentItem()]);
}

void CellFormat::alignClicked(bool on)
{
  alignClicked(0);
}

void CellFormat::alignClicked(int index)
{
  changedAlign=TRUE;
  setTextAlign((Qt::AlignmentFlags)(flagsVAlign[comboAlignmentVertical->currentItem()] | flagsHAlign[comboAlignmentHorizontal->currentItem()] | (checkAlignmentWrap->isChecked() ? Qt::WordBreak : 0)));
}

void CellFormat::createSizeCombo(QComboBox *combo)
{
  combo->clear();
  QValueList<int> sizes=fontDB.standardSizes();
  for (QValueList<int>::ConstIterator i=sizes.begin(); i!=sizes.end(); ++i)
    combo->insertItem(QString::number(*i));
}

void CellFormat::borderClicked(BorderEditor::BorderArea area)
{
  QPen newPen(qtColors[comboBordersColor->currentItem()], comboBordersWidth->currentItem()+1, Qt::SolidLine);
  if (newPen==borderEditor->getPen(area))
    borderEditor->setPen(QPen(Qt::gray, 1, Qt::NoPen), area);
  else
    borderEditor->setPen(newPen, area);
}

int CellFormat::findComboItemIndex(QComboBox *combo, const QString &item)
{
  for (int i=0; i<combo->count(); ++i)
    if (combo->text(i)==item)
      return i;
  return 0;
}

QComboBox *CellFormat::createCombo(comboType type, QWidget *parent, const QString &caption, int y)
{
  QComboBox *combo=new QComboBox(FALSE, parent);
  combo->setGeometry(70, y, COMBO_WIDTHS, COMBO_HEIGHTS);
  combo->setSizeLimit(5);

  switch (type)
  {
    case COMBO_WIDTH: createWidthCombo(combo); break;
    case COMBO_STYLE: createStyleCombo(combo); break;
    case COMBO_FONT: createFontCombo(combo); break;
    case COMBO_SIZE: createSizeCombo(combo); break;
    case COMBO_COLOR: createColorCombo(combo); break;
    case COMBO_HALIGN: createHAlignCombo(combo); break;
    case COMBO_VALIGN: createVAlignCombo(combo); break;
  }

  QLabel *label=new QLabel(combo, caption, parent);
  label->setGeometry(10, y, 50, COMBO_HEIGHTS);

  return combo;
}

void CellFormat::createHAlignCombo(QComboBox *combo)
{
  for (int i=0; i<HALIGN_COUNT; ++i)
    combo->insertItem(namesHAlign[i]);
}

void CellFormat::createVAlignCombo(QComboBox *combo)
{
  for (int i=0; i<VALIGN_COUNT; ++i)
    combo->insertItem(namesVAlign[i]);
}

void CellFormat::createWidthCombo(QComboBox *combo)
{
  int width=combo->listBox()->maxItemWidth();
  QPixmap pix(width, COMBO_HEIGHTS);
  QPainter p(&pix);

  for (int i=1; i<=6; ++i)
  {
    pix.fill();
    p.setPen(QPen(Qt::black, i, Qt::SolidLine));
    p.drawLine(5, COMBO_HEIGHTS/2, width-10, COMBO_HEIGHTS/2);
    combo->insertItem(pix);
  }
}

void CellFormat::createFontCombo(QComboBox *combo)
{
  combo->insertStringList(fontDB.families());
}

void CellFormat::createStyleCombo(QComboBox *combo)
{
  int width=combo->listBox()->maxItemWidth();
  QPixmap pix(width, COMBO_HEIGHTS);
  QPainter p(&pix);

  for (int i=0; i<STYLE_COUNT; ++i)
  {
    pix.fill();
    p.fillRect(5, 5, width-10, COMBO_HEIGHTS-10, brushStyles[i]);
    combo->insertItem(pix);
  }
}

void CellFormat::createColorCombo(QComboBox *combo)
{
  int width=combo->listBox()->maxItemWidth();
  QPixmap pix(width, COMBO_HEIGHTS);
  QPainter p(&pix);

  for (int i=0; i<COLOR_COUNT; ++i)
  {
    pix.fill();
    p.setPen(QPen(qtColors[i], 3, Qt::SolidLine));
    p.drawLine(5, COMBO_HEIGHTS/2, width-10, COMBO_HEIGHTS/2);
    combo->insertItem(pix);
  }
}

int CellFormat::exec(Sheet *s)
{
  sheet=s;
  int row1, row2, col1, col2, row, col;
  sheet->getSelection(&row1, &col1, &row2, &col2);

  QPen penTop=sheet->getPen(row1-1, col1, 0), penBottom=sheet->getPen(row2, col1, 0),
       penLeft=sheet->getPen(row1, col1-1, 1), penRight=sheet->getPen(row1, col2, 1),
       penVert=sheet->getPen(row1, col1, 1), penHorz=sheet->getPen(row1, col1, 0),
       penDefault=borderEditor->getDefaultPen();
  for (row=row1+1; row<=row2; ++row)
    if (sheet->getPen(row, col1-1, 1)!=penLeft)
    {
      penLeft=penDefault;
      break;
    }
  for (row=row1+1; row<=row2; ++row)
    if (sheet->getPen(row, col2, 1)!=penRight)
    {
      penRight=penDefault;
      break;
    }
  for (col=col1+1; col<=col2; ++col)
    if (sheet->getPen(row1-1, col, 0)!=penTop)
    {
      penTop=penDefault;
      break;
    }
  for (col=col1+1; col<=col2; ++col)
    if (sheet->getPen(row2, col, 0)!=penBottom)
    {
      penBottom=penDefault;
      break;
    }
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<col2; ++col)
      if (sheet->getPen(row, col, 1)!=penVert)
      {
        penVert=penDefault;
        break;
      }
  for (row=row1; row<row2; ++row)
    for (col=col1; col<=col2; ++col)
      if (sheet->getPen(row, col, 0)!=penHorz)
      {
        penHorz=penDefault;
        break;
      }

  borderEditor->setPen(penTop, BorderEditor::Top);
  borderEditor->setPen(penBottom, BorderEditor::Bottom);
  borderEditor->setPen(penLeft, BorderEditor::Left);
  borderEditor->setPen(penRight, BorderEditor::Right);
  borderEditor->setPen(penVert, BorderEditor::Vert);
  borderEditor->setPen(penHorz, BorderEditor::Horz);

  setBrushBackground(sheet->getBrush(row1, col1));
  setTextFont(sheet->getFont(row1, col1), sheet->getFontColor(row1, col1));
  setTextAlign(sheet->getAlignment(row1, col1));

  if (QDialog::exec()==QDialog::Accepted)
  {
    penTop=borderEditor->getPen(BorderEditor::Top);
    penBottom=borderEditor->getPen(BorderEditor::Bottom);
    penLeft=borderEditor->getPen(BorderEditor::Left);
    penRight=borderEditor->getPen(BorderEditor::Right);
    penVert=borderEditor->getPen(BorderEditor::Vert);
    penHorz=borderEditor->getPen(BorderEditor::Horz);

    if (penTop!=penDefault)
      for (col=col1; col<=col2; ++col)
        sheet->setPen(row1-1, col, 0, penTop);
    if (penBottom!=penDefault)
      for (col=col1; col<=col2; ++col)
        sheet->setPen(row2, col, 0, penBottom);
    if (penLeft!=penDefault)
      for (row=row1; row<=row2; ++row)
        sheet->setPen(row, col1-1, 1, penLeft);
    if (penRight!=penDefault)
      for (row=row1; row<=row2; ++row)
        sheet->setPen(row, col2, 1, penRight);
    if (penVert!=penDefault)
      for (row=row1; row<=row2; ++row)
        for (col=col1; col<col2; ++col)
          sheet->setPen(row, col, 1, penVert);
    if (penHorz!=penDefault)
      for (row=row1; row<row2; ++row)
        for (col=col1; col<=col2; ++col)
          sheet->setPen(row, col, 0, penHorz);

    if (changedBrush)
    {
      for (row=row1; row<=row2; ++row)
        for (col=col1; col<=col2; ++col)
          sheet->setBrush(row, col, brushBackground);
    }

    if (changedAlign)
    {
      for (row=row1; row<=row2; ++row)
        for (col=col1; col<=col2; ++col)
          sheet->setTextAlign(row, col, textAlignment);
    }

    if (changedFont)
    {
      for (row=row1; row<=row2; ++row)
        for (col=col1; col<=col2; ++col)
          sheet->setTextFont(row, col, fontFont, fontColor);
    }
    return QDialog::Accepted;
  }
  return QDialog::Rejected;
}

//
// Border Editor
//

BorderEditor::BorderEditor(QWidget *parent=0)
             :QFrame(parent)
{
  setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
}

BorderEditor::~BorderEditor()
{
}

void BorderEditor::drawContents(QPainter *p)
{
  QFrame::drawContents(p);

  int x=contentsRect().x(), y=contentsRect().y(), width=contentsRect().width()/3, height=contentsRect().height()/3;
  int lineFirstX=x+width/6, lineFirstY=y+height/6, lineLastX=contentsRect().right()-width/6, lineLastY=contentsRect().bottom()-height/6;

  p->fillRect(contentsRect(), Qt::white);

  p->fillRect(x+width/3, y+height/3, width, height, Qt::gray);
  p->fillRect(x+(5*width/3), y+height/3, width, height, Qt::gray);
  p->fillRect(x+width/3, y+(5*height)/3, width, height, Qt::gray);
  p->fillRect(x+(5*width)/3, y+(5*height)/3, width, height, Qt::gray);

  if (penTop.width()>0)
  {
    p->setPen(penTop);
    p->drawLine(lineFirstX, lineFirstY, lineLastX, lineFirstY);
  }
  if (penBottom.width()>0)
  {
    p->setPen(penBottom);
    p->drawLine(lineFirstX, lineLastY, lineLastX, lineLastY);
  }
  if (penHorz.width()>0)
  {
    p->setPen(penHorz);
    p->drawLine(lineFirstX, y+contentsRect().height()/2, lineLastX, y+contentsRect().height()/2);
  }
  if (penLeft.width()>0)
  {
    p->setPen(penLeft);
    p->drawLine(lineFirstX, lineFirstY, lineFirstX, lineLastY);
  }
  if (penRight.width()>0)
  {
    p->setPen(penRight);
    p->drawLine(lineLastX, lineFirstY, lineLastX, lineLastY);
  }
  if (penVert.width()>0)
  {
    p->setPen(penVert);
    p->drawLine(x+contentsRect().width()/2, lineFirstY, x+contentsRect().width()/2, lineLastY);
  }
}

void BorderEditor::setPen(const QPen &pen, BorderArea area)
{
  switch (area)
  {
    case Top: penTop=pen; break;
    case Bottom: penBottom=pen; break;
    case Left: penLeft=pen; break;
    case Right: penRight=pen; break;
    case Horz: penHorz=pen; break;
    case Vert: penVert=pen; break;
  };
  update();
}

void BorderEditor::mouseReleaseEvent(QMouseEvent *e)
{
  QFrame::mouseReleaseEvent(e);

  int x=contentsRect().x(), y=contentsRect().y(), width=contentsRect().width()/3, height=contentsRect().height()/3;
  BorderArea area=None;

  if (e->x()<x+width/3) area=Left;
  if (e->x()>x+(8*width)/3) area=Right;
  if (e->x()>x+(4*width)/3 && e->x()<x+(5*width)/3) area=Vert;

  if (e->y()<y+height/3) area=Top;
  if (e->y()>y+(8*height)/3) area=Bottom;
  if (e->y()>y+(4*height)/3 && e->y()<y+(5*height)/3) area=Horz;

  if (area!=None)
    emit clicked(area);
}

QPen BorderEditor::getPen(BorderArea area)
{
  switch (area)
  {
    case Top: return penTop;
    case Bottom: return penBottom;
    case Left: return penLeft;
    case Right: return penRight;
    case Horz: return penHorz;
    case Vert: return penVert;
  };
  return getDefaultPen();
}
