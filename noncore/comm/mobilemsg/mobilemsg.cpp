#include "mobilemsg.h"
#include <qpushbutton.h>

#include <qpe/qpeapplication.h>

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>


/* 
 *  Constructs a MobileMsg which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
MobileMsg::MobileMsg( QWidget* parent,  const char* name, WFlags fl )
    : MobileMsgBase( parent, name, fl )
{
    //connect(add, SIGNAL(clicked()), this, SLOT(goodBye()));
    //connect(remove, SIGNAL(clicked()), this, SLOT(goodBye()));
    //connect(props, SIGNAL(clicked()), this, SLOT(goodBye()));
    //connect(settings, SIGNAL(selectionChanged()), this, SLOT(goodBye()) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MobileMsg::~MobileMsg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  A simple slot... not very interesting.
 */
void MobileMsg::goodBye()
{
    close();
}
