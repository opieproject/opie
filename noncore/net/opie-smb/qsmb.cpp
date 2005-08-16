#include "qsmb.h"
#include <qpushbutton.h>
#include <qpe/qpeapplication.h>

#include <string.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qtabwidget.h>

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

#include <sys/vfs.h>
#include <mntent.h>

// #include <opie2/odebug.h>
// using namespace Opie::Core;


Qsmb::Qsmb( QWidget* parent,  const char* name, WFlags fl )
   : FormQPESMBBase( parent, name, fl )
{
   connect(CBHost, SIGNAL(activated(int)), this, SLOT(hostSelected(int)));
   connect(DoItBtn, SIGNAL(clicked()), this, SLOT(DoItClicked()));
   connect(UnmountBtn, SIGNAL(clicked()), this, SLOT(umountIt()));
   connect(BtnScan, SIGNAL(clicked()), this, SLOT(scanClicked()));
   connect(BtnClear, SIGNAL(clicked()), this, SLOT(clear()));
   connect(ListViewScan, SIGNAL(clicked(QListViewItem*)), this, SLOT(TextViewClicked(QListViewItem*)));

   mountpt->setEditable(true);
   mountpt->insertItem("/mnt/samba1",-1);
   mountpt->insertItem("/mnt/samba2",-1);
   mountpt->insertItem("/mnt/samba3",-1);
   
   setTabOrder(BtnScan, username);
   setTabOrder(username, password);
   setTabOrder(password, CBHost);
   setTabOrder(CBHost, TextViewOutput);
   setTabOrder(TextViewOutput, mountpt);
   setTabOrder(mountpt, DoItBtn);
   setTabOrder(DoItBtn, UnmountBtn);
   
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
   CBHost->clear();
   top_element = NULL;	
}

void Qsmb::scanClicked() 
{
   if (scanning) return;
   pthread_create(&tpid, NULL, runit, (void *)this);
}

void Qsmb::DoItClicked() 
{

   if( !ListViewScan->selectedItem()) {
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
   clear();
//   if (scanning) return;
   scanning = true;

   QString match;
   QString cmd;
   LScan->setText("Scanning...");
   qApp->processEvents();

   sockaddr_in my_addr;
   get_myaddress( &my_addr);

   QString ip = inet_ntoa( my_addr.sin_addr);
   qWarning("IP Address : "+ip);

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

void Qsmb::hostSelected(int /*index*/ )
{
   QListViewItem *element;
//   QListViewItem *parent;

   QString text = CBHost->currentText();
   ListViewScan->clear();

   if (scanning) return;
   scanning = true;

   QString cmd;
   QStringList ccmd;

   LScan->setText("Scanning...");

   ccmd << "/usr/bin/smbclient";
   ccmd << "-L";
   ccmd << CBHost->currentText();
   ccmd << "-N";
   
   if(username->text().isEmpty()) {
      //do nothing
   } else {
      ccmd << "-U";
      ccmd << username->text()+"\%"+ password->text();
   }
   runCommand(ccmd);
   QTextStream s(&out, IO_ReadOnly);

   while ( !s.atEnd() ) {
      QString share;
      QString comment;
      QString mount;
      QString tmp = s.readLine();
      
      if( tmp.find("$") == -1 && tmp.find("Disk") != -1) {
         QStringList token = QStringList::split(' ',  tmp );
         share = token[0];
         comment = token[2];
         share = share.stripWhiteSpace();
         comment = comment.stripWhiteSpace();
//         if(isMounted(share))

         mount = getMount(share);
         element = new QListViewItem(ListViewScan, share, comment, mount);
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
      return;
   }

   if (scanning) return;
   scanning = true;

   QString mount = mountpt->currentText();
   if(isMounted(mount)) {
      qWarning(mount +" is already mounted");
      TextViewOutput->append(mount +" is already mounted");
      return;
   }
   
   bool noerr = false;

   QString share;
   QString cmd;
   QString cmd2;
   QString text = mountpt->currentText();
   QStringList ccmd;

   LScan->setText("Mounting...");
   qApp->processEvents();

   if( !QFileInfo(text).exists()) {
      ccmd << "mkdir";
      ccmd << "-p";
      ccmd << text;

      qWarning( "cmd: "+ ccmd.join(" "));
      runCommand(ccmd);
   }

   share = element->text(0);
   qWarning("selected share is "+share);

   QString service = CBHost->currentText();
   service = service.stripWhiteSpace();
   if(mount.left(1) != "/")
      mount = QDir::currentDirPath()+"/"+mount;
   mount = mount.stripWhiteSpace();
   ccmd.clear();
   
   ccmd << "/usr/bin/smbmount";
   ccmd << "//"+ service+"/"+share;
   ccmd << mount;
   ccmd << "-o";
   ccmd << "username="+username->text()+",password="+password->text()+"";

   if(onbootBtn->isChecked()) {
      qWarning("Saving Setting permanently...");
      QFile sambenv("/opt/QtPalmtop/etc/samba.env");
      QTextStream smbv(&sambenv);
      sambenv.open(IO_WriteOnly);
      smbv << ccmd.join(" ") ;
      sambenv.close();
   }

   noerr = runCommand(ccmd);
   
   LScan->setText("");

   if(noerr && isMounted(mount)) {
      element->setText(2, mount);
      TextViewOutput->append("\n\n================CheckMounts==================\n");
      ccmd = "/bin/mount";
      runCommand(ccmd);
      TextViewOutput->append("\n\n============================================\n");
      qApp->processEvents();
   } else {
   //do nothing
 }

   scanning = false;
}

void Qsmb::umountIt()
{
   QListViewItem *element;
   element = ListViewScan->selectedItem();
   if(!element) {
      return;
   }

   QString mount = mountpt->currentText();
   if(!isMounted(mount)) {
      qWarning(mount +" is not mounted");
      TextViewOutput->append(mount +" is not mounted");
      return;
   }

   QStringList ccmd;
   QString share;
   share = element->text(0);
   qWarning("selected share is "+share);

   if(mount.left(1) != "/")
      mount = QDir::currentDirPath()+"/"+mount;
   mount = mount.stripWhiteSpace();

   ccmd << "/usr/bin/smbumount";
   ccmd << mount;
   runCommand(ccmd);

   element->setText(2, "");

   ccmd = "/bin/mount";
   runCommand(ccmd);
}

bool Qsmb::runCommand(const QStringList & command) {
   qWarning( "runCommand " + command.join(" ") );
   TextViewOutput->append(command.join(" "));
   out = "";
   Process ipkg_status( command);
   bool r = ipkg_status.exec("",out);

   qWarning("result is %d"+ r );
   qWarning("Output " + out );
   TextViewOutput->append(out);

//very hacky
   if(out.find("failed") !=-1) {
      r = false;
   }
   return r;
}


bool Qsmb::isMounted(const QString &mountPoint)
{
    struct mntent *me;
    bool mounted = false;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );
    if ( mntfp ){
        while ( (me = getmntent( mntfp )) != 0 ) {
            QString deviceName = me->mnt_fsname;
            QString mountDir = me->mnt_dir;
            QString fsType = me->mnt_type;
            if( fsType == "smbfs" && (mountDir.find(mountPoint) != -1 | deviceName.find(mountPoint) != -1))
               mounted = true;
        }
    }
    endmntent( mntfp );
    return mounted;
}

QString Qsmb::getMount(const QString &shareName)
{
   struct mntent *me;
    QString mount;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );
    if ( mntfp ){
        while ( (me = getmntent( mntfp )) != 0 ) {
            QString deviceName = me->mnt_fsname;
            QString mountDir = me->mnt_dir;
            QString fsType = me->mnt_type;
            if( fsType == "smbfs" &&  deviceName.find(shareName) != -1)
               mount = mountDir;
        }
    }
    endmntent( mntfp );
    return mount;
}

void Qsmb::TextViewClicked(QListViewItem* item)
{
   if(item == NULL) return;

   QString text = item->text(2);
   qWarning(text);
   if( !text.isEmpty())
      mountpt->insertItem(text,0);
}
