#include "powerchord.h"
#include <qpushbutton.h>

/* 
 *  Constructs a Example which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
Powerchord::Powerchord( QWidget* parent,  const char* name, WFlags fl )
    : PowerchordBase( parent, name, fl )
{
//    connect(quit, SIGNAL(clicked), this, SLOT(goodBye()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Powerchord::~Powerchord()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  A simple slot... not very interesting.
 */
void Powerchord::goodBye()
{
    close();
}
