/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/

class QStringList;

#include <qmainwindow.h>

class Oxygen : public QMainWindow
{
    Q_OBJECT
	
  public:
    Oxygen(QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    static QString appName() { return QString::fromLatin1("oxygen"); }
    
 private:
    void loadNames();
    QStringList names;
};
