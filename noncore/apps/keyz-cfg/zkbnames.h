#ifndef ZKBNAMES_H
#define ZKBNAMES_H

#ifdef USE_ZKB_NAMES

#include <qstring.h>

class KeyNames {
public:
	static int find(const QString& key);
	static const QString& find(int);
};

class KeycodeNames {
public:
	static int find(const QString& key);
	static const QString& find(int);
};

class ModifierNames {
public:
	static int find(const QString& key);
	static const QString& find(int);
};

#endif
#endif
