#ifndef MINIKDE_KDEBUG_H
#define MINIKDE_KDEBUG_H

#include <stdio.h>

#include <qstring.h>

class kdbgstream;
typedef kdbgstream & (*KDBGFUNC)(kdbgstream &); // manipulator function

class kdbgstream {
 public:
    kdbgstream(unsigned int _area, unsigned int _level, bool _print = true) :
      area(_area), level(_level),  print(_print) { }
    kdbgstream(const char * initialString, unsigned int _area, unsigned int _level, bool _print = true) :
      output(QString::fromLatin1(initialString)), area(_area), level(_level),  print(_print) { }
    ~kdbgstream()
    {
      if (!output.isEmpty()) {
        fprintf(stderr,"ASSERT: debug output not ended with \\n\n");
        *this << "\n";
      }
    }
   kdbgstream &operator<<(bool i)  {
	if (!print) return *this;
	output += QString::fromLatin1(i ? "true" : "false");
	return *this;
    }
    kdbgstream &operator<<(short i)  {
	if (!print) return *this;
	QString tmp; tmp.setNum(i); output += tmp;
	return *this;
    }
    kdbgstream &operator<<(unsigned short i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    kdbgstream &operator<<(char i)  {
	if (!print) return *this;
	QString tmp; tmp.setNum(int(i)); output += tmp;
	return *this;
    }
    kdbgstream &operator<<(unsigned char i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(static_cast<unsigned int>(i)); output += tmp;
        return *this;
    }

    kdbgstream &operator<<(int i)  {
	if (!print) return *this;
	QString tmp; tmp.setNum(i); output += tmp;
	return *this;
    }
    kdbgstream &operator<<(unsigned int i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    kdbgstream &operator<<(long i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    kdbgstream &operator<<(unsigned long i) {
        if (!print) return *this;
        QString tmp; tmp.setNum(i); output += tmp;
        return *this;
    }
    kdbgstream &operator<<(const QString& string) {
	if (!print) return *this;
	output += string;
	if (output.at(output.length() -1 ) == '\n')
	    flush();
	return *this;
    }
    kdbgstream &operator<<(const char *string) {
	if (!print) return *this;
	output += QString::fromUtf8(string);
	if (output.at(output.length() - 1) == '\n')
	    flush();
	return *this;
    }
    kdbgstream &operator<<(const QCString& string) {
      *this << string.data();
      return *this;
    }
    kdbgstream& operator<<(KDBGFUNC f) {
	if (!print) return *this;
	return (*f)(*this);
    }
    kdbgstream& operator<<(double d) {
      QString tmp; tmp.setNum(d); output += tmp;
      return *this;
    }
    void flush() {
      if (output.isEmpty() || !print)
	return;
      printf("%s",output.latin1());
      output = QString::null;
    }
 private:
    QString output;
    unsigned int area, level;
    bool print;
};

inline kdbgstream &endl( kdbgstream &s) { s << "\n"; return s; }

inline kdbgstream kdDebug(int area = 0) { return kdbgstream(area, 0); }

#endif
