#ifndef TRANSACTIONDISPLAY_H
#define TRANSACTIONDISPLAY_H

#include <qlayout.h>
#include <qhbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetime.h>

class TransactionDisplay : public QWidget
  {
    Q_OBJECT

  public:
    TransactionDisplay ( QWidget* parent );

    QListView* listview;
    QPushButton* newtransaction;
    QPushButton* edittransaction;
    QPushButton* deletetransaction;
    QPushButton* toggletransaction;
    QPushButton* viewtransactionnotes;

    QLabel *name;
    QLabel *balance;
    QLineEdit *limitbox;
    QLineEdit *amount;
    QLineEdit *date;

    int getIDColumn ();

  public slots:
    void setChildren ( bool );
    void setAccountID ( int );
    void showTransactionNotes ();

  private slots:
    void addTransaction ();
    void editTransaction ();
    void editTransfer ();
    void deleteTransaction ();
    void toggleTransaction ();
    void checkListViewDelete ();
    void checkListViewEdit ();
    void checkListViewToggle ();
    void saveColumnSize ( int column, int oldsize, int newsize );
    void limitDisplay ( const QString & );
    void showCalculator ();
    void showCalendar ();
    void setTransactionDisplayDate ();

  private:
    int accountid, fromaccount, fromparent, toaccount, toparent, day, month, year, transferid;
    QDate displaydate;
    bool children;
    QBoxLayout *layout;
    QHBox *firstline;
    QHBox *secondline;
    void redisplayAccountBalance ();
    void updateAndDisplay ( int accountid );
  };

#endif

#ifndef COLORLISTITEM_H
#define COLORLISTITEM_H

class ColorListItem : public QListViewItem
  {
    public:

      ColorListItem ( QListView *parent );
      ColorListItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4 );
      ColorListItem ( QListView *parent, QString label1, QString label2, QString label3, QString label4, QString label5 );

      virtual void paintCell ( QPainter *p, const QColorGroup &cg, int column, int width, int alignment );
  };

#endif

