/*
 * stocktickerpluginwidget.cpp
 *
 * copyright   : (c) 2002 by L.J. Potter
 * email       : llornkcor@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qsocket.h>
#include <qvaluelist.h>
#include <qtl.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qobject.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtimer.h>
#include <qmessagebox.h>

#include <qpe/config.h>
#include <qpe/timestring.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/network.h>

#include <opie/oticker.h>

extern "C" {
#include "libstocks/stocks.h"
}

#include <pthread.h>

#include "stocktickerpluginwidget.h"

QString output;
OTicker *stocktickerTicker;

void getStocks( const QString *blah) {

//    stocktickerTicker->setText( "Downloading stock data.");
    stock *stocks_quotes=NULL;
    stock *stocks_tmp;
    qDebug("%s", blah->latin1());
    QString tempString;
    output = "";

    libstocks_return_code error;

    Config cfg( "stockticker");
    cfg.setGroup( "Fields" );
    bool dotimeCheck, dodateCheck, dosymbolCheck, donameCheck;
    bool docurrentPriceCheck, dolastPriceCheck, doopenPriceCheck;
    bool dominPriceCheck, domaxPriceCheck, dovariationCheck, dovolumeCheck;

    dotimeCheck=dodateCheck=dosymbolCheck=donameCheck= docurrentPriceCheck=dolastPriceCheck=doopenPriceCheck=dominPriceCheck=domaxPriceCheck=dovariationCheck=dovolumeCheck=false;

    dotimeCheck=cfg.readBoolEntry("timeCheck",1);
    dodateCheck=cfg.readBoolEntry("dateCheck",1);
    dosymbolCheck=cfg.readBoolEntry("symbolCheck",1);
    donameCheck=cfg.readBoolEntry("nameCheck",1);
    docurrentPriceCheck=cfg.readBoolEntry("currentPriceCheck",1);
    dolastPriceCheck=cfg.readBoolEntry("lastPriceCheck",1);
    doopenPriceCheck=cfg.readBoolEntry("openPriceCheck",1);
    dominPriceCheck=cfg.readBoolEntry("minPriceCheck",1);
    domaxPriceCheck=cfg.readBoolEntry("maxPriceCheck",1);
    dovariationCheck=cfg.readBoolEntry("variationCheck",1);
    dovolumeCheck=cfg.readBoolEntry("volumeCheck",1);

//    DefProxy();
 {
     char *proxy;
     libstocks_return_code error;

       /* Proxy support */
       /* Checks for "http_proxy" environment variable */
     proxy = getenv("http_proxy");
     if(proxy) {
           /* printf("proxy set\n"); */
         error = set_proxy(proxy);
         if (error) {
             printf("Proxy error (%d)\n", error);
             QString tempString;
             tempString.sprintf("Proxy error (%d)\n", error);
             output = tempString;
             return;
//             exit(1);
         }
     }
 }
    char *stock_liste = (char *)blah->latin1();
//    char *stock_liste = (char *)blah;
      /* Get the stocks and process errors */
    error = get_stocks( stock_liste, &stocks_quotes);

    if (error) {
        printf("Error in getting stocks (%d)\n", error);
        tempString.sprintf("Error in getting stocks (%d)\n", error);
        output =tempString;
        return;
//        exit(1);
    }

    stocks_tmp = stocks_quotes;

      /* Displays the stocks */
    while(stocks_tmp!=0){
        
        if (stocks_tmp->Time) {
//            printf("%s ", stocks_tmp->Time);
            tempString.sprintf("|  %s  ", stocks_tmp->Time);
            tempString.replace(QRegExp("\""),"");
            if( dotimeCheck)
                output +=tempString;
        }
        if (stocks_tmp->Date) {
//             printf("%s ", stocks_tmp->Date);
            tempString.sprintf("|  %s  ", stocks_tmp->Date);
            tempString.replace(QRegExp("\""),"");
            if(dodateCheck)
                output +=tempString;
        }
//         printf("\n");
      
//         printf("----------------------------------------\n");

        if ( strlen(stocks_tmp->Symbol) > 20 ) {
//             printf("| Symbol    | %.20s     |\n",stocks_tmp->Symbol);
            tempString.sprintf("|  Symbol %s  ",stocks_tmp->Symbol);
            if(dosymbolCheck)
                output +=tempString;
        }
        else {
//             printf("| Symbol    | %-20s |\n",stocks_tmp->Symbol);
            tempString.sprintf("|  Symbol %s  ",stocks_tmp->Symbol);
            if(dosymbolCheck)
                output +=tempString;
        }

        if (stocks_tmp->Name) {
            if ( strlen(stocks_tmp->Name) > 20 ) {
//                 printf("| Name %.20s  |\n",stocks_tmp->Name);
                tempString.sprintf("|  Name %s  ",stocks_tmp->Name);
                tempString.stripWhiteSpace();
                if(donameCheck)
                    output +=tempString;
            } else {
//                 printf("| Name      | %-20s     |\n",stocks_tmp->Name);
                tempString.sprintf("|  Name %s  ",stocks_tmp->Name);
                tempString.stripWhiteSpace();
                if(donameCheck)
                    output +=tempString;
            }
        }
        else {
//             printf("| Name      |                          |\n");
            tempString.sprintf("|  Name  | |");
            if(donameCheck)
                output +=tempString;
        }

//         printf("| Price     | %-7.2f                  |\n", stocks_tmp->CurrentPrice);
        tempString.sprintf("|  Price %-7.2f  ", stocks_tmp->CurrentPrice);
        if(docurrentPriceCheck)
            output +=tempString;

//         printf("| Yesterday | %-7.2f                  |\n",stocks_tmp->LastPrice);
        tempString.sprintf("|  Yesterday  %-7.2f  ",stocks_tmp->LastPrice);
        if(dolastPriceCheck)
            output +=tempString;

//         printf("| Open      | %-7.2f                  |\n",stocks_tmp->OpenPrice);
        tempString.sprintf("|  Open %-7.2f  ",stocks_tmp->OpenPrice);
        if(doopenPriceCheck)
            output +=tempString;

//         printf("| Min       | %-7.2f                  |\n", stocks_tmp->MinPrice);
        tempString.sprintf("|  Min %-7.2f  ", stocks_tmp->MinPrice);
        if(dominPriceCheck)
            output +=tempString;

//         printf("| Max       | %-7.2f                  |\n",stocks_tmp->MaxPrice);
        tempString.sprintf("|  Max %-7.2f  ",stocks_tmp->MaxPrice);
        if(domaxPriceCheck)
            output +=tempString;

//         printf("| Var       | %-6.2f (%5.2f %%)         |\n", stocks_tmp->Variation, stocks_tmp->Pourcentage);
        tempString.sprintf("|  Var %-6.2f (%5.2f %%)  ", stocks_tmp->Variation, stocks_tmp->Pourcentage);
        if(dovariationCheck)
                output +=tempString;

//         printf("| Volume    | %-9d                |\n", stocks_tmp->Volume);
        tempString.sprintf("|  Volume %-9d  ", stocks_tmp->Volume);
        if(dovolumeCheck)
            output +=tempString;

//         printf("----------------------------------------\n\n");
        tempString.sprintf("||==++==|");
        output +=tempString;

          /* Simple function which help to browse in the stocks list */ 
        stocks_tmp = next_stock(stocks_tmp);
    }

      /* frees stocks */
    free_stocks(stocks_quotes);
    stocktickerTicker->setText( output );

}

StockTickerPluginWidget::StockTickerPluginWidget( QWidget *parent,  const char* name)
    : QWidget(parent,  name ) {
    init();
    startTimer(1000);
//    checkConnection();
}

StockTickerPluginWidget::~StockTickerPluginWidget() {
}

void StockTickerPluginWidget::init() {

    QHBoxLayout* layout = new QHBoxLayout( this );
    stocktickerTicker = new OTicker(this);
    stocktickerTicker->setMinimumHeight(15);
    connect( stocktickerTicker, SIGNAL( mousePressed()), this, SLOT( checkConnection() ));
    layout->addWidget( stocktickerTicker);
    wasError = true;
}

void StockTickerPluginWidget::doStocks() {
    Config cfg( "stockticker");
    cfg.setGroup( "Symbols" );
    symbollist="";
    symbollist = cfg.readEntry("Symbols", "");
    symbollist.replace(QRegExp(" "),"+");//seperated by +

//    qDebug(symbollist);
    if (!symbollist.isEmpty()) {
        pthread_t thread1;
        pthread_create(&thread1,NULL, (void * (*)(void *))getStocks, &symbollist);
    }
//    pthread_join(thread1,NULL);
//    getStocks( symbollist.latin1() );
}

//void StockTickerPluginWidget::DefProxy(void) {
//     char *proxy;
//     libstocks_return_code error;

//       /* Proxy support */
//       /* Checks for "http_proxy" environment variable */
//     proxy = getenv("http_proxy");
//     if(proxy) {
//           /* printf("proxy set\n"); */
//         error = set_proxy(proxy);
//         if (error) {
// //             printf("Proxy error (%d)\n", error);
//           QString tempString;
//             tempString.sprintf("Proxy error (%d)\n", error);
//             output = tempString;
//             return;
// //             exit(1);
//         }
//     }
//}

void StockTickerPluginWidget::timerEvent( QTimerEvent *e ) {
    killTimer(e->timerId());
    checkConnection();
}

void StockTickerPluginWidget::checkConnection() {
//     qDebug("checking connection");
//     Sock = new QSocket( this );

//      if( wasError)
//          stocktickerTicker->setText("Checking connection");

//     if(Sock->state() == QSocket::Idle) {    
//         Sock->connectToHost("finance.yahoo.com", 80);
//         connect( Sock, SIGNAL( error(int) ),  SLOT(socketError(int)) );
//         connect( Sock, SIGNAL( hostFound() ), SLOT(isConnected()) );
//     } else {
//         qDebug("State is not Idle");
        isConnected();
//    }
}

void StockTickerPluginWidget::isConnected() {
//    qDebug("We connect, so ok to grab stocks");
    doStocks();

    Config cfg( "stockticker");
    cfg.setGroup("Timer");
    timerDelay= cfg.readNumEntry("Delay",0);
    if(timerDelay > 0)
        startTimer(timerDelay*60000);
    qDebug("timer set for %d",(timerDelay*60000)/60000);
    wasError = false;
        
//    Sock->close();
}

void StockTickerPluginWidget::socketError(int errcode) {
    switch(errcode) {
      case QSocket::ErrConnectionRefused:
          output = tr("Connection refused.");
          break;
      case QSocket::ErrHostNotFound:
          output = tr("Could not find server.");
          break;
      case QSocket::ErrSocketRead :
          output = tr("Socket read error.");
          break;
    };
    stocktickerTicker->setText( output );
    wasError = true;
//    Sock->close();

}
