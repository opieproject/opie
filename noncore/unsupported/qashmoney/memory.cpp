#include "memory.h"

#include <stdlib.h>

Memory::Memory ()
  {
    db = sqlite_open ( "qmmemory.db", 0, NULL );
  }

Memory::~Memory ()
  {
    sqlite_close ( db );
  }

void Memory::addMemoryItem ( QString item )
  {
    sqlite_exec_printf ( db, "insert into memory values ( '%q', 0, 0 );", 0, 0, 0, ( const char * ) item );
  }

void Memory::deleteMemoryItem ( QString item )
  {
    sqlite_exec_printf ( db, "delete from memory where item = '%q';", 0, 0, 0, ( const char * ) item );
  }

int Memory::getNumberOfMemoryItems ()
  {
    char **results;
    sqlite_get_table ( db, "select count() from memory;", &results, NULL, NULL, NULL );
    return atoi ( results [ 1 ] );
  }

void Memory::changeMemoryName ( QString item )
  {
    sqlite_exec_printf ( db, "update memory set item = '%q' where item = '%q';", 0, 0, 0, ( const char * ) item );
  }

void Memory::displayMemoryItems ( QListBox *listbox )
  {
    char **results;
    int rows;
    sqlite_get_table ( db, "select item from memory order by item asc;", &results, &rows, NULL, NULL );
    int counter = 1;
    while ( counter < ( rows + 1 ) )
      {
        listbox->insertItem ( results [ counter ] );
        counter ++;
      }
  }

void Memory::displayMemoryItems ( QComboBox *box )
  {
    char **results;
    int rows;
    sqlite_get_table ( db, "select item from memory order by item asc;", &results, &rows, NULL, NULL );
    int counter = 1;
    while ( counter < ( rows + 1 ) )
      {
        box->insertItem ( results [ counter ] );
        counter ++;
      }
  }

