#include "sendmailprogress.h"
#include <qprogressbar.h>
#include <qlabel.h>
#include <qstring.h>

progressMailSend::progressMailSend(QWidget*parent, const char * name)
    :progressMailSendUI(parent,name,false),m_current_mail(0),m_current_single(0),m_max_mail(0),m_max_single(0)
{
}

progressMailSend::~progressMailSend()
{
}

void progressMailSend::setMaxMails(unsigned int aMaxMails)
{
    m_max_mail = aMaxMails;
    allMailProgressBar->setTotalSteps(aMaxMails);
    setMails();
}

void progressMailSend::setCurrentMails(unsigned int aCurrent)
{
    m_current_mail = aCurrent;
    allMailProgressBar->setProgress(aCurrent);
    setMails();
}
    
void progressMailSend::setSingleMail(unsigned int aCurrent,unsigned int aMax)
{
    m_current_single = aCurrent;
    m_max_single = aMax;
    setSingle();
}

void progressMailSend::setSingle()
{
    QString text = QString(tr("%1 of %2 bytes send")).arg(m_current_single).arg(m_max_single);
    singleMailLabel->setText(text);
    singleMailProgressBar->setTotalSteps(m_max_single);
    singleMailProgressBar->setProgress(m_current_single);
}

void progressMailSend::setMails()
{
    QString text = QString(tr("Sending mail %1 of %2")).arg(m_current_single+1).arg(m_max_single);
    allMailLabel->setText(text);
}
