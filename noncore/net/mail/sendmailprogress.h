#include "sendmailprogressui.h"

class progressMailSend:public progressMailSendUI
{
    Q_OBJECT
public:
    progressMailSend(QWidget*parent = 0, const char * name = 0);
    ~progressMailSend();

    void setMaxMails(unsigned int aMaxMails);
    void setCurrentMails(unsigned int aCurrent);
    
    void setSingleMail(unsigned int aCurrent,unsigned int aMax);

protected:
    unsigned m_current_mail,m_current_single,m_max_mail,m_max_single;
    void setSingle();
    void setMails();
};
