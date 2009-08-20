#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>

class CBkmkSelectorItem : public QListBoxText
{
  int m_ref;
 public:
  CBkmkSelectorItem(const QString& _t, int ref) : QListBoxText(_t), m_ref(ref)
    {
    }
  int reference() { return m_ref; }
};

class CBkmkSelector : public QWidget
{

  Q_OBJECT

  QListBox* bkmkselector;
  QPushButton* exitButton;
  /*
  void keyPressEvent ( QKeyEvent * e )
    {
      if ((e->key() == Key_Return) || (e->key() == Key_Space))
	{
	  emit selected(reinterpret_cast<CBkmkSelectorItem*>(bkmkselector->item(bkmkselector->currentItem()))->reference());
	  e->accept();
	}
      else
	{
	  e->ignore();
	}
    }
  */
signals:
  void selected(int i);
  void cancelled();
private slots:
  void slotSelected(QListBoxItem* t)
  {
    if (t != NULL)
      {
	emit selected(reinterpret_cast<CBkmkSelectorItem*>(t)->reference());
      }
  }
//void slotSelected(int t) { emit selected(t); }
  void slotCancel() { emit cancelled(); }
  void slotSort()
    {
      bkmkselector->sort();
#ifdef USEQPE
      setCurrentItem(bkmkselector->currentItem());
#endif
    }
public:
  CBkmkSelector( QWidget *parent=0, const char *name=0, WFlags f = 0) :
	QWidget(parent, name, f)
  {

//      QFont f("unifont", 16);
//      setFont( f );

    QVBoxLayout* grid = new QVBoxLayout(this);
    QHBoxLayout* hgrid = new QHBoxLayout();
    bkmkselector = new QListBox(this, tr("Bookmarks"));
    QPushButton* _sort = new QPushButton(tr("Sort"), this);
    connect(_sort, SIGNAL(clicked()), this, SLOT( slotSort() ) );
    exitButton = new QPushButton(tr("Cancel"), this);
    //    connect(bkmkselector, SIGNAL( selected(int) ), this, SLOT( slotSelected(int) ) );
    connect(bkmkselector, SIGNAL( clicked(QListBoxItem*) ), this, SLOT( slotSelected(QListBoxItem*) ) );
    connect(bkmkselector, SIGNAL( returnPressed(QListBoxItem*) ), this, SLOT( slotSelected(QListBoxItem*) ) );
    connect(exitButton, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    grid->addWidget(bkmkselector,1);
    grid->addLayout(hgrid);
    hgrid->addWidget(_sort);
    hgrid->addWidget(exitButton);
  }
  void clear() { bkmkselector->clear(); }
  void insertItem(const QString& _item, int ref)
    {
      CBkmkSelectorItem* item = new CBkmkSelectorItem(_item, ref);
      bkmkselector->insertItem(item);
    }
  QString text(int index) const { return bkmkselector->text(index); }
  void setText(const QString& _l) { exitButton->setText(_l); }
  void setCurrentItem(int _i) { bkmkselector->setCurrentItem(_i); }
};
