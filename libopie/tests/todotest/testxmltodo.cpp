#include <qstring.h>

#include <opie/todoevent.h>
#include <opie/tododb.h>
#include <opie/todoxmlresource.h>


int main(int argc, char *argv[] ) {

    if ( argc < 2 ) {
        qWarning("No file to open");
        return -1;
    }

    QString path = QString::fromLatin1( argv[1] );
    Opie::ToDoXMLResource* res = new Opie::ToDoXMLResource( "todotest", path );
    Opie::ToDoDB db( res );
    db.load();
    QStringList list;
    list << "-12345678";
    list << "-12354566";

    for (int i = 0; i < 10; i++ ) {
        qWarning("Test %d", i);
        Opie::ToDoEvent ev( false, Opie::ToDoEvent::NORMAL,
                            list, "Summary" + QString::number( i ),
                            "Description " + QString::number( i ),
                            50, true);
        db.addEvent(ev  );
    }
    Opie::ToDoDB::Iterator it = db.rawToDos();
    for ( ; it != db.end(); ++it ) {
        qWarning("Summary " + (*it).summary() );
    }
    db.save();

}
