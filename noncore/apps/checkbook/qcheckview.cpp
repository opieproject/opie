#include "qcheckview.h"

#include <qpe/resource.h>
#include <qregexp.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qheader.h>

QCheckView::QCheckView(QWidget *parent, QString filename)
  : QCheckViewBase(parent)
{
  tblTransactions->setHScrollBarMode( QTable::AlwaysOff );
  tblTransactions->setNumRows( 0 );
  tblTransactions->setNumCols( 4 );
  tblTransactions->setShowGrid( FALSE );
  tblTransactions->horizontalHeader()->setLabel(0, "Num", 29);
  tblTransactions->horizontalHeader()->setLabel(1, "Description", 81);
  tblTransactions->horizontalHeader()->setLabel(2, "Date", 57);
  tblTransactions->horizontalHeader()->setLabel(3, "Amount", 59);
  tblTransactions->verticalHeader()->hide();
  tblTransactions->setLeftMargin( 0 );
  tblTransactions->setSelectionMode(QTable::NoSelection);

  m_filename = filename;
  load(filename);
}

void QCheckView::deleteClicked(int row, int col)
{
  QStringList existing;
  QString rowText = tblTransactions->text(row, 0);
  config->setGroup(rowText);
  QString originalamount = config->readEntry("Amount", "0.00");

  config->setGroup("Data");
  int lastCheck = config->readNumEntry("LastCheck", 0);

  qWarning(rowText);
  config->setGroup(rowText);
  QString payment = config->readEntry("Payment", "true");
  if ( payment == QString("true") && rowText.toInt() <= lastCheck)
  {
    for(int i = row; i != (lastCheck); i++)
    {
      config->setGroup(tblTransactions->text(i, 0));
      QString ibalance = config->readEntry("Balance", "0.00");
      // this adds the old amount on to the transaction and then takes the new amount away
      QString newbalance = calculator(ibalance, originalamount, false);
      config->writeEntry("Balance", newbalance);

      if (i == (lastCheck - 1))
      {
        config->setGroup("Totals");
        config->writeEntry("Balance", newbalance);
        break;
      }
    }
    QString category = config->readEntry("Category", "Error");
    if (category != QString("Error"))
    {
      config->setGroup("Totals");
      config->writeEntry("Spent", calculator(config->readEntry("Spent", QString("0")), originalamount, true));
      QString categorytotal = config->readEntry(category, QString("0"));
      categorytotal = calculator(categorytotal, originalamount, true);
      config->writeEntry(category, categorytotal);
    }
  }
  if ( payment == QString("false") && rowText.toInt() <= lastCheck)
  {
    for(int i = row; i != (lastCheck); i++)
    {
      config->setGroup(tblTransactions->text(i, 0));
      QString ibalance = config->readEntry("Balance", "0.00");
      // this subtracts the old amount on to the transaction and then adds the new amount on
      QString newbalance = calculator(ibalance, originalamount, true);
      config->writeEntry("Balance", newbalance);

      if (i == lastCheck - 1)
      {
        config->setGroup("Totals");
        config->writeEntry("Balance", newbalance);
        break;
      }
    }
    config->setGroup("Totals");
    config->writeEntry("Deposited", calculator(config->readEntry("Deposited", QString("0")), originalamount, true));
  }
  for (int i = rowText.toInt(); i != lastCheck; i++)
  {
    qWarning(QString::number(i +1));
    config->setGroup(QString::number(i +1));
    QString origamount = config->readEntry("Amount", "0");
    qWarning(origamount);
    QString origbalance = config->readEntry("Balance", "0");
    QString origchecknumber = config->readEntry("CheckNumber", "0");
    QString origcomments = config->readEntry("Comments", "");
    QString origdate = config->readEntry("Date", "01/01/2000");
    QString origdescription = config->readEntry("Description", "No Description");
    QString origpayment = config->readEntry("Payment", "true");
    QString origtransactionfee = config->readEntry("TransactionFee", "0");
    QString origtype = config->readEntry("Type", "0");

    if (config->hasKey("Category"))
    {
      QString origcategory = config->readEntry("Category", "No Category");
      config->removeGroup();
      config->setGroup(QString::number(i));
      config->clearGroup();
      config->writeEntry("Category", origcategory);
    } else {
      config->removeGroup();
      config->setGroup(QString::number(i));
      config->clearGroup();
    }
    config->writeEntry("Amount", origamount);
    config->writeEntry("Balance", origbalance);
    config->writeEntry("CheckNumber", origchecknumber);
    config->writeEntry("Comments", origcomments);
    config->writeEntry("Date", origdate);
    config->writeEntry("Description", origdescription);
    config->writeEntry("Payment", origpayment);
    config->writeEntry("TransactionFee", origtransactionfee);
    config->writeEntry("Type", origtype);
  }
  tblTransactions->clearCell(row, col);
  labelBalance->setText(QString("$" + config->readEntry("Balance", QString("0.00"))));
  config->setGroup("Data");
  config->writeEntry("LastCheck", QString::number(QString(config->readEntry("LastCheck", 0)).toInt() -1));
  config->write();
  delete qcd;
  emit reload(m_filename);

}

void QCheckView::load(const QString filename)
{
  config = new Config(filename, Config::File);

  connect(tblTransactions, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(tableClicked(int, int, int, const QPoint &)));

  config->setGroup("Totals");
  labelBalance->setText(QString("$" + config->readEntry("Balance", QString("0.00"))));

  config->setGroup("Data");
  int lastCheck = config->readNumEntry("LastCheck", 1);
  for (int i = 1; i != (lastCheck + 1); i++)
  {
    config->setGroup(QString::number(i));
    QString item = config->readEntry("Description", QString("Not Found"));
    if (item != "Not Found")
    {
      QTableItem *qti = new QTableItem(tblTransactions, QTableItem::Never, QString::number(i));
      QTableItem *qti1 = new QTableItem(tblTransactions, QTableItem::Never, config->readEntry("Description", QString("None")));
      QTableItem *qti2 = new QTableItem(tblTransactions, QTableItem::Never, config->readEntry("Date", QString("None")));
      QTableItem *qti3 = new QTableItem(tblTransactions, QTableItem::Never, QString("$" + config->readEntry("Amount", QString("0.00"))));
      int row = tblTransactions->numRows();
      tblTransactions->setNumRows(row + 1);
      tblTransactions->setItem(row,0, qti);
      tblTransactions->setItem(row,1, qti1);
      tblTransactions->setItem(row,2, qti2);
      tblTransactions->setItem(row,3, qti3);
      tblTransactions->updateCell(row, 0);
      tblTransactions->updateCell(row, 1);
      tblTransactions->updateCell(row, 2);
      tblTransactions->updateCell(row, 3);
    }
  }
}

void QCheckView::editClicked(int row, int col)
{
  delete qcd;
  QStringList existing;
  QString rowText = tblTransactions->text(row, 0);
  config->setGroup("Data");
  QString recent = config->readEntry("Recent", "");

  config->setGroup(rowText);
  //We need the original amount to add or subtract to check's blanaces written after this edited check
  QString originalamount = config->readEntry("Amount", "0.00");
  QString originalcategory = config->readEntry("Category", "None");

  existing << config->readEntry("Payment", "true") << config->readEntry("Description", "No Description Found") << config->readEntry("Category", "Misc.") << config->readEntry("Type", "0") << config->readEntry("CheckNumber", "0") << originalamount << config->readEntry("TransactionFee", "") << config->readEntry("Date", "01/01/2001") << config->readEntry("Comments", "") << recent;
  QStringList result = QCheckEntry::popupEntry(existing);
  if (result.isEmpty() == false)
  {
    config->setGroup("Data");
    int lastCheck = config->readNumEntry("LastCheck", 0);
    config->writeEntry("Recent", result[9]);

    config->setGroup(rowText);

    tblTransactions->setText(row, 1, result[1]);
    tblTransactions->updateCell(row, 1);

    tblTransactions->setText(row, 2, result[7]);
    tblTransactions->updateCell(row, 2);

    tblTransactions->setText(row, 3, QString("$" + result[5]));
    tblTransactions->updateCell(row, 3);

    // This is how the list looks:
    // 0: true or false, true == payment, false == deposit
    // 1: description of the transaction
    // 2: category name
    // 3: transaction type (stores the integer value of the index of the combobox)
    // 4: check number of the transaction (if any)
    // 5: transaction amount
    // 6: transaction fee (e.g. service charge, or ATM charge).
    // 7: date of the transaction
    // 8: additional comments
    config->writeEntry("Payment", result[0]);
    config->writeEntry("Description", result[1]);
    if (result[0] == QString("true"))
    {
      config->writeEntry("Category", result[2]);
    }
    config->writeEntry("Type", result[3]);
    config->writeEntry("CheckNumber", result[4]);
    config->writeEntry("Amount", result[5]);
    config->writeEntry("TransactionFee", result[6]);
    config->writeEntry("Date", result[7]);
    config->writeEntry("Comments", result[8]);
    if (result[0] == QString("true"))
    {
      if (rowText.toInt() <= lastCheck)
      {
        for(int i = (rowText.toInt() - 1); i != (lastCheck); i++)
        {
          config->setGroup(tblTransactions->text(i, 0));
          QString ibalance = config->readEntry("Balance", "0.00");

          // this adds the old amount on to the transaction and then takes the new amount away
          QString newbalance = calculator(calculator(ibalance, originalamount, false), result[5], true);
          config->writeEntry("Balance", newbalance);
          if (i == (lastCheck - 1))
          {
            config->setGroup("Totals");
            config->writeEntry("Balance", newbalance);
            break;
          }
        }
      }
      config->setGroup("Totals");
      config->writeEntry("Spent", calculator(config->readEntry("Spent", QString("0")), originalamount, true));
      config->writeEntry("Spent", calculator(config->readEntry("Spent", QString("0")), result[5], false));

      if (result[2] == originalcategory)
      {
        QString categorytotal = config->readEntry(result[2], QString("0"));
        categorytotal = calculator(categorytotal, originalamount, true);
        categorytotal = calculator(categorytotal, result[5], false);
        config->writeEntry(result[2], categorytotal);
      } else {
        QString origtotal = config->readEntry(originalcategory, QString("0"));
        QString origfinal = calculator(origtotal, result[5], true);
        if (origfinal == "0" || origfinal == "0.00")
        {
          config->removeEntry(originalcategory);
        } else {
          config->writeEntry(originalcategory, origfinal);
        }
        QString categorytotal = config->readEntry(result[2], QString("0"));
        categorytotal = calculator(categorytotal, originalamount, false);
        config->writeEntry(result[2],categorytotal);
      }
    }
    if (result[0] == QString("false"))
    {
      if (rowText.toInt() <= lastCheck)
      {
        for(int i = (rowText.toInt() - 1 ); i != (lastCheck); i++)
        {
          config->setGroup(tblTransactions->text(i, 0));
          QString ibalance = config->readEntry("Balance", "0.00");

          // this subtracts the old amount on to the transaction and then adds the new amount on
          QString newbalance = calculator(calculator(ibalance, originalamount, true), result[5], false);
          config->writeEntry("Balance", newbalance);
          if (i == lastCheck - 1)
          {
            config->setGroup("Totals");
            config->writeEntry("Balance", newbalance);
            break;
          }
        }
      }
      config->setGroup("Totals");
      config->writeEntry("Deposited", calculator(config->readEntry("Deposited", QString("0")), originalamount, true));
      config->writeEntry("Deposited", calculator(config->readEntry("Deposited", QString("0")), result[5], false));
    }
    labelBalance->setText(QString("$" + config->readEntry("Balance", QString("0.00"))));
    config->write();
  }
}
void QCheckView::tableClicked(int row, int col, int mouseButton, const QPoint &mousePos)
{
  if (tblTransactions->text(row, col).isEmpty() == false)
  {
    QStringList existing;
    config->setGroup(tblTransactions->text(row, 0));
    existing << config->readEntry("Payment", "true") << config->readEntry("Description", "No Description Found") << config->readEntry("Category", "Misc.") << config->readEntry("Type", "0") << config->readEntry("CheckNumber", "0") << config->readEntry("Amount", "0.00") << config->readEntry("TransactionFee", "0.00") << config->readEntry("Date", "01/01/2001") << config->readEntry("Comments", "") << config->readEntry("Balance", "0.00");
    qcd = new QCheckDetails(row, col, existing);
    qcd->showMaximized();
    connect(qcd, SIGNAL(editClicked(int, int)), this, SLOT(editClicked(int, int)));
    connect(qcd, SIGNAL(deleteClicked(int, int)), this, SLOT(deleteClicked(int, int)));
  }
}

void QCheckView::newClicked()
{
  config->setGroup("Data");
  QString recent = config->readEntry("Recent", "");
  QStringList kindablank;
  kindablank << recent;
  QStringList result = QCheckEntry::popupEntry(kindablank);
  if (result.count() > 1)
  {
    QTableItem *qti = new QTableItem(tblTransactions, QTableItem::Never, result[1]);
    int row = tblTransactions->numRows();
    tblTransactions->setNumRows(row + 1);
    tblTransactions->setItem(row,1, qti);
    tblTransactions->updateCell(row, 1);
    config->setGroup("Data");
    config->writeEntry("Recent", result[9]);
    int lastCheck = config->readNumEntry("LastCheck", 0);
    if (lastCheck == 0)
    {
      config->writeEntry("LastCheck", 1);
    } else {
      config->writeEntry("LastCheck", (lastCheck + 1));
    }
    QString number = QString::number(lastCheck + 1);
    config->setGroup(number);

    QTableItem *qti1 = new QTableItem(tblTransactions, QTableItem::Never, number);
    tblTransactions->setItem(row, 0, qti1);
    tblTransactions->updateCell(row, 0);

    QTableItem *qti2 = new QTableItem(tblTransactions, QTableItem::Never, result[7]);
    tblTransactions->setItem(row, 2, qti2);
    tblTransactions->updateCell(row, 2);

    QTableItem *qti3 = new QTableItem(tblTransactions, QTableItem::Never, QString("$" + result[5]));
    tblTransactions->setItem(row, 3, qti3);
    tblTransactions->updateCell(row, 3);

    // This is how the list looks:
    // 0: true or false, true == payment, false == deposit
    // 1: description of the transaction
    // 2: category name
    // 3: transaction type (stores the integer value of the index of the combobox)
    // 4: check number of the transaction (if any)
    // 5: transaction amount
    // 6: transaction fee (e.g. service charge, or ATM charge).
    // 7: date of the transaction
    config->writeEntry("Payment", result[0]);
    config->writeEntry("Description", result[1]);
    if (result[0] == QString("true"))
    {
      config->writeEntry("Category", result[2]);
    }
    config->writeEntry("Type", result[3]);
    config->writeEntry("CheckNumber", result[4]);
    config->writeEntry("Amount", result[5]);
    config->writeEntry("TransactionFee", result[6]);
    config->writeEntry("Date", result[7]);
    config->writeEntry("Comments", result[8]);
    config->setGroup("Totals");
    if (result[0] == QString("true"))
    {
      QString totalspent = config->readEntry("Spent", QString("0"));
      totalspent = calculator(totalspent, result[5], false);
      config->writeEntry("Spent", totalspent);
      QString balance = config->readEntry("Balance", QString("0"));
      balance = calculator(balance, result[5], true);

      // Make sure to add the fee on, if any
      balance = calculator(balance, result[6], true);

      config->writeEntry("Balance", balance);

      config->setGroup(number);
      config->writeEntry("Balance", balance);

      config->setGroup("Totals");
      QString categorytotal = config->readEntry(result[2], QString("0"));
      categorytotal = calculator(categorytotal, result[5], false);
      config->writeEntry(result[2], categorytotal);
    }
    if (result[0] == QString("false"))
    {
      QString totaldeposited = config->readEntry("Deposited", QString("0"));
      totaldeposited = calculator(totaldeposited, result[5], false);
      config->writeEntry("Deposited", totaldeposited);
      QString balance = config->readEntry("Balance", QString("0"));
      balance = calculator(balance, result[5], false);

      // Make sure to add the fee on, if any
      balance = calculator(balance, result[6], true);

      config->writeEntry("Balance", balance);

      config->setGroup(number);
      config->writeEntry("Balance", balance);
    }
  }
  config->setGroup("Totals");
  labelBalance->setText(QString("$" + config->readEntry("Balance", QString("0.00"))));
  config->write();
}

QString QCheckView::calculator(QString largervalue, QString smallervalue, bool subtract)
{
  // This class provides a way to eliminate the ARM processor problem of not being able to handle
  // decimals. I just take the two QString'ed numbers and find the decimal point, then I remove the decimal
  // point seperating the number into two. Then I convert it to cents (times it times 100) and add/
  // substract the two together. Put the decimals back in, and return.

  largervalue = largervalue.remove(largervalue.find(".", 0, false), 1);
  smallervalue = smallervalue.remove(smallervalue.find(".", 0, false), 1);

  int largercents = largervalue.toInt();
  int smallercents = smallervalue.toInt();

  int finalammount = 0;

  if (subtract == true)
  {
    finalammount = largercents - smallercents;
  } else {
    finalammount = largercents + smallercents;
  }

  QString returnvalue = QString::number(finalammount);
  if (returnvalue.length() >= 2)
  {
    returnvalue.insert((returnvalue.length() - 2), ".");
  } else {
    if (returnvalue.length() == 1)
    {
      returnvalue.prepend("0.0");
    }
  }
  return returnvalue;
}
