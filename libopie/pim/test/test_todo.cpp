#include <qdatetime.h>
#include <qmap.h>

#include <opie/otodo.h>
#include <opie/otodoaccess.h>
#include <opie/otodoaccessxml.h>

int main(int argc, char* argv[] ) {

    OTodoAccessXML* xml = new OTodoAccessXML("demo");
    OTodoAccess ac(xml);
    int elc;
    QTime timer;
    qWarning("start loading");
    timer.start();
    ac.load();
    elc = timer.elapsed();
    qWarning("Elapsed loading %d %d", elc, elc/1000);

    timer.start();
    OTodoAccess::List lis = ac.allRecords();
    elc = timer.elapsed();
    qWarning("Elapsed allRecords %d %d", elc, elc/1000 );
    OTodoAccess::List::Iterator it;
    timer.start();
ac.save();
/*
    for( it = lis.begin(); it != lis.end(); ++it ) {
        qWarning("Desc: " + (*it).summary() );
        qWarning("UID %d", (*it).uid() );
    }
    */
elc = timer.elapsed();

 qWarning("elapsed iterating %d %d", elc, elc/1000 );
};
