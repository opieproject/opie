/****************************************************************************
** Form interface generated from reading ui file 'outputEdit.ui'
**
** Created: Fri Apr 12 15:12:44 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef OUTPUT_H
#define OUTPUT_H

#include <qvariant.h>
#include <qdialog.h>
#include <qcstring.h>
#include <qstringlist.h>
#include <qlineedit.h>
#include <qwhatsthis.h>

#include <opie/oprocess.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QMultiLineEdit;

class Output : public QDialog
{ 
    Q_OBJECT

public:
    Output(const QStringList commands=0, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Output();

    QMultiLineEdit* OutputEdit;

protected:
    QGridLayout* OutputLayout;
    OProcess *proc;
protected slots:
    void saveOutput();
    void commandStdout(OProcess*, char *, int);
    void commandStdin(const QByteArray &);
    void commandStderr(OProcess*, char *, int);
    void processFinished();
  private:
    QString cmmd;
    
};

class InputDialog : public QDialog
{ 
    Q_OBJECT

public:
    InputDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~InputDialog();
    QString inputText;
    QLineEdit* LineEdit1;
  void setInputText(const QString &); 
private slots:
    void returned();
    
};

#endif // OUTPUT_H
