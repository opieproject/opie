#ifndef ACCOUNTDISPLAY_H
#define ACCOUNTDISPLAY_H

#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qhbox.h>

class AccountDisplay : public QWidget
  {
    Q_OBJECT

    public:
      AccountDisplay ( QWidget *parent );

      QHBox *firstline;

      QPushButton* newaccount;
      QPushButton* editaccount;
      QPushButton* deleteaccount;
      QPushButton* transferbutton;

      QListView* listview;

      QBoxLayout *layout;

      void setTabs ( QWidget *newtab2, QTabWidget *newtabs );
      int getIDColumn ();
      void setToggleButton ();

    public slots:
      void addAccount ();
      void editAccount ();
      void deleteAccount ();
      void accountTransfer ( bool state );
      void getTransferAccounts ( QListViewItem * item );
      void disableParentsWithChildren ();
      void enableAccounts ();

    private slots:
      void saveColumnSize ( int column, int oldsize, int newsize );
      void setAccountExpanded ( QListViewItem *item );
      void setAccountCollapsed ( QListViewItem *item );

    private:
      int type, firstaccountid, secondaccountid, cleared;
      QWidget *tab2;
      QTabWidget *maintabs;
};

#endif // ACCOUNTDISPLAY_H

