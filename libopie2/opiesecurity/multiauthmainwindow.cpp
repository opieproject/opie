#include "multiauthmainwindow.h"

#include "multiauthcommon.h"
#include <qpe/config.h>

namespace Opie {
namespace Security {

/// Initializes widgets according to allowBypass argument (false by default) and explanScreens config
/**
 * \note if allowBypass is true, we will show explanatory screens anyway
 */
MultiauthMainWindow::MultiauthMainWindow(bool allowBypass = false)
    : QDialog(0, "main Opie multiauth modal dialog", TRUE,
              Qt::WStyle_NoBorder | Qt::WStyle_Customize | Qt::WStyle_StaysOnTop)

{
    alreadyDone = false;
    // initializes widget pointers which not always point to an object
    quit = 0;
    message2 = 0;

    if (allowBypass == true)
        explanScreens = true;
    else
    {
        Config *pcfg = new Config("Security");
        pcfg->setGroup("Misc");
        explanScreens = pcfg->readBoolEntry("explanScreens", true);
        delete pcfg;
    }

    layout = new QVBoxLayout(this);
    layout->setSpacing(11);
    layout->setMargin(11);
    layout->setAlignment( Qt::AlignTop );

    // if explanScreens is false, we don't show any text in the QDialog,
    // and we proceed directly
    if ( explanScreens == true )
    {
        title = new QLabel("<center><h1>" + tr("Welcome to Opie Multi-authentication Framework") + "</h1></center>", this);
        message = new QLabel("<center><h3>" + tr("Launching authentication plugins...") + "</h3></center>", this);
    } else {
        title = new QLabel("", this);
        message = new QLabel("", this);
    }

    layout->addWidget(title);
    layout->addWidget(message);
    proceedButton = new QPushButton(tr("Proceed..."), this);
    layout->addWidget(proceedButton, 0, Qt::AlignHCenter);

    QObject::connect(proceedButton, SIGNAL(clicked()), this, SLOT(proceed()));

    if ( explanScreens == true )
    {
        quit = new QPushButton("Exit", this);
        layout->addWidget(quit, 0, Qt::AlignHCenter);
        if ( allowBypass == true )
        {
            // very important: we can close the widget through the quit button, and bypass authentication, only if allowBypass is set!
            message2 = new QLabel("<center><i>" + tr("Note: this 'exit' button only appears during <b>simulations</b>, like the one we are in. If you don't succeed an authentication step at some point, it will keep on asking you to authenticate, but remember you can <b>skip</b> it too (and have access to this button again).") + "</i></center>", this);
            layout->addWidget(message2);
            QObject::connect(quit, SIGNAL(clicked()), this, SLOT(close()));
        }
        else
        {
            quit->hide();
        }

    }
    else
    {
        // we will need this button only if runPlugins() fails in proceed()
        proceedButton->hide();
        // let's proceed now
        proceed();
    }
}

/// nothing to do
MultiauthMainWindow::~MultiauthMainWindow() {
}

/// launch the authentication
void MultiauthMainWindow::proceed() {
    int result = Internal::runPlugins();


    if ( (result == 0) && !explanScreens )
    {
        // the authentication has succeeded, we can exit directly
        // this will work if we haven't been called by the constructor of MultiauthMainWindow
        close();
        // and if we've been called by this constructor, we use this variable to tell our
        // caller we're already done
        alreadyDone = true;
        return;
    }
    else
    {

        proceedButton->setText("Another try?");
        QString resultMessage;

        if (result == 0)
        {
            // authentication has succeeded, adapt interface then
            message->setText( "<center><h3>" + tr("Congratulations! Your authentication has been successful.") + "</h3></center>" );
            quit->setText("Enter Opie");
            if ( quit->isHidden() )
            {
                // that means we don't allow to bypass, but now we can show and connect this button
                QObject::connect(quit, SIGNAL(clicked()), this, SLOT(close()));
                quit->show();
            } else {
                if ( message2 != 0 ) message2->hide();
            }
        }
        else
        {
            // authentication has failed, explain that according to allowBypass
            message->setText( "<center><h3>" + tr("You have <b>not</b> succeeded enough authentication steps!") + "</h3></center>" );
            proceedButton->show();
            if ( allowBypass == true )
            {
                message2->setText( "<center><p>" + tr("Be careful: if this was not a <b>simulation</b>, you would have to go back through all the steps now.") + "</p></center>" );
                message2->show();
        }
    }
}
}

/** When we don't show explanatory screens and we succeed authentication,
 * as early as during the proceed() call of the constructor, the caller must know
 * (through this function) authentication has already been succeeded..
 * \todo try to avoid this hack?
 */
bool MultiauthMainWindow::isAlreadyDone() {
    return alreadyDone;
}

}
}
