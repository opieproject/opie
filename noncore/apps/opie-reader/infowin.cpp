#include "infowin.h"
#include "version.h"
#include <stdio.h>

infowin::infowin( QWidget *parent=0, const char *name=0, WFlags f = 0) :
    QWidget(parent, name, f)
{
    grid = new QGridLayout(this, 6, 2);
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
    l = new QLabel("Per centage read", this);
    grid->addWidget(l, 4, 0);
    read = new QLabel("0", this);
    read->setAlignment( AlignVCenter | AlignRight );
    grid->addWidget(read, 4, 1);
    char vstr[128];
    sprintf(vstr, "QT Reader v%u.%u%c (%s)\nA small e-text reader", MAJOR, BKMKTYPE, MINOR, RELEASE_TYPE);
    l = new QLabel(vstr, this);
    grid->addWidget(l, 5, 0);
    QPushButton* exitbutton = new QPushButton("Cancel", this);
    connect( exitbutton, SIGNAL( released() ), this, SLOT( infoClose() ) );
    grid->addWidget(exitbutton, 5, 1);
}
