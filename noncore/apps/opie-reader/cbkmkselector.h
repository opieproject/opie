#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>

class CBkmkSelector : public QWidget
{

  Q_OBJECT

  QListBox* bkmkselector;
  QPushButton* exitButton;

signals:
  void selected(int i);
  void cancelled();
private slots:
  void slotSelected(QListBoxItem* t) { emit selected(bkmkselector->index(t)); }
  void slotSelected(int t) { emit selected(t); }
  void slotCancel() { emit cancelled(); }
public:
  CBkmkSelector( QWidget *parent=0, const char *name=0, WFlags f = 0) :
	QWidget(parent, name, f)
  {

//      QFont f("unifont", 16);
//      setFont( f );

    QVBoxLayout* grid = new QVBoxLayout(this);
    bkmkselector = new QListBox(this, "Bookmarks");
    exitButton = new QPushButton("Cancel", this);
    connect(bkmkselector, SIGNAL( selected(int) ), this, SLOT( slotSelected(int) ) );
    connect(bkmkselector, SIGNAL( clicked(QListBoxItem*) ), this, SLOT( slotSelected(QListBoxItem*) ) );
    connect(exitButton, SIGNAL( released() ), this, SLOT( slotCancel() ) );
    grid->addWidget(bkmkselector,1);
    grid->addWidget(exitButton);
  }
  void clear() { bkmkselector->clear(); }
  void insertItem(const QString& item) { bkmkselector->insertItem(item); }
  QString text(int index) const { return bkmkselector->text(index); }
  void setText(const QString& _l) { exitButton->setText(_l); }
};

