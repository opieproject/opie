/* $Id: pandialog.h,v 1.2 2006-04-04 12:15:10 korovkin Exp $ */
/* PAN connection dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PANDIALOG_H
#define DUNDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>

class QVBoxLayout;
class QMultiLineEdit;
class QPushButton;
class QCheckBox;

namespace OpieTooth {
    class PanDialog : public QDialog {

        Q_OBJECT

    public:
        PanDialog(const QString& device = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~PanDialog();

    private slots:
        void connectToDevice();
        void fillOutPut( Opie::Core::OProcess* pppDial, char* cha, int len );
        void fillErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);
        void closeEvent(QCloseEvent* e);
    protected:
        QVBoxLayout* layout;
        QPushButton* connectButton;
        QMultiLineEdit* outPut;
        QCheckBox* doEncryption;
        QCheckBox* doSecure;
        Opie::Core::OProcess* m_panConnect;
        
    private:
        QString m_device; //device BT address
    };
}
#endif
