#ifndef QASHMONEY_H
#define QASHMONEY_H

#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qdatetime.h>

#include "accountdisplay.h"
#include "transactiondisplay.h"
#include "budgetdisplay.h"
#include "account.h"
#include "preferences.h"
#include "transaction.h"
#include "transfer.h"
#include "memory.h"
#include "budget.h"

class QashMoney : public QWidget
  {
    Q_OBJECT

    public:
      QashMoney ();
      ~QashMoney();

      QTabWidget* maintabs;
      QWidget* tab;
      QWidget* tab_2;
      QWidget* tab_3;

      QMenuBar *mainmenu;
      QPopupMenu *preferencesmenu;
      QPopupMenu *utilitiesmenu;

    public slots:
      void displayDatePreferencesDialog ();
      void displayTransactionPreferencesDialog ();
      void displayAccountPreferencesDialog ();
      void displayMemoryDialog ();
      void setTransactionTab ();

    private slots:
      void changeTabDisplay ();
      void showTransactions ();
      void enableOneTouchViewing ();
      void disableOneTouchViewing ();
      void toggleOneTouchViewing ( bool );
      void setTransactionDisplayDate ();

    private:
      QVBoxLayout *layout;
      QVBoxLayout *tabslayout;
      AccountDisplay *accountdisplay;
      TransactionDisplay *transactiondisplay;
      BudgetDisplay *budgetdisplay;
      int tabheight;
      QDate newdate;

  };

#endif

