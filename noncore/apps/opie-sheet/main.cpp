/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#include <qpe/qpeapplication.h>

#include "mainwindow.h"

int main(int argc, char **argv)
{
  QPEApplication application(argc, argv);

  MainWindow windowMain;
  windowMain.setHelpFile(application.qpeDir()+"/help/html/"+QString(argv[0])+".html");
  application.showMainDocumentWidget(&windowMain);

  return application.exec();
}
