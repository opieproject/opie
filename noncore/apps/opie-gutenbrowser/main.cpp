
/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Jul 24 22:33:12 MDT 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gutenbrowser.h"
#include <opie2/oapplicationfactory.h>

using namespace Opie::Core;

OPIE_EXPORT_APP( OApplicationFactory<Gutenbrowser> )

// #ifdef QT_QWS_NONOPIE
		
// #include <qpe/qpeapplication.h>
// #include "gutenbrowser.h"
// #include <qstring.h>
// // #include <stdio.h>
// // #include <stdlib.h>

// int main(int argc, char *argv[]){
// 		QPEApplication a(argc, argv);
// //     char * psz;
// //     if  (argc > 1) {
// //         for( int i=0; i < argc; i++) {
// //             psz = argv[i];
// //             if( psz[0] == '-' || psz[0] == '/' ) {
// // // The next character is the option.
// //                 switch( psz[1] )
// //                     {
// //                     case 'v': // help
// //                         printf("Gutenbrowser: "VERSION"\n");
// // //                        odebug << "Gutenbrowser: "VERSION << oendl; 
// //                           //                      return 0;
// //                         break;
// //                     case 't': // current time 19:34:20 04/03/00
// // //                        return 0;
// //                         break;
// //                     case 'f': //
// // //                         file_Name=argv[i+1];
// //                         break;
// //                     }
// //             }
// //         }
// //     }
// //        QNetworkProtocol::registerNetworkProtocol("http", new QNetworkProtocolFactory<Http>);
// //          qInitNetworkProtocols();//registers ftp protocol // for now
// 		Gutenbrowser *gutenbrowser=new Gutenbrowser();
// //        a.setMainWidget(gutenbrowser);
// 		a.showMainWidget(gutenbrowser,TRUE);
// 		gutenbrowser->showMaximized();
// 		gutenbrowser->show();
// 		return a.exec();
// }


// #endif
