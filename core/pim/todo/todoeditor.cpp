
#include "otaskeditor.h"
#include "todoeditor.h"

using namespace Todo;

Editor::Editor() {
    m_accepted = false;
}
Editor::~Editor() {
}
OTodo Editor::newTodo( int cur,
                           QWidget* par) {

    OTaskEditor e( cur);


#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif

    int ret = e.exec();
    if ( QDialog::Accepted == ret ) {
        m_accepted = true;
    }else
        m_accepted = false;

    OTodo ev = e.todo();
    qWarning("Todo uid");
    qWarning("Todo %s %d %d", ev.summary().latin1(), ev.progress(), ev.isCompleted() );
    ev.setUid(1);

    return ev;
}
OTodo Editor::edit( QWidget *wid,
                        const OTodo& todo ) {
    OTaskEditor e( todo );
    e.setCaption( QObject::tr( "Edit Task" ) );

#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    e.showMaximized();
#endif
    int ret = e.exec();

    OTodo ev = e.todo();
    if ( ret == QDialog::Accepted )
        m_accepted = true;
    else
        m_accepted = false;

    return ev;
}
bool Editor::accepted()const {
    return m_accepted;
}
