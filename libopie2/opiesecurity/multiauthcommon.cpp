#include "multiauthplugininterface.h"
#include "multiauthcommon.h"

/* Opie */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>

/* Qt */
#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/qcom.h>
#include <qtextview.h>
#include <qdir.h>

/* UNIX */
#include <unistd.h>
#include <qpe/config.h>


SecOwnerDlg::SecOwnerDlg( QWidget *parent, const char * name, Contact c,
                          bool modal, bool fullscreen = FALSE )
: QDialog( parent, name, modal,
           fullscreen ?
           WStyle_NoBorder | WStyle_Customize | WStyle_StaysOnTop : 0 )
{
    if ( fullscreen ) {
        QRect desk = qApp->desktop()->geometry();
        setGeometry( 0, 0, desk.width(), desk.height() );
    }
    // set up contents.
    QString text("<H3>" + tr("Please contact the owner (directions follow), or try again clicking of this screen (and waiting for the penalty time) if you are the legitimate owner") + "</H3>");
    text += c.toRichText();
    tv = new QTextView(this);
    tv->setText(text);

    tv->viewport()->installEventFilter(this);
}

void SecOwnerDlg::resizeEvent( QResizeEvent * )
{
    tv->resize( size() );
}

bool SecOwnerDlg::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::KeyPress || e->type() == QEvent::MouseButtonPress ) {
        accept();
        return TRUE;
    }
    return QWidget::eventFilter(o, e);
}

void SecOwnerDlg::mousePressEvent( QMouseEvent * ) { accept(); }


/// run plugins until we reach nbSuccessMin successes
int runPlugins() {

    SecOwnerDlg *oi = 0;
    // see if there is contact information.
    QString vfilename = Global::applicationFileName("addressbook",
                                                    "businesscard.vcf");
    if (QFile::exists(vfilename)) {
        Contact c;
        c = Contact::readVCard( vfilename )[0];

        oi = new SecOwnerDlg(0, 0, c, TRUE, TRUE);
    }

    Config config("Security");
    config.setGroup("Plugins");
    QStringList plugins = config.readListEntry("IncludePlugins", ',');
    config.setGroup("Misc");
    int nbSuccessMin = config.readNumEntry("nbSuccessMin", 1);
    int nbSuccess = 0;

    /* tries to launch successively each plugin in $OPIEDIR/plugins/security
     * directory which file name is in Security.conf / [Misc] / IncludePlugins
     */
    QString path = QPEApplication::qpeDir() + "/plugins/security";
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

                // displays owner information, if any
                if (oi)
                {
                    oi->exec();
                    odebug << "Contact information displayed" << oendl;
                }

                /// \todo parametrize the time penalty according to \em mode (exponential,
                /// linear or fixed) and \em basetime (time penalty for the first failure)
                sleep(2 * tries);

                if (oi)
                {
                    oi->hide();
                    /** \todo fix the focus here: should go back to the current plugin widget
                     * but it doesn't, so we have to tap once on the widget before e.g. buttons
                     * are active again
                     */
                    odebug << "Contact information hidden" << oendl;
                }

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
                if(oi) delete oi;
                // we have reached the required number of successes, we can exit the plugin loop
                return 0;
            }
        } else {
            owarn << "Could not recognize plugin " << QString( path + "/" + *libIt ) << oendl;
            delete lib;
        } // end if plugin recognized
    } //end for
    if(oi) delete oi;
    return 1;
}
