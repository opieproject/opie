#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qlistbox.h>
#include <qfile.h>

#include <qpe/resource.h>
#include <opie2/ocontactaccess.h>
#include <opie2/ocontact.h>


#include <stdlib.h>

#include "composemail.h"

AddressPicker::AddressPicker( QWidget *parent, const char *name, bool modal, WFlags flags ) 
    : AddressPickerUI( parent, name, modal, flags )
{
    okButton->setIconSet( Resource::loadPixmap( "enter" ) );
    cancelButton->setIconSet( Resource::loadPixmap( "editdelete" ) );

    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(close()));
    Opie::OContactAccess::List::Iterator it;

    QString lineEmail, lineName, contactLine;
    /* what name has to set here???? */
    Opie::OContactAccess m_contactdb("opiemail");
    
    QStringList mails;
    QString pre,suf;
    Opie::OContactAccess::List m_list = m_contactdb.sorted( true, 0, 0, 0 );
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ((*it).defaultEmail().length()!=0) {
            mails = (*it).emailList();
            if ((*it).fileAs().length()>0) {
                pre = "\""+(*it).firstName()+" "+(*it).lastName()+"\" <";
                suf = ">";
            } else {
                pre = "";
                suf = "";
            }
            QStringList::ConstIterator sit = mails.begin();
            for (;sit!=mails.end();++sit) {
                contactLine=pre+(*sit)+suf;
                addressList->insertItem(contactLine);
            }            
        }
    }
	if ( addressList->count() <= 0 ) {
#if 0
    // makes this realy sense??
		addressList->insertItem( 
            tr( "There are no entries in the addressbook." ) );
#endif
		addressList->setEnabled( false );
		okButton->setEnabled( false );
	} else {
//        addressList->sort();
    }
}

void AddressPicker::accept()
{
	QListBoxItem *item = addressList->firstItem();
	QString names;

	while ( item ) {
		if ( item->selected() )
			names += item->text() + ", ";
		item = item->next();
	}
	names.replace( names.length() - 2, 2, "" );

	if ( names.isEmpty() ) {
		QMessageBox::information(this, tr("Error"), tr("<p>You have to select"
			" at least one address entry.</p>"), tr("Ok"));
		return;
	}

	selectedNames = names;
	QDialog::accept();
}

QString AddressPicker::getNames()
{
	QString names = 0;
	
	AddressPicker picker(0, 0, true);
	picker.showMaximized();
	picker.show();

	int ret = picker.exec();
	if ( QDialog::Accepted == ret ) {
		return picker.selectedNames;
	}

	return 0;
}

