#ifndef CURRENCY_H
#define CURRENCY_H

#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qlayout.h>

class Currency : public QWidget
  {
    Q_OBJECT

    public:

      Currency ( QWidget *parent );
      QLabel *currencylabel;
      QComboBox *currencybox;
      QBoxLayout *layout;
  };

#endif

