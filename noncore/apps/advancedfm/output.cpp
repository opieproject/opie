/****************************************************************************
** outputEdit.cpp
**
** Copyright: Fri Apr 12 15:12:58 2002 L.J. Potter <ljp@llornkcor.com>
****************************************************************************/
#include "output.h"

#include <opie/oprocess.h>

#include <qpe/filemanager.h>
#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>

#include <qmessagebox.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qcstring.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>

#include <errno.h>

/* XPM */
static char * filesave_xpm[] = {
"16 16 78 1",
"   c None",
".  c #343434",
"+  c #A0A0A0",
"@  c #565656",
"#  c #9E9E9E",
"$  c #525252",
"%  c #929292",
"&  c #676767",
"*  c #848484",
"=  c #666666",
"-  c #D8D8D8",
";  c #FFFFFF",
">  c #DBDBDB",
",  c #636363",
"'  c #989898",
")  c #2D2D2D",
"!  c #909090",
"~  c #AEAEAE",
"{  c #EAEAEA",
"]  c #575757",
"^  c #585858",
"/  c #8A8A8A",
"(  c #828282",
"_  c #6F6F6F",
":  c #C9C9C9",
"<  c #050505",
"[  c #292929",
"}  c #777777",
"|  c #616161",
"1  c #3A3A3A",
"2  c #BEBEBE",
"3  c #2C2C2C",
"4  c #7C7C7C",
"5  c #F6F6F6",
"6  c #FCFCFC",
"7  c #6B6B6B",
"8  c #959595",
"9  c #4F4F4F",
"0  c #808080",
"a  c #767676",
"b  c #818181",
"c  c #B8B8B8",
"d  c #FBFBFB",
"e  c #F9F9F9",
"f  c #CCCCCC",
"g  c #030303",
"h  c #737373",
"i  c #7A7A7A",
"j  c #7E7E7E",
"k  c #6A6A6A",
"l  c #FAFAFA",
"m  c #505050",
"n  c #9D9D9D",
"o  c #333333",
"p  c #7B7B7B",
"q  c #787878",
"r  c #696969",
"s  c #494949",
"t  c #555555",
"u  c #949494",
"v  c #E6E6E6",
"w  c #424242",
"x  c #515151",
"y  c #535353",
"z  c #3E3E3E",
"A  c #D4D4D4",
"B  c #0C0C0C",
"C  c #353535",
"D  c #474747",
"E  c #ECECEC",
"F  c #919191",
"G  c #7D7D7D",
"H  c #000000",
"I  c #404040",
"J  c #858585",
"K  c #323232",
"L  c #D0D0D0",
"M  c #1C1C1C",
"    ...+        ",
"   @#$%&..+     ",
"   .*=-;;>,..+  ",
"  ')!~;;;;;;{]..",
"  ^/(-;;;;;;;_:<",
"  [}|;;;;;;;{12$",
" #34-55;;;;678$+",
" 90ab=c;dd;e1fg ",
" [ahij((kbl0mn$ ",
" op^q^^7r&]s/$+ ",
"@btu;vbwxy]zAB  ",
"CzDEvEv;;DssF$  ",
"G.H{E{E{IxsJ$+  ",
"  +...vEKxzLM   ",
"     +...z]n$   ",
"        +...    "};

Output::Output( const QStringList commands, QWidget* parent,  const char* name, bool modal, WFlags fl)
    : QDialog( parent, name, modal, fl )
{
   QStringList cmmds;
//   cmmds=QStringList::split( " ", commands, false);
   cmmds=commands;
//   qDebug("count %d", cmmds.count());   
   if ( !name )
       setName( tr("Output"));
    resize( 196, 269 ); 
    setCaption( name );

    OutputLayout = new QGridLayout( this ); 
    OutputLayout->setSpacing( 2);
    OutputLayout->setMargin( 2);

    QPushButton *docButton;
     docButton = new QPushButton(  QPixmap(( const char** ) filesave_xpm  ) ,"",this,"saveButton");
     docButton->setFixedSize( QSize( 20, 20 ) );
     connect( docButton,SIGNAL(released()),this,SLOT( saveOutput() ));
 //    docButton->setFlat(TRUE);
     OutputLayout->addMultiCellWidget( docButton, 0,0,3,3 );

    OutputEdit = new QMultiLineEdit( this, "OutputEdit" );
    OutputLayout->addMultiCellWidget( OutputEdit, 1,1,0,3 );

    proc = new OProcess();

    connect(proc, SIGNAL(processExited(OProcess *)),
            this, SLOT( processFinished()));

    connect(proc, SIGNAL(receivedStdout(OProcess *, char *, int)),
            this, SLOT(commandStdout(OProcess *, char *, int)));

    connect(proc, SIGNAL(receivedStderr(OProcess *, char *, int)),
            this, SLOT(commandStderr(OProcess *, char *, int)));

//     connect( , SIGNAL(received(const QByteArray &)),
//             this, SLOT(commandStdin(const QByteArray &)));

//    * proc << commands.latin1(); 
      for ( QStringList::Iterator it = cmmds.begin(); it != cmmds.end(); ++it ) {
         qDebug( "%s", (*it).latin1() );
         * proc << (*it).latin1();
      }

     if(!proc->start(OProcess::NotifyOnExit, OProcess::All)) {

         OutputEdit->append("Process could not start");
         OutputEdit->setCursorPosition( OutputEdit->numLines() + 1,0,FALSE);
         perror("Error: ");
         QString errorMsg="Error\n"+(QString)strerror(errno);
         OutputEdit->append( errorMsg);
         OutputEdit->setCursorPosition( OutputEdit->numLines() + 1,0,FALSE);
     }
}

Output::~Output() {
}

void Output::saveOutput() {

    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Save output to file (name only)"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
      QString  filename = QPEApplication::documentDir(); 
      if(filename.right(1).find('/') == -1)
          filename+="/";
      QString name = fileDlg->LineEdit1->text();
      filename+="text/plain/"+name;
      qDebug(filename);

      QFile f(filename);
      f.open( IO_WriteOnly);
      if( f.writeBlock( OutputEdit->text(), qstrlen( OutputEdit->text()) ) != -1) {
          DocLnk lnk;
          lnk.setName(name); //sets file name
          lnk.setFile(filename); //sets File property
          lnk.setType("text/plain");
          if(!lnk.writeLink()) {
              qDebug("Writing doclink did not work");
          }
      } else
          qWarning("Could not write file");
      f.close();
    }
}

void Output::commandStdout(OProcess*, char *buffer, int buflen) {
    qWarning("received stdout %d bytes", buflen);

//     QByteArray data(buflen);
//     data.fill(*buffer, buflen);
//     for (uint i = 0; i < data.count(); i++ ) {
//         printf("%c", buffer[i] );
//     }
//     printf("\n");

    QString lineStr = buffer;
    lineStr=lineStr.left(lineStr.length()-1);
    OutputEdit->append(lineStr);
    OutputEdit->setCursorPosition( OutputEdit->numLines() + 1,0,FALSE);
}


void Output::commandStdin( const QByteArray &data) {
    qWarning("received stdin  %d bytes", data.size());
    // recieved data from the io layer goes to sz
    proc->writeStdin(data.data(), data.size());
}

void Output::commandStderr(OProcess*, char *buffer, int buflen) {
    qWarning("received stderrt %d bytes", buflen);

    QString lineStr = buffer;
//    lineStr=lineStr.left(lineStr.length()-1);
    OutputEdit->append(lineStr);
    OutputEdit->setCursorPosition( OutputEdit->numLines() + 1,0,FALSE);
}

void Output::processFinished() {

    delete proc;
    OutputEdit->append( "\nFinished\n");
    OutputEdit->setCursorPosition( OutputEdit->numLines() + 1,0,FALSE);
//    close();
//     disconnect( layer(), SIGNAL(received(const QByteArray &)),
//                this, SLOT(commandStdin(const QByteArray &)));
}

//==============================

InputDialog::InputDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( "InputDialog" );
    resize( 234, 50 ); 
    setMaximumSize( QSize( 240, 50 ) );
    setCaption( tr(name ) );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 10, 10, 216, 22 ) );
    connect(LineEdit1,SIGNAL(returnPressed()),this,SLOT(returned() ));
}

InputDialog::~InputDialog() {
    inputText = LineEdit1->text();
}

void InputDialog::setInputText(const QString &string) {
    LineEdit1->setText( string);
}

void InputDialog::returned() {
    inputText = LineEdit1->text();
    this->accept();
}
