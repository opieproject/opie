#ifndef MEMORY_H
#define MEMORY_H

#include <qstring.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <sqlite.h>

class Memory : public QObject
  {

    Q_OBJECT

    public:

      Memory ();
      ~Memory ();

      // This function adds a new memory to the database.  It takes the memory name, parent,
      // initial balance, a displayed variable, and the memory type
      // The parent is an integer memory id.  Its -1 if there is not parent
      // The memory types  are 0=not defined 1=parent 2=child
      void addMemoryItem ( QString );

      // Returns the number of checking memorys
      int getNumberOfMemoryItems ();

      void changeMemoryName ( QString );

      // This takes a QListView and puts parents and children memorys
      // into the list view
      void displayMemoryItems ( QListBox * );
      void displayMemoryItems ( QComboBox * );

      // The primary database that stores all our data
      sqlite *db;

    public slots:

      // Deletes a memory item.  Takes the memoryid as its parameter
      void deleteMemoryItem ( QString );
  };

#endif

