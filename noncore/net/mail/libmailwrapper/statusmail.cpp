#include "statusmail.h"

#include <opie2/odebug.h>

using namespace Opie::Core;

StatusMail::StatusMail(QList<Account>&list)
{
    currentImapStat.message_count=0;
    currentImapStat.message_unseen=0;
    currentImapStat.message_recent=0;
    lastPop3Stat = currentImapStat;
    currentPop3Stat = currentImapStat;
    connectionList.setAutoDelete(true);
    connectionList.clear();
    initAccounts(list);
}

StatusMail::~StatusMail()
{
}

void StatusMail::initAccounts(QList<Account>&accounts)
{

    Account *it;
    folderStat currentStat;
    AbstractMail * current = 0;
    currentPop3Stat.message_count=0;
    currentPop3Stat.message_recent=0;
    currentPop3Stat.message_unseen=0;
    for ( it = accounts.first(); it; it = accounts.next() ) {
        if ( it->getType()==MAILLIB::A_IMAP && !it->getOffline() ) {
            IMAPaccount*ima = static_cast<IMAPaccount *>(it);
            current = AbstractMail::getWrapper(ima);
            connectionList.append(current);
            current->statusFolder(currentStat);
            currentImapStat.message_count+=currentStat.message_unseen;
            currentImapStat.message_count+=currentStat.message_recent;
            currentImapStat.message_count+=currentStat.message_count;
        } else if ( it->getType() == MAILLIB::A_POP3 && !it->getOffline() ) {
            POP3account *pop3 = static_cast<POP3account *>(it);
            current = AbstractMail::getWrapper(pop3);
            connectionList.append(current);
            current->statusFolder(currentStat);
            currentPop3Stat.message_count+=currentStat.message_count;
        }
        current->logout();
    }
    odebug << "Pop3 init count: " << currentPop3Stat.message_count << "" << oendl; 
    currentPop3Stat.message_recent = currentPop3Stat.message_unseen = 0;
    lastPop3Stat.message_unseen = currentPop3Stat.message_unseen;
    lastPop3Stat.message_recent = currentPop3Stat.message_recent;
    lastPop3Stat.message_count = currentPop3Stat.message_count;
}

void StatusMail::reset_status()
{
    lastPop3Stat = currentPop3Stat;
}

void StatusMail::check_current_stat(folderStat&targetStat)
{
    AbstractMail*it = 0;
    folderStat currentStat;
    currentPop3Stat.message_recent = 0;
    currentPop3Stat.message_count = 0;
    currentPop3Stat.message_unseen = 0;
    currentImapStat = currentPop3Stat;
    for ( it = connectionList.first(); it; it = connectionList.next() ) {
        it->statusFolder(currentStat);
        it->logout();
        if (it->getType() == MAILLIB::A_IMAP) {
            currentImapStat.message_unseen+=currentStat.message_unseen;
            currentImapStat.message_recent+=currentStat.message_recent;
            currentImapStat.message_count+=currentStat.message_count;
        } else if (it->getType() == MAILLIB::A_POP3) {
            currentPop3Stat.message_count+=currentStat.message_count;
            odebug << "Pop3 count: " << currentPop3Stat.message_count << "" << oendl; 
        }
    }
    odebug << "Pop3 last: " << lastPop3Stat.message_count << "" << oendl; 
    if (currentPop3Stat.message_count > lastPop3Stat.message_count) {
        currentPop3Stat.message_recent = currentPop3Stat.message_count - lastPop3Stat.message_count;
        currentPop3Stat.message_unseen = currentPop3Stat.message_recent;
    } else {
        lastPop3Stat.message_count = currentPop3Stat.message_count;
        currentPop3Stat.message_recent = currentPop3Stat.message_unseen = 0;
    }
    targetStat = currentImapStat;
    targetStat.message_unseen+=currentPop3Stat.message_unseen;
    targetStat.message_recent+=currentPop3Stat.message_recent;
    targetStat.message_count+=currentPop3Stat.message_count;
}
