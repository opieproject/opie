/****************************************************************************
** Form implementation generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:35 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ButtonPrefs.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qbuttongroup.h>
#include <qmultilineedit.h>
#ifdef USECOMBO
#include <qcombobox.h>
#else
#include <qpe/menubutton.h>
#endif
#include <qfontdatabase.h>

#include <qlistview.h>

class MyQListViewItem : public QListViewItem
{
  orKey o;
public:
  MyQListViewItem(const orKey& _o, QListView* p, const QString& c1, const QString& c2, const QString& c3) : QListViewItem(p, c1, c2, c3), o(_o) { }
  orKey getKey() { return o; }
};

void CButtonPrefs::mapkey(Qt::ButtonState st, int _key)
{
  mapkey(st, _key,  action->currentItem());
}

void CButtonPrefs::mapkey(Qt::ButtonState st, int _key, int act)
{
  orKey key(st, _key, ((act == cesScrollMore) || (act == cesScrollLess)));
  QMap<orKey,QListViewItem*>::Iterator iter = listmap.find(key);
  if (iter != listmap.end())
    {
      lb->takeItem(iter.data());
    }
  (*kmap)[key] = act;
  listmap[key] = new MyQListViewItem(key, lb, key.text(), action->text(act), (key.isScroll()) ? "*":"");
}

void CButtonPrefs::keyPressEvent(QKeyEvent* e)
{
  switch (e->key())
    {
    case Key_Shift:
    case Key_Control:
    case Key_Meta:
    case Key_Alt:
    case Key_CapsLock:
    case Key_NumLock:
    case Key_ScrollLock:
      e->ignore();
      break;
    default:
      mapkey(e->state(), e->key());
      e->accept();
      break;
    }
}

#ifdef USECOMBO
void CButtonPrefs::populate(QComboBox *mb)
#else
void CButtonPrefs::populate(MenuButton *mb)
#endif
{
    mb->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    mb->insertItem( tr("Open file") );
    mb->insertItem( tr("Autoscroll") );
    mb->insertItem( tr("Bookmark") );
    mb->insertItem( tr("Annotate") );
    mb->insertItem( tr("Fullscreen") );

    mb->insertItem( tr("Zoom in") );
    mb->insertItem( tr("Zoom out") );
    mb->insertItem( tr("Back") );
    mb->insertItem( tr("Forward") );
    mb->insertItem( tr("Home") );
    mb->insertItem( tr("Page up") );
    mb->insertItem( tr("Page down") );
    mb->insertItem( tr("Line up") );
    mb->insertItem( tr("Line down") );
    mb->insertItem( tr("Beginning") );
    mb->insertItem( tr("End") );
    mb->insertItem( tr("Rotate") );
    mb->insertItem( tr("Scroll faster") );
    mb->insertItem( tr("Scroll slower") );
    mb->insertItem( tr("Invert colours") );
    mb->insertItem( tr("Toggle tools") );
    mb->insertItem( tr("Toggle scrollbar") );
    mb->insertItem( tr("Toggle statusbar") );
    mb->insertItem( tr("Next link") );
    mb->insertItem( tr("Goto link") );
}

CButtonPrefs::CButtonPrefs( QMap<orKey, int>* _kmap, QWidget* parent,  const char* name, WFlags fl )
  : QWidget( parent, name, fl ), kmap(_kmap)
{
  QVBoxLayout* vo = new QVBoxLayout(this);
  QHBoxLayout* lo = new QHBoxLayout();
  setFocusPolicy(QWidget::StrongFocus);
#ifdef USECOMBO
    action = new QComboBox( this );
#else
    action = new MenuButton( this );
#endif
    populate(action);
    action->setFocusProxy(this);



    QMultiLineEdit* TextLabel1 = new QMultiLineEdit( this );
    TextLabel1->setText( tr( "Press the key(s) you want assigned to the highlighted function.\n\nPress the delete button to unmap the key.\n\nUse the \"Close\" button (not the [x]) to finish." ) );
    TextLabel1->setReadOnly(true);
    TextLabel1->setWordWrap(QMultiLineEdit::WidgetWidth);

    //    lo->addWidget(TextLabel, 0, Qt::AlignTop);
    //    lo->addWidget(action, 0, Qt::AlignTop);
    lo->addWidget(TextLabel1);
    lo->addWidget(action);
    vo->addLayout(lo);
    lb = new QListView(this);
    lb->addColumn( tr( "Key" ) );
    lb->addColumn( tr( "Function" ) );
    lb->addColumn( tr( "Scroll" ) );
    lb->setFocusProxy(this);
    vo->addWidget(lb,1);
    for (QMap<orKey,int>::Iterator i = kmap->begin(); i != kmap->end(); i++)
      {
	listmap[i.key()] = new MyQListViewItem(i.key(), lb, i.key().text(), action->text(i.data()), (i.key().isScroll()) ? "*":"");
      }

    lo = new QHBoxLayout();
    QLabel* TextLabel = new QLabel( this, "TextLabel1" );
    TextLabel->setText( tr( "Debounce" ) );
    lo->addWidget(TextLabel);

    debounce = new QSpinBox( this, "Debounce" );
    debounce->setRange(0,1000);
    lo->addWidget(debounce);
    /*
    QPushButton* assignClose = new QPushButton("Assign\nCancel", this);
    lo->addWidget(assignClose);
    */
    lo->addStretch(1);
    QPushButton* delButton = new QPushButton("Delete", this);
    connect(delButton, SIGNAL( clicked() ), this, SLOT( erasemapping() ));
    lo->addWidget(delButton);
    QPushButton* exitButton = new QPushButton("Close", this);
    connect(exitButton, SIGNAL( clicked() ), this, SLOT( slotClosed() ) );
    lo->addWidget(exitButton);
    vo->addLayout(lo);
}

void CButtonPrefs::erasemapping()
{
  MyQListViewItem* li = (MyQListViewItem*)lb->selectedItem();
  if (li != NULL)
    {
      orKey key(li->getKey());
      qDebug("Tapped %s", (const char*)key.text());
      kmap->remove(key);
      lb->takeItem(listmap[key]);
      listmap.remove(key);
    }
}

CButtonPrefs::~CButtonPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}
