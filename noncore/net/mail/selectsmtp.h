#ifndef __selectsmtp_h
#define __selectstmp_h

#include <qt.h>
#include "selectstoreui.h"
#include <qlist.h>

class SMTPaccount;

class selectsmtp : public selectstoreui
{
    Q_OBJECT
public:
    selectsmtp(QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    virtual ~selectsmtp();
    void setSelectionlist(QList<SMTPaccount>*list);
    SMTPaccount*selected_smtp();

protected:
    QList<SMTPaccount>*m_smtpList;
    SMTPaccount*m_current_smtp;

protected slots:
    virtual void slotAccountselected(int);
};

#endif
