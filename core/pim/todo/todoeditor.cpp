
#include "todoentryimpl.h"
#include "todoeditor.h"

using namespace Todo;

Editor::Editor() {
    m_accepted = false;
}
Editor::~Editor() {
}
OTodo Editor::newTodo( int cur,
                           QWidget* par) {

    NewTaskDialog e( cur, par, 0, TRUE );


#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif

    int ret = e.exec();
    if ( QDialog::Accepted == ret ) {
        m_accepted = true;
    }else
        m_accepted = false;

    OTodo ev = e.todoEntry();
    ev.setUid(1);

    return ev;
}
OTodo Editor::edit( QWidget *wid,
                        const OTodo& todo ) {
    NewTaskDialog e( todo, wid, 0, TRUE );
    e.setCaption( QObject::tr( "Edit Task" ) );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif
    int ret = e.exec();

    OTodo ev = e.todoEntry();
    if ( ret == QDialog::Accepted )
        m_accepted = true;
    else
        m_accepted = false;

    return ev;
}
bool Editor::accepted()const {
    return m_accepted;
}
