/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qdialog.h>

class DocLnk;
class FileSelector;

class ImportDialog : public QDialog
{ 
    Q_OBJECT

public:
    ImportDialog(QWidget* parent = 0, const char* name = 0);
    ~ImportDialog();

    const DocLnk* selected();

private:
    FileSelector* m_pFileSelector;
};

#endif // IMPORTDIALOG_H
