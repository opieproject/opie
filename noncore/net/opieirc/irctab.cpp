#include <qpe/resource.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include "irctab.h"

IRCTab::IRCTab(QWidget *parent, const char *name, WFlags f) : QWidget(parent, name, f) {
    m_layout = new QVBoxLayout(this);
    QHBoxLayout *descLayout = new QHBoxLayout(m_layout);
    descLayout->setMargin(5);
    m_description = new QLabel(tr("Missing description"), this);
    descLayout->addWidget(m_description);
    descLayout->setStretchFactor(m_description, 5);
    QPushButton *close = new QPushButton(this);
    close->setPixmap(Resource::loadPixmap("close"));
    connect(close, SIGNAL(clicked()), this, SLOT(remove()));
    descLayout->addWidget(close);
    descLayout->setStretchFactor(m_description, 1);
}

