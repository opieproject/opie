#ifndef OPIE_CONSOLE_CONFIG_WIDGET_H
#define OPIE_CONSOLE_CONFIG_WIDGET_H

#include "profiledialogwidget.h"

class QVBoxLayout;
class QLineEdit;
class QListView;
class QPushButton;

class ConsoleConfigWidget : public ProfileDialogConnectionWidget {
    Q_OBJECT
public:
    ConsoleConfigWidget( const QString& name, QWidget* parent, const char* name = 0l );
    ~ConsoleConfigWidget();

    void load( const Profile& );
    void save( Profile& );

protected slots:
    void slotAdd();
    void slotRemove();
private:
    QVBoxLayout *m_lay;
    QLineEdit *m_cmd;
    QLineEdit *m_name;
    QLineEdit *m_value;
    QListView *m_env;
    QPushButton *m_add;
    QPushButton *m_remove;
};


#endif
