#include "qsmb.h"
#include <qpushbutton.h>
#include <qpe/qpeapplication.h>

#include <string.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <qpe/process.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qtextview.h>
#include <qmessagebox.h>
#include <qtextstream.h>


#include <pthread.h>
#include <signal.h>
#include <ctype.h>


#include <netinet/in.h>
#include <arpa/inet.h>

#include <rpc/clnt.h>


#include <opie2/odebug.h>
using namespace Opie::Core;


Qsmb::Qsmb( QWidget* parent,  const char* name, WFlags fl )
   : FormQPESMBBase( parent, name, fl )
{
   connect(CBHost, SIGNAL(activated(int)), this, SLOT(hostSelected(int)));
   connect(DoItBtn, SIGNAL(clicked()), this, SLOT(DoItClicked()));
   connect(BtnScan, SIGNAL(clicked()), this, SLOT(scanClicked()));
   connect(BtnClear, SIGNAL(clicked()), this, SLOT(clear()));

   mountpt->insertItem("/mnt/samba1",-1);
   mountpt->insertItem("/mnt/samba2",-1);
   mountpt->insertItem("/mnt/samba3",-1);

   top_element = NULL;
   scanning = false;
}

Qsmb::~Qsmb()
{
}

void Qsmb::clear() 
{
   if (scanning) return;
   ListViewScan->clear();	
   TextViewOutput->setText("");
   top_element = NULL;	
}

void Qsmb::scanClicked() 
{
   if (scanning) return;
   pthread_create(&tpid, NULL, runit, (void *)this);
}

void Qsmb::DoItClicked() 
{

   if(! ListViewScan->selectedItem()) {
      QMessageBox::warning(this, tr("Error"),tr("<p>No share selected!</p>"));
      return;
   }
   if (scanning) return;
   pthread_create(&tpid, NULL, runitm, (void *)this);
}

void* runit(void* arg) 
{
   Qsmb* caller = (Qsmb*)arg;
   caller->scan();
   return(0);
}

void* runitm(void* arg) 
{
   Qsmb* caller = (Qsmb*)arg;
   caller->DoIt();
   return(0);
}

void Qsmb::scan() 
{
//   int i;

   if (scanning) return;
   scanning = true;

   QString match;
   QString cmd;
   LScan->setText("Scanning...");
   qApp->processEvents();

   sockaddr_in my_addr;
   get_myaddress( &my_addr);

   QString ip = inet_ntoa( my_addr.sin_addr);
   owarn << "IP Address : " << ip<< oendl;

   match = ip.left(5);

   QStringList ccmd;
   TextViewOutput->append("smbfind");

   QFile lmhosts("/etc/samba/lmhosts");
   QTextStream lms(&lmhosts);
   lmhosts.open(IO_WriteOnly);
   lms << "127.0.0.1 localhost\n";
    
   /* parse output and display in ListViewScan */
   ccmd = "smbfind";
	 runCommand(ccmd);

   QTextStream s(&out, IO_ReadOnly);

   while ( !s.atEnd() ) {
      QString ip_addr, host, output;
      QString tmp = s.readLine();
      bool ok;
      tmp.left(1).toInt( &ok, 10 );
      if(ok) {
         QStringList token = QStringList::split(' ',  tmp );
         ip_addr =  token[0];
         host = token[1];
         CBHost->insertItem( host, -1);
         lms <<  ip_addr+" "+host+"\n";
      }
   }
   lmhosts.close();

   TextViewOutput->append("\n\n============================================\n");
   LScan->setText("");
   scanning = false;
}

void Qsmb::hostSelected(int index)
{
   owarn << "hostSelected" << oendl;
   QListViewItem *element;
   QListViewItem *parent;

   QString text = CBHost->currentText();

   if (scanning) return;
   scanning = true;

   QString cmd;
   QStringList ccmd;

   LScan->setText("Scanning...");

   ccmd << "/usr/bin/smbclient";
   ccmd << "-L";
   ccmd << CBHost->currentText();
   ccmd << "-N";
   
   if(!username->text().isEmpty()) {
   } else {
      ccmd << "-U";
      ccmd << username->text()+"\%"+ password->text();
   }
   runCommand(ccmd);
   QTextStream s(&out, IO_ReadOnly);

   while ( !s.atEnd() ) {
      QString share;
      QString comment;
      QString tmp = s.readLine();
      
      if( tmp.find("$") == -1 && tmp.find("Disk") != -1) {
         QStringList token = QStringList::split(' ',  tmp );
         share = token[0];
         comment = token[2];
         element = new QListViewItem(ListViewScan,share, comment);
         element->setOpen(true);
//             top_element = element;
//             parent = element;
      }

   }
//   owarn << "i="<< index << "cmd:" <<  cmd << oendl;

   TextViewOutput->append(cmd);

   /* run smbclient & read output */
//    if ((pipe = popen(cmd.latin1(), "r")) == NULL) {
//       snprintf(result, 256, "Error: Can't run %s", cmd.latin1());
// //      cmd = "Error: Can't run "+cmd;
//       TextViewOutput->append(result);
//       return;
//    }

   /* parse output and display in ListViewScan */
//    while(fgets(result, 256, pipe) != NULL) {
//       /* put result into TextViewOutput */
//       TextViewOutput->append(result);

//       if( strchr(result, '$') == NULL ) { 
//          char share[256], *ptr1;

//          strcpy(share,result);
//          ptr1 = strchr(share,' ');
//          share[ptr1 - share]='\0';

//          owarn<< "add share: " << share << oendl;

//          if(top_element != NULL) {
//             bool found = false;
//             element = top_element;

//             while(element != NULL && !found) {
//                if(strcmp( element->text(0).ascii(), share)==0) {
//                   parent = element;
//                   found = true;
//                }
//                element = element->nextSibling();
//             }

//             if(!found) {
//                element = new QListViewItem(ListViewScan,share);
//                element->setOpen(true);
//                parent=element;
//             }
//          } else {
//             element = new QListViewItem(ListViewScan,share);
//             element->setOpen(true);
//             top_element = element;
//             parent = element;
//          }
//       }
//    }

   TextViewOutput->append("\n\n============================================\n");
   LScan->setText("");
   scanning = false;
}

void Qsmb::DoIt()
{
   QListViewItem *element;
   element = ListViewScan->selectedItem();
   if(!element) {
      QMessageBox::warning(this, tr("Error!!"),tr("<p><b>No</b> share selected!!</p>"));
      return;
   }

   if (scanning) return;
   scanning = true;

   int i;
   bool err = false;

   char share[512];
   QString cmd;
   QString cmd2;
   char result[256];
//   QString result;
   QString text = mountpt->currentText();

   FILE *pipe,*pipe2;

   LScan->setText("Mounting...");
   qApp->processEvents();


   if(! QFileInfo(text).exists()) {
      QStringList ccmd;
      ccmd << "mkdir";
      ccmd << "-p";
      ccmd << text;

      owarn<<"cmd: "<< ccmd << oendl;
      runCommand(ccmd);
   }


   strcpy(share,(const char *)element->text(0));

   for(i = 0; i < 256; i++) {
      if(isalpha( share[i]))  {
         strcpy( share, share + i);
         break;
      }
   }

   cmd = "/usr/bin/smbmount  //"+CBHost->currentText()+"/"+share+" "+mountpt->currentText()+" -o username="+username->text()+",password="+password->text();

   owarn << "cmd: " << cmd << oendl;
   TextViewOutput->append(cmd.latin1());


   if(onbootBtn->isChecked()) {
      owarn << "Saving Setting permanently..." << oendl;
      cmd2 = "echo '" + cmd + "'>/opt/QtPalmtop/etc/samba.env";
      /* run command & read output */
      if ((pipe = popen(cmd2.latin1(), "r")) == NULL) {
         snprintf(result, 256, "Error: Can't run %s", cmd.latin1());
         return;
      }
      /* parse output and display in ListViewScan */
      while(fgets(result, 256, pipe) != NULL) {
      }
   }
	

   /* run command & read output */
   if ((pipe = popen(cmd.latin1(), "r")) == NULL) {
      snprintf(result, 256, "Error: Can't run %s", cmd.latin1());
      TextViewOutput->append(result);
      return;
   }

   /* parse output and display in ListViewScan */
   while(fgets(result, 256, pipe) != NULL) {
      /* put result into TextViewOutput */
      TextViewOutput->append(result);
   }

   
   TextViewOutput->append("\n\n================CheckMounts==================\n");
   LScan->setText("");

   cmd = "/bin/mount 2>&1";
   owarn << "cmd: " << cmd << oendl;

   if ((pipe2 = popen(cmd.latin1(), "r")) == NULL) {

      snprintf(result, 256, "Error: Can't run %s", cmd.latin1());
//         result = "Error: Can't run "+ cmd;

      TextViewOutput->append(result);
      return;
   }
   /* parse output and display in ListViewScan */
   while(fgets(result, 256, pipe2) != NULL) {
      /* put result into TextViewOutput */
      TextViewOutput->append(result);
   }

   TextViewOutput->append("\n\n============================================\n");
   scanning = false;
}

bool Qsmb::runCommand(const QStringList & command) {
   owarn << "runCommand " << command.join(" ") << oendl;
   out = "";
   Process ipkg_status( command);
   bool r = ipkg_status.exec("",out);

   if(!r) {
      QMessageBox::warning(this, tr("Error!!"),tr("<p>"+out+"</p>"));
   }

   owarn << "Output " << out << oendl;
   TextViewOutput->append(out.latin1());
   return r;
}


