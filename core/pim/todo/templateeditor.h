#ifndef OPIE_TEMPLATE_EDITOR_H
#define OPIE_TEMPLATE_EDITOR_H

#include <qobject.h>
#include <qdialog.h>
#include <qstring.h>

namespace Todo {

    class MainWindow;
    class TemplateManager;
    class TemplateEditor : public QObject{
        Q_OBJECT
    public:
        TemplateEditor( MainWindow* win,
                        TemplateManager* man);
        ~TemplateEditor();

    signals:
        void configChanged();
    private:
        void init();
        MainWindow* m_main;
        TemplateManager* m_man;
private slots:
        void setUp();

    };
};

#endif
