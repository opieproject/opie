#ifndef TRANSFER_H
#define TRANSFER_H

#include <qlistview.h>
#include <qstring.h>
#include <sqlite.h>
#include <qdatetime.h>

#include "preferences.h"

class Transfer
  {
    public:

      Transfer ();
      ~Transfer ();

      // This function adds a new transfer to the database.  It takes the fromaccount, toaccount,
      // number, frombudget, tobudget, day, month, year, amount, cleared
      void addTransfer ( int fromaccount, int fromparent, int toaccount, int toparent, int day, int month, int year, float amount, int cleared );
      void updateTransfer ( int fromaccount, int fromparent, int toaccount, int toparent, int day, int month, int year, float amount, int cleared, int transferid );

      // Deletes a transfer.  Takes the transferid as its parameter
      void deleteTransfer ( int );

      // Deletes all transfers for a given accountid
      void deleteAllTransfers ( int accountid );

      // Returns the number of checking transfers
      int getNumberOfTransfers ();
      int getNumberOfTransfers ( int accountid );

      void displayTransfers ( QListView *listview, int accountid, bool children, QDate displaydate );
      int getCleared ( int id );
      void setCleared ( int id, int cleared );
      int getFromAccountID ( int id );
      int getToAccountID ( int id );

      int getDay ( int id );
      int getMonth ( int id );
      int getYear ( int id );
      QString getAmount ( int id );

    private:

      sqlite *db;
  };

#endif



