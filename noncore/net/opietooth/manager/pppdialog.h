/* $Id: pppdialog.h,v 1.8 2006-04-05 19:29:20 korovkin Exp $ */
/* PPP/rfcomm connection dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PPPDIALOG_H
#define PPPDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;
class QComboBox;
#define NCONNECTS 10 //Maximal

namespace OpieTooth {
    typedef struct {
      Opie::Core::OProcess proc; //Connection process
      QString btAddr; //MAC address
      int port; //port
    } Connection;

    class PPPDialog : public QDialog {

        Q_OBJECT

    public:
        PPPDialog(const QString& device = 0, int port = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~PPPDialog();

    private slots:
        void connectToDevice();
        void fillOutPut( Opie::Core::OProcess* pppDial, char* cha, int len );
        void fillErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);
        void closeEvent(QCloseEvent* e);
    public:
        //Array of connections indexed by rfcomm device number
        static Connection conns[NCONNECTS];
    protected:
        QVBoxLayout* layout;
        QComboBox* cmdLine;
        QPushButton* connectButton;
        QMultiLineEdit* outPut;
        QComboBox* serPort;

    private:
        QString m_device;
        int m_port;
    };
}
#endif
