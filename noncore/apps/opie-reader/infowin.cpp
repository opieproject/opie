#include "infowin.h"
#include "version.h"
#include <stdio.h>
#include <qmultilineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include "names.h"

void infowin::setAbout(const QString& _s)
{
  char vstr[128];
  sprintf(vstr, PROGNAME " v%u.%u%c (%s) - A small e-text reader\n%s %s", MAJOR, BKMKTYPE, MINOR, RELEASE_TYPE, __DATE__, __TIME__);
    //    l = new QLabel(vstr, this);
  QString c = vstr;
  c += _s;
  aboutbox->setText(c);
}

infowin::infowin( QWidget *parent, const char *name, WFlags f) :
    QWidget(parent, name, f)
{
  QVBoxLayout* vl = new QVBoxLayout(this);
  aboutbox = new QMultiLineEdit(this);
  aboutbox->setReadOnly(true);
    aboutbox->setWordWrap(QMultiLineEdit::WidgetWidth);
  //grid->addWidget(l, 5, 0);
  vl->addWidget(aboutbox);
    QGridLayout* grid = new QGridLayout(vl, 10, 2);
    QLabel* l;
    l = new QLabel("Compressed file size", this);
    grid->addWidget(l, 0, 0);
    fileSize = new QLabel("0", this);
    fileSize->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(fileSize, 0, 1);
    l = new QLabel("Original text size", this);
    grid->addWidget(l, 1, 0);
    textSize = new QLabel("0", this);
    textSize->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(textSize, 1, 1);
    l = new QLabel("Compression Ratio", this);
    grid->addWidget(l, 2, 0);
    ratio = new QLabel("0", this);
    grid->addWidget(ratio, 2, 1);
    ratio->setAlignment( AlignVCenter | AlignRight );
    l = new QLabel("Current location", this);
    grid->addWidget(l, 3, 0);
    location = new QLabel("0", this);
    location->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(location, 3, 1);
    l = new QLabel("Per centage read (file)", this);
    grid->addWidget(l, 4, 0);
    read = new QLabel("0", this);
    read->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(read, 4, 1);

    l = new QLabel("Document Size", this);
    grid->addWidget(l, 5, 0);
    docSize = new QLabel("0", this);
    docSize->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(docSize, 5, 1);

    l = new QLabel("Document Location", this);
    grid->addWidget(l, 6, 0);
    docLocation = new QLabel("0", this);
    docLocation->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(docLocation, 6, 1);

    l = new QLabel("Per centage read (doc)", this);
    grid->addWidget(l, 7, 0);
    docread = new QLabel("0", this);
    docread->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(docread, 7, 1);


    l = new QLabel("Zoom", this);
    grid->addWidget(l, 8, 0);
    zoom = new QLabel("0", this);
    zoom->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(zoom, 8, 1);
    QPushButton* exitbutton = new QPushButton("Cancel", this);
    connect( exitbutton, SIGNAL( clicked() ), this, SLOT( infoClose() ) );
    grid->addWidget(exitbutton, 9, 1);
}
