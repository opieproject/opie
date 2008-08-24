#include "multiauthplugininterface.h"
#include "multiauthcommon.h"

/* Opie */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/ocontactaccessbackend_vcard.h>
#include <opie2/ocontactaccess.h>
#include <opie2/oresource.h>

/* Qt */
#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/qcom.h>
#include <qtextview.h>
#include <qdir.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* UNIX */
#include <unistd.h>
#include <qpe/config.h>

namespace Opie {
namespace Security {

SecOwnerDlg::SecOwnerDlg( QWidget *parent, const char * name,
                          bool modal, bool fullscreen = FALSE )
: QDialog( parent, name, modal,
           fullscreen ?
           WStyle_NoBorder | WStyle_Customize | WStyle_StaysOnTop : 0 )
{
    if ( !name )
        setName( "OwnerInfoDialog" );

    if ( fullscreen ) {
        QRect desk = qApp->desktop()->geometry();
        setGeometry( 0, 0, desk.width(), desk.height() );
    }

    OwnerInfoDialogLayout = new QVBoxLayout( this );
    OwnerInfoDialogLayout->setSpacing( 6 );
    OwnerInfoDialogLayout->setMargin( 11 );

    Layout5 = new QHBoxLayout;
    Layout5->setSpacing( 6 );
    Layout5->setMargin( 0 );

    // I'd rather this be laid out with the Owner label on the left, however 
    // there is apparently something in the top left of the dialog that
    // covers the text. I couldn't figure out the cause so I had no choice 
    // but to align the label to the right so it is at least readable.
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout5->addItem( spacer );
    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Minimum, false ) );
    QFont f( TextLabel1->font() );
    f.setPointSize(18);
    f.setBold(true);
    TextLabel1->setFont(f);
    TextLabel1->setMinimumSize( QSize( 0, 20 ) );
    TextLabel1->setText( tr( "Owner" ) + "  " );
    TextLabel1->setAlignment( AlignVCenter | AlignRight ); 
    Layout5->addWidget( TextLabel1 );

    pxIcon = new QLabel( this, "pxIcon" );
    pxIcon->setScaledContents( FALSE );
    pxIcon->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout5->addWidget( pxIcon );
    OwnerInfoDialogLayout->addLayout( Layout5 );

    Layout8 = new QGridLayout;
    Layout8->setSpacing( 6 );
    Layout8->setMargin( 0 );

    lbHomePhoneLabel = new QLabel( this, "lbHomePhoneLabel" );
    QFont lbHomePhoneLabel_font(  lbHomePhoneLabel->font() );
    lbHomePhoneLabel_font.setBold( TRUE );
    lbHomePhoneLabel->setFont( lbHomePhoneLabel_font );
    lbHomePhoneLabel->setText( tr( "Home:" ) );

    Layout8->addWidget( lbHomePhoneLabel, 1, 0 );

    lbEmail = new QLabel( this, "lbEmail" );

    Layout8->addWidget( lbEmail, 7, 1 );

    lbNameLabel = new QLabel( this, "lbNameLabel" );
    QFont lbNameLabel_font(  lbNameLabel->font() );
    lbNameLabel_font.setBold( TRUE );
    lbNameLabel->setFont( lbNameLabel_font ); 
    lbNameLabel->setText( tr( "Name:" ) );

    Layout8->addWidget( lbNameLabel, 0, 0 );

    lbHomeMobile = new QLabel( this, "lbHomeMobile" );

    Layout8->addWidget( lbHomeMobile, 3, 1 );

    lbHomeAddressLabel = new QLabel( this, "lbHomeAddressLabel" );
    lbHomeAddressLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)1, lbHomeAddressLabel->sizePolicy().hasHeightForWidth() ) );
    QFont lbHomeAddressLabel_font(  lbHomeAddressLabel->font() );
    lbHomeAddressLabel_font.setBold( TRUE );
    lbHomeAddressLabel->setFont( lbHomeAddressLabel_font ); 
    lbHomeAddressLabel->setText( tr( "Address:" ) );
    lbHomeAddressLabel->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

    Layout8->addWidget( lbHomeAddressLabel, 5, 0 );

    lbWorkAddress = new QLabel( this, "lbWorkAddress" );
    lbWorkAddress->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, false ) );
    lbWorkAddress->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

    Layout8->addWidget( lbWorkAddress, 6, 1 );

    lbEmailLabel = new QLabel( this, "lbEmailLabel" );
    QFont lbEmailLabel_font(  lbEmailLabel->font() );
    lbEmailLabel_font.setBold( TRUE );
    lbEmailLabel->setFont( lbEmailLabel_font ); 
    lbEmailLabel->setText( tr( "Email:" ) );

    Layout8->addWidget( lbEmailLabel, 7, 0 );

    lbWorkAddressLabel = new QLabel( this, "lbWorkAddressLabel" );
    lbWorkAddressLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)1, lbWorkAddressLabel->sizePolicy().hasHeightForWidth() ) );
    QFont lbWorkAddressLabel_font(  lbWorkAddressLabel->font() );
    lbWorkAddressLabel_font.setBold( TRUE );
    lbWorkAddressLabel->setFont( lbWorkAddressLabel_font ); 
    lbWorkAddressLabel->setText( tr( "Address:" ) );
    lbWorkAddressLabel->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

    Layout8->addWidget( lbWorkAddressLabel, 6, 0 );

    lbWorkPhoneLabel = new QLabel( this, "lbWorkPhoneLabel" );
    QFont lbWorkPhoneLabel_font(  lbWorkPhoneLabel->font() );
    lbWorkPhoneLabel_font.setBold( TRUE );
    lbWorkPhoneLabel->setFont( lbWorkPhoneLabel_font ); 
    lbWorkPhoneLabel->setText( tr( "Work:" ) );

    Layout8->addWidget( lbWorkPhoneLabel, 2, 0 );

    lbHomeAddress = new QLabel( this, "lbHomeAddress" );
    lbHomeAddress->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, false ) );
    lbHomeAddress->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

    Layout8->addWidget( lbHomeAddress, 5, 1 );

    lbWorkMobile = new QLabel( this, "lbWorkMobile" );

    Layout8->addWidget( lbWorkMobile, 4, 1 );

    lbHomePhone = new QLabel( this, "lbHomePhone" );

    Layout8->addWidget( lbHomePhone, 1, 1 );

    lbWorkMobileLabel = new QLabel( this, "lbWorkMobileLabel" );
    QFont lbWorkMobileLabel_font(  lbWorkMobileLabel->font() );
    lbWorkMobileLabel_font.setBold( TRUE );
    lbWorkMobileLabel->setFont( lbWorkMobileLabel_font ); 
    lbWorkMobileLabel->setText( tr( "Mobile:" ) );

    Layout8->addWidget( lbWorkMobileLabel, 4, 0 );

    lbName = new QLabel( this, "lbName" );

    Layout8->addWidget( lbName, 0, 1 );

    lbWorkPhone = new QLabel( this, "lbWorkPhone" );

    Layout8->addWidget( lbWorkPhone, 2, 1 );

    lbHomeMobileLabel = new QLabel( this, "lbHomeMobileLabel" );
    QFont lbHomeMobileLabel_font(  lbHomeMobileLabel->font() );
    lbHomeMobileLabel_font.setBold( TRUE );
    lbHomeMobileLabel->setFont( lbHomeMobileLabel_font ); 
    lbHomeMobileLabel->setText( tr( "Mobile:" ) );

    Layout8->addWidget( lbHomeMobileLabel, 3, 0 );
    OwnerInfoDialogLayout->addLayout( Layout8 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    OwnerInfoDialogLayout->addItem( spacer_2 );

    lbMessage = new QLabel( this, "lbMessage" );
    lbMessage->setAlignment( AlignLeft | AlignVCenter | ExpandTabs | WordBreak );
    OwnerInfoDialogLayout->addWidget( lbMessage );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    OwnerInfoDialogLayout->addItem( spacer_3 );

    pbOK = new QPushButton( this, "pbOK" );
    pbOK->setMinimumSize( QSize( 0, 40 ) );
    pbOK->setText( tr( "OK" ) );
    OwnerInfoDialogLayout->addWidget( pbOK );

    pxIcon->setPixmap( Opie::Core::OResource::loadPixmap( "security/ownerinfo", Opie::Core::OResource::BigIcon ) );

    connect( pbOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
}



namespace Internal {
/// run plugins until we reach nbSuccessMin successes
int runPlugins() {

    Config config("Security");
    config.setGroup("Plugins");
    QStringList plugins = config.readListEntry("IncludePlugins", ',');
    /* if there are no configured plugins, we simply return 0 to
     * let the user in:
     */
    if (plugins.isEmpty() == true) {
        owarn << "No authentication plugin has been configured yet!" << oendl;
        odebug << "Letting the user in..." << oendl;
        return 0;
    }
    config.setGroup("Misc");
    int nbSuccessMin = config.readNumEntry("nbSuccessMin", 1);
    int nbSuccess = 0;

    config.setGroup("OwnerInfo");
    int ownerInfoMode = config.readNumEntry("WhenToShow", oimAuthFail);

    if(ownerInfoMode == oimFirst)
        showOwnerInfo();

    /* tries to launch successively each plugin in $OPIEDIR/plugins/security
     * directory which file name is in Security.conf / [Misc] / IncludePlugins
     */
    QString path = QPEApplication::qpeDir() + "plugins/security";
    QStringList::Iterator libIt;

    for ( libIt = plugins.begin(); libIt != plugins.end(); ++libIt ) {
        QInterfacePtr<MultiauthPluginInterface> iface;
        QLibrary *lib = new QLibrary( path + "/" + *libIt );

        if ( lib->queryInterface(
                                 IID_MultiauthPluginInterface,
                                 (QUnknownInterface**)&iface ) == QS_OK )
        {
            // the plugin is a true Multiauth plugin
            odebug << "Accepted plugin: " << QString( path + "/" + *libIt ) << oendl;
            odebug << "Plugin name: " << iface->plugin()->pluginName() << oendl;

            int resultCode;
            int tries = 0;

            // perform authentication
            resultCode = iface->plugin()->authenticate();

            // display the result in command line
            QString resultMessage;
            switch (resultCode)
            {
                case MultiauthPluginObject::Success:
                    resultMessage = "Success!";
                    nbSuccess++;
                    break;
                case MultiauthPluginObject::Failure:
                    resultMessage = "Failure...";
                    break;
                case MultiauthPluginObject::Skip:
                    resultMessage = "Skip";
                    break;
            }
            odebug << "Plugin result: " << resultMessage << oendl;

            // if failure, wait, reperform, wait, reperform... until right
            while (resultCode == MultiauthPluginObject::Failure)
            {
                tries++;
                owarn << "This plugin has failed " << tries << " times already" << oendl;

                if(ownerInfoMode == oimAuthFail)
                    showOwnerInfo();

                /// \todo parametrize the time penalty according to \em mode (exponential,
                /// linear or fixed) and \em basetime (time penalty for the first failure)
                sleep(2 * tries);

                // perform authentication
                resultCode = iface->plugin()->authenticate();

                // display the result in command line
                switch (resultCode)
                {
                    case MultiauthPluginObject::Success:
                        resultMessage = "Success!";
                        nbSuccess++;
                        break;
                    case MultiauthPluginObject::Failure:
                        resultMessage = "Failure...";
                        break;
                    case MultiauthPluginObject::Skip:
                        resultMessage = "Skip";
                        break;
                }
                odebug << "Plugin result: " << resultMessage << oendl;
            }
            delete lib;

            if (resultCode == MultiauthPluginObject::Success && nbSuccess == nbSuccessMin)
            {
                // we have reached the required number of successes, we can exit the plugin loop
                return 0;
            }
        } else {
            owarn << "Could not recognize plugin " << QString( path + "/" + *libIt ) << oendl;
            delete lib;
        } // end if plugin recognized
    } //end for
    return 1;
}

void showOwnerInfo()
{
    SecOwnerDlg ownerInfoDialog(0, "Owner info dialog", TRUE, TRUE);

    QLabel ownerLabel(&ownerInfoDialog);
    QString vfilename = Global::applicationFileName("addressbook", "businesscard.vcf");
    Opie::OPimContactAccess acc( "today", vfilename,
                                 new Opie::OPimContactAccessBackend_VCard("today", vfilename ) );

    if ( acc.load() ) {
        Opie::OPimContact cont = acc.allRecords()[0];

        Config config("Security");
        config.setGroup("OwnerInfo");

        // Name
        ownerInfoDialog.lbName->setText( cont.fullName() );

        // Home phone
        if ( config.readBoolEntry("ShowHomePhone", TRUE ) ) {
            ownerInfoDialog.lbHomePhone->setText( cont.homePhone() );
        }
        else {
            ownerInfoDialog.lbHomePhone->hide();
            ownerInfoDialog.lbHomePhoneLabel->hide();
        }

        // Home mobile
        if ( config.readBoolEntry("ShowHomeMobile", TRUE ) ) {
            ownerInfoDialog.lbHomeMobile->setText( cont.homeMobile() );
        }
        else {
            ownerInfoDialog.lbHomeMobile->hide();
            ownerInfoDialog.lbHomeMobileLabel->hide();
        }

        // Work phone
        if ( config.readBoolEntry("ShowWorkPhone", TRUE ) ) {
            ownerInfoDialog.lbWorkPhone->setText( cont.businessPhone() );
        }
        else {
            ownerInfoDialog.lbWorkPhone->hide();
            ownerInfoDialog.lbWorkPhoneLabel->hide();
        }

        // Work mobile
        if ( config.readBoolEntry("ShowWorkMobile", TRUE ) ) {
            ownerInfoDialog.lbWorkMobile->setText( cont.businessMobile() );
        }
        else {
            ownerInfoDialog.lbWorkMobile->hide();
            ownerInfoDialog.lbWorkMobileLabel->hide();
        }

        // Home Address
        if ( config.readBoolEntry("ShowHomeAddress", FALSE ) ) {
            ownerInfoDialog.lbHomeAddress->setText( cont.displayHomeAddress() );
        }
        else {
            ownerInfoDialog.lbHomeAddress->hide();
            ownerInfoDialog.lbHomeAddressLabel->hide();
        }

        // Work Address
        if ( config.readBoolEntry("ShowWorkAddress", FALSE ) ) {
            QString addr = cont.company();
            if (addr != "")
                addr += '\n';
            addr += cont.displayBusinessAddress();
            ownerInfoDialog.lbWorkAddress->setText( addr );
        }
        else {
            ownerInfoDialog.lbWorkAddress->hide();
            ownerInfoDialog.lbWorkAddressLabel->hide();
        }

        // Email
        if ( config.readBoolEntry("ShowEmail", TRUE ) ) {
            ownerInfoDialog.lbEmail->setText( cont.defaultEmail() );
        }
        else {
            ownerInfoDialog.lbEmail->hide();
            ownerInfoDialog.lbEmailLabel->hide();
        }

        QString msg = config.readEntry("Message", "" );
        msg = msg.replace( QRegExp("\\\\n"), "\n" );
        ownerInfoDialog.lbMessage->setText( msg );
    }
    else {
        ownerInfoDialog.lbMessage->setText( QObject::tr ( "Owner information has not been entered" ) );
    }
    ownerInfoDialog.exec();
}

}
}
}
