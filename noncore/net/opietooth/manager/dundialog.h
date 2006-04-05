/* $Id: dundialog.h,v 1.4 2006-04-05 19:29:20 korovkin Exp $ */
/* DUN connection dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef DUNDIALOG_H
#define DUNDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;
class QCheckBox;
class QComboBox;

namespace OpieTooth {
    class DunDialog : public QDialog {

        Q_OBJECT

    public:
        DunDialog(const QString& device = 0, int port = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~DunDialog();

    private slots:
        void connectToDevice();
        void fillOutPut( Opie::Core::OProcess* pppDial, char* cha, int len );
        void fillErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);
        void closeEvent(QCloseEvent* e);
    protected:
        QVBoxLayout* layout;
        QComboBox* cmdLine;
        QPushButton* connectButton;
        QMultiLineEdit* outPut;
        QCheckBox* doEncryption;
        QCheckBox* persist;

    private:
        QString m_device; //device BT address
        int m_port; //device process
        Opie::Core::OProcess* m_dunConnect; //DUN process
    };
}
#endif
