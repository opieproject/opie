#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qhbox.h>
#include <qregexp.h>
#include <stdio.h>

#include "consoleconfigwidget.h"

ConsoleConfigWidget::ConsoleConfigWidget( const QString& name, QWidget* parent,
                                const char* na )
    : ProfileDialogConnectionWidget( name, parent, na ) {
    m_lay = new QVBoxLayout( this );
    QLabel *label = new QLabel(tr("Command to execute"), this);
    m_lay->addWidget(label);
    m_cmd = new QLineEdit(this);
    m_lay->addWidget(m_cmd);
    label = new QLabel(tr("Environment Variables"), this);
    m_lay->addWidget(label);
    m_env = new QListView(this);
    m_env->addColumn(tr("Name"));
    m_env->addColumn(tr("Value"));
    m_lay->addWidget(m_env);

    QHBox *hbox = new QHBox(this);
    label = new QLabel(tr("Name :"), hbox);
    m_name = new QLineEdit(hbox);
    m_lay->addWidget(hbox);

    hbox = new QHBox(this);
    label = new QLabel(tr("Value :"), hbox);
    m_value = new QLineEdit(hbox);
    m_lay->addWidget(hbox);

    hbox = new QHBox(this);
    hbox->setSpacing(10);
    m_remove = new QPushButton(tr("Remove"), hbox);
    connect(m_remove, SIGNAL(clicked()), this, SLOT(slotRemove()));
    m_add = new QPushButton(tr("Add"), hbox);
    connect(m_add, SIGNAL(clicked()), this, SLOT(slotAdd()));
    m_lay->addWidget(hbox);
}

void ConsoleConfigWidget::slotAdd() {
    if (!(m_name->text().isEmpty() || m_value->text().isEmpty())) {
        QListViewItem *item = new QListViewItem(m_env);
        item->setText(0, m_name->text());
        item->setText(1, m_value->text());
        m_env->insertItem(item);
    }
}

void ConsoleConfigWidget::slotRemove() {
    QListViewItem *item = m_env->currentItem();
    if (item) {
        m_env->takeItem(item);
    }
}

ConsoleConfigWidget::~ConsoleConfigWidget() {
}

void ConsoleConfigWidget::load( const Profile& prof ) {
    /*
     * we will use /bin/bash as default
     * but will fallback in MyPty to /bin/sh
     * if necessary
     */
    m_cmd->setText(prof.readEntry("Command", "/bin/bash"));
    int envcount = prof.readNumEntry("EnvVars", 0);
    for (int i=0; i<envcount; i++) {
        QString name = prof.readEntry("Env_Name_" + QString::number(i), "");
        QString value = prof.readEntry("Env_Value_" + QString::number(i), "");
        if (!(name.isEmpty() || value.isEmpty())) {
            QListViewItem *item = new QListViewItem(m_env);
            item->setText(0, name);
            item->setText(1, value);
            m_env->insertItem(item);
        }
    }
}

void ConsoleConfigWidget::save( Profile& prof ) {
    prof.writeEntry( "Command", m_cmd->text());
    QListViewItem *item = m_env->firstChild();
    int counter = 0;
    while (item) {
        QString name = item->text(0);
        QString value = item->text(1);
        prof.writeEntry("Env_Name_" + QString::number(counter), name);
        prof.writeEntry("Env_Value_" + QString::number(counter), value);
        item = item->nextSibling();
        counter++;
    }
    prof.writeEntry("EnvVars", QString::number(counter));
}

