#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>

#include "settings.h"

// Defined in util.h
string getHomeDir();

#define DEFAULT_DIR "."
#define DEFAULT_FILE "Settings.cfg"
#define MAX_LINE_SIZE  2048


Settings::Settings( char * env_file, char * env_dir )
{
	// Store the correct environment directory
	if (env_dir == NULL)
	{
		envFile = getHomeDir();
		envFile.append("/");

		envFile.append(DEFAULT_DIR);
	}
	else
		envFile.append(env_dir);

	envFile.append("/");

	// Store the correct environment file
	if (env_file == NULL)
		envFile.append(DEFAULT_FILE);
	else
		envFile.append(env_file);
}

Settings::Settings()
{
	envFile = getHomeDir();
	envFile.append("/");

	envFile.append(DEFAULT_DIR);
	envFile.append("/");

	envFile.append(DEFAULT_FILE);
}

Settings::~Settings()
{
}

bool Settings::readSetting(const string key_str,int& result)
{
	string Buffer;
	if (readSetting(key_str,Buffer))
	{
		result = atoi(Buffer.c_str());
		return true;
	}
	else
		return false;
}

bool Settings::readSetting(const string key_str,unsigned int& result)
{
	string Buffer;
	if (readSetting(key_str,Buffer))
	{
		result = atoi(Buffer.c_str());
		return true;
	}
	else
		return false;
}

bool Settings::readSetting(const string key_str,long int& result)
{
	string Buffer;
	if (readSetting(key_str,Buffer))
	{
		result = atol(Buffer.c_str());
		return true;
	}
	else
		return false;
}

bool Settings::readSetting(const string key_str,unsigned long& result)
{
	string Buffer;
	if (readSetting(key_str,Buffer))
	{
		result = atol(Buffer.c_str());
		return true;
	}
	else
		return false;
}

bool Settings::readSetting(const string key_str,double& result)
{
	string Buffer;
	if (readSetting(key_str,Buffer))
	{
        result = atof( Buffer.c_str() );
		return true;
	}
	else
		return false;
}

bool Settings::readSetting(const string key_str,bool& result)
{
	string Buffer;
	if (readSetting(key_str,Buffer))
	{
		result = (Buffer == "true");
		return true;
	}
	else
		return false;
}

bool Settings::readSetting(const string key_str,string& results)
{
	// This function will read a string from the env file that corresponds to the
	// key key_str passed in.
	FILE *  fd = 0;
	char  buf[MAX_LINE_SIZE];
	bool ret_flag = false;
	char* key;
	char* value;

	// open file
	fd = fopen(envFile.c_str(), "r");

	if (fd)
	{
		while (fgets(buf, MAX_LINE_SIZE-1, fd))
		{
			key = strtok(buf, "\t");
			value = strtok(NULL, "\n");
			// find key in file
			if (!strcasecmp(key,key_str.c_str()))
			{
				results = value;
				ret_flag = true;
			}
		}
		fclose(fd);
	}

	return(ret_flag);
}

void Settings::writeSetting(const string key_str,const bool value)
{
	value ?	writeSetting(key_str,string("true")) :writeSetting(key_str,string("false"));
}

void Settings::writeSetting(const string key_str,const double value)
{
	char Buffer[30];

	sprintf(Buffer,"%lf",value);
	writeSetting(key_str,string(Buffer));
}

void Settings::writeSetting(const string key_str,const int value)
{
	char Buffer[30];

	sprintf(Buffer,"%i",value);
	writeSetting(key_str,string(Buffer));
}

void Settings::writeSetting(const string key_str,const unsigned int value)
{
	char Buffer[30];

	sprintf(Buffer,"%i",value);
	writeSetting(key_str,string(Buffer));
}

void Settings::writeSetting(const string key_str,const long int value)
{
	char Buffer[30];

	sprintf(Buffer,"%li",value);
	writeSetting(key_str,string(Buffer));
}

void Settings::writeSetting(const string key_str,const unsigned long value)
{
	char Buffer[30];

	sprintf(Buffer,"%lu",value);
	writeSetting(key_str,string(Buffer));
}

void Settings::writeSetting(const string key_str,const string value)
{
	// This function will write a value for the key key_str. If the key_str
	// already exists then it will be overwritten.
	FILE *fd=NULL,*ftemp=NULL;
	char  buf[MAX_LINE_SIZE];

	string tmp = getHomeDir() + "/tmpsfcave.dat";

	// if file exists we need to save contents
	fd = fopen( envFile.c_str(), "r" );
	ftemp = fopen( tmp.c_str(), "w" );
	if ( fd != NULL && ftemp != NULL )
	{
		while (fgets(buf, MAX_LINE_SIZE-1, fd))
		{
			if ( strncmp( buf, key_str.c_str(), key_str.size() ) != 0 )
				fprintf( ftemp, "%s", buf );
		}
		fclose(fd);
	}

	if ( ftemp != NULL )
	{
		fprintf(ftemp, "%s\t%s\n", key_str.c_str(),value.c_str());
		fclose( ftemp );

		remove(envFile.c_str());
		rename( tmp.c_str(), envFile.c_str() );
	}
/*
			string tmp = getHomeDir() + "/tmpsfcave.dat";
			strcpy(tempname,tmp.c_str() );
			printf( "tmp - %s\n", tempname );
			if ((ftemp = fopen(tempname,"w")) != NULL)
			{
				char *key1,*key2;
				char buff1[80],buff2[80];

				strncpy(buff1,key_str.c_str(),80);
				key1 = strtok(buff1,"\t");
				for (std::vector<string>::iterator iter = FileEntries.begin(); iter < FileEntries.end(); iter++)
				{
					strncpy(buff2,(*iter).c_str(),80);
					key2 = strtok(buff2,"\t");
					// IF not the key string then write out to file
					if (strcmp(key1,key2) != 0)
					{
						fprintf(ftemp,"%s",iter->c_str());
						fflush(ftemp);
					}
				}

				fprintf(ftemp, "%s\t%s\n", key_str.c_str(),value.c_str());
				fflush(ftemp);
				fclose(ftemp);
			}
			else
				printf( "Can't open file %s\n", envFile.c_str() );
		}

		delete dir_str;
	}
*/
}

void Settings::deleteFile(void)
{
	remove(envFile.c_str());
}
