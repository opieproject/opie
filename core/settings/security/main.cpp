#include "multiauthconfig.h"
#include <opie2/oapplication.h>
#include <opie2/odebug.h>



int main(int argc, char **argv) {
    Opie::Core::OApplication app(argc, argv, "MultiAuthentication Config");
    // protect this dialog if option set
    Config* pcfg = new Config("Security");
    pcfg->setGroup( "Misc" );
    bool protectConfigDialog = ! pcfg->readBoolEntry("noProtectConfig", true);
    delete pcfg;
    bool show = true;
    if ( protectConfigDialog )
    {
        if (Opie::Security::Internal::runPlugins() != 0)
        {
            // authentication failed
            show = false;
        }
    }
    if ( show == true ){
        MultiauthConfig dialog;
        app.setMainWidget(&dialog);

        if ( dialog.exec() == QDialog::Accepted ) {
            // write the general, login and sync config
            dialog.writeConfig();
            // call writeConfig() on each plugin config widget
            Opie::Security::MultiauthConfigWidget *confWidget;
            for ( confWidget = dialog.configWidgetList.first(); confWidget != 0;
                  confWidget = dialog.configWidgetList.next() ) {
                confWidget->writeConfig();
            }
        }
        dialog.close();
        app.quit();
        return 0;
    } else {
        owarn << "authentication failed, not showing opie-security" << oendl;
        return 1;
    }
}
