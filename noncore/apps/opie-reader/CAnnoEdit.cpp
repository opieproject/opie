#include "CAnnoEdit.h"

QColor CAnnoEdit::getColor()
{
  switch (colorbox->currentItem())
    {
    case 0:
    default:
      return white;
    case 1:
      return darkGray;
    case 2:
      return gray;
    case 3:
      return lightGray;
    case 4:
      return red;
    case 5:
      return green;
    case 6:
      return blue;
    case 7:
      return cyan;
    case 8:
      return magenta;
    case 9:
      return yellow;
    case 10:
      return darkRed;
    case 11:
      return darkGreen;
    case 12:
      return darkBlue;
    case 13:
      return darkCyan;
    case 14:
      return darkMagenta;
    case 15:
      return darkYellow;
    }
}

void CAnnoEdit::setColor(QColor v)
{
  if (v == white)
    colorbox->setCurrentItem(0);
  else if (v == darkGray)
    colorbox->setCurrentItem(1);
  else if (v == gray)
    colorbox->setCurrentItem(2);
  else if (v == lightGray)
    colorbox->setCurrentItem(3);
  else if (v == red)
    colorbox->setCurrentItem(4);
  else if (v == green)
    colorbox->setCurrentItem(5);
  else if (v == blue)
    colorbox->setCurrentItem(6);
  else if (v == cyan)
    colorbox->setCurrentItem(7);
  else if (v == magenta)
    colorbox->setCurrentItem(8);
  else if (v == yellow)
    colorbox->setCurrentItem(9);
  else if (v == darkRed)
    colorbox->setCurrentItem(10);
  else if (v == darkGreen)
    colorbox->setCurrentItem(11);
  else if (v == darkBlue)
    colorbox->setCurrentItem(12);
  else if (v == darkCyan)
    colorbox->setCurrentItem(13);
  else if (v == darkMagenta)
    colorbox->setCurrentItem(14);
  else if (v == darkYellow)
    colorbox->setCurrentItem(15);
  else
    colorbox->setCurrentItem(0);
}

CAnnoEdit::CAnnoEdit(QWidget *parent, const char *name, WFlags f) :
  QWidget(parent, name, f)
{
  QVBoxLayout* grid = new QVBoxLayout(this);
  m_name = new QLineEdit(this, "Name");
  m_anno = new QMultiLineEdit(this, "Annotation");
  m_anno->setWordWrap(QMultiLineEdit::WidgetWidth);
  QPushButton* exitButton = new QPushButton("Okay", this);
  connect(exitButton, SIGNAL( clicked() ), this, SLOT( slotOkay() ) );
  QPushButton* cancelButton = new QPushButton("Cancel", this);
  connect(cancelButton, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
  QLabel *l = new QLabel("Text",this);
  grid->addWidget(l);
  grid->addWidget(m_name);
  l = new QLabel("Annotation",this);
  grid->addWidget(l);
  grid->addWidget(m_anno,1);

  colorbox = new QComboBox( this );

  colorbox->insertItem("None");
  colorbox->insertItem("Dark Gray");
  colorbox->insertItem("Gray");
  colorbox->insertItem("Light Gray");
  colorbox->insertItem("Red");
  colorbox->insertItem("Green");
  colorbox->insertItem("Blue");
  colorbox->insertItem("Cyan");
  colorbox->insertItem("Magenta");
  colorbox->insertItem("Yellow");
  colorbox->insertItem("Dark Red");
  colorbox->insertItem("Dark Green");
  colorbox->insertItem("Dark Blue");
  colorbox->insertItem("Dark Cyan");
  colorbox->insertItem("Dark Magenta");
  colorbox->insertItem("Dark Yellow");
  QHBoxLayout* hgrid = new QHBoxLayout(grid);
  hgrid->addWidget(colorbox);
  hgrid->addWidget(cancelButton);
  hgrid->addWidget(exitButton);
}
