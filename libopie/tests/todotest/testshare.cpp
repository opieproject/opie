#include <opie/todoevent.h>


int main(int argc, char* argv[] ) {
Opie::ToDoEvent ev1;
Opie::ToDoEvent ev2;
Opie::ToDoEvent ev3;

ev1.setUid( 1 );
ev2 = ev1;
ev3 = ev2;

ev3.setUid( 4 );

return 0;
}