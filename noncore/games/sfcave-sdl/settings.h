#ifndef __SETTINGS_H
#define __SETTINGS_H

// This class will create a .<name> directory in the users home directory or
// a directory for the users choice. It will then manage a set of key values
// that the programmer can search for. This allows programmers to save a users
// settings and then retrieve then at a latter time. It currently supports
// upto 1024 different settings.
// Two constructors are provided. They will dertermine what directory to look
// for the settings file and what the name of the file is. If the directory is
// not specified then a default directory of .<DEFAULT_DIR> will be created in
// the users home directory. A file will be created in this directory. The name
// will be the one specified by the caller. If none is specified then
// DEFAULT_FILE will be created.
// To retrieve and store strings into the file readSetting and writeSetting
// should be called.

#include <string>
using namespace std;

class Settings
{

public:

   Settings( char * env_file = 0, char * env_dir = 0 );
   Settings();
   ~Settings();

   bool readSetting(const string key_str,string& results);
   bool readSetting(const string key_str,int& result);
   bool readSetting(const string key_str,unsigned int& result);
   bool readSetting(const string key_str,long int& result);
   bool readSetting(const string key_str,unsigned long& result);
   bool readSetting(const string key_str,double & result);
   bool readSetting(const string key_str,bool& result);

   void writeSetting(const string key_str,const string value);
   void writeSetting(const string key_str,const int value);
   void writeSetting(const string key_str,const unsigned int result);
   void writeSetting(const string key_str,const long int result);
   void writeSetting(const string key_str,const unsigned long result);
   void writeSetting(const string key_str,const double value);
   void writeSetting(const string key_str,const bool value);

   void deleteFile(void);

private:

	string envFile;
};


#endif
