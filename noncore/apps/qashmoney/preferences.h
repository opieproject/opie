#include <sqlite.h>
#include <qstring.h>

#ifndef PREFERENCES_H
#define PREFERENCES_H

class Preferences
  {
    public:
      Preferences ();
      ~Preferences ();

      void addPreferences ();
      void initializeColumnPreferences ();

      // Returns a preference value for the given preferences ID
      int getPreference ( int );
      int getColumnPreference ( int id );

      // Convenience for getting date separator with one function
      // call.  Takes the preference id as its argument
      QString getSeparator ( );

      //  Changes a preference for the given ID
      void changePreference ( int , int );
      void changeColumnPreference ( int id, int width );

      // The primary database that stores all our data
      sqlite *db;

      // This function returns a Qstring for the year first date formats
      // for displaying.  It takes the date numbers
      QString getDate ( int, int, int );
      QString getDate ( int y, int m );

      void setDefaultDatePreferences ();
  };

#endif











