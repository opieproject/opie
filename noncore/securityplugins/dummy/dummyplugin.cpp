#include "dummyplugin.h"

#include <opie2/oapplication.h>

#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

/// Simply return its name (Dummy plugin)
QString DummyPlugin::pluginName() const {
    return "Dummy plugin";
}

/// no configuration widget for the moment
MultiauthConfigWidget * DummyPlugin::configWidget(QWidget * parent) {
    return 0l;
}
QString DummyPlugin::pixmapNameWidget() const {
    return "security/dummyplugin";
}
QString DummyPlugin::pixmapNameConfig() const {
    return 0l;
}

/// Emit the MultiauthPluginObject::Success emitCode
void DummyPlugin::success() {
    emit emitCode(MultiauthPluginObject::Success);
}

/// Emit the MultiauthPluginObject::Failure emitCode
void DummyPlugin::failure() {
    emit emitCode(MultiauthPluginObject::Failure);
}

/// Emit the MultiauthPluginObject::Skip emitCode
void DummyPlugin::skip() {
    emit emitCode(MultiauthPluginObject::Skip);
}

/// Make one authentication attempt with this plugin
/**
 * (very simple "success" / "failure" buttons in a dialog)
 * \return The outcome code of this  authentication
 */
int DummyPlugin::authenticate() {

    /* Standard, inescapable authentication dialog
     */
    QDialog dummyDialog(0,
                        "Dummy dialog",
                        TRUE,
                        Qt::WStyle_NoBorder | Qt::WStyle_Customize | Qt::WStyle_StaysOnTop);

    QRect desk = oApp->desktop()->geometry();
    dummyDialog.setGeometry( 0, 0, desk.width(), desk.height() );
    
    /* Creation of the particular widgets of our Dummy user interface
     * Note: we have to resize the VBox to the QDialog size, since it's not
     * done automatically.
     */
    QVBoxLayout layout(&dummyDialog);
    layout.setSpacing(11);
    layout.setMargin(11);
    layout.setAlignment( Qt::AlignTop );
    
    QLabel title("<center><h1>" + tr("\"Dummy\" <br />demonstration plugin") + "</h1></center>", &dummyDialog);
    QLabel subTitle("<center><h2>" +tr("You can simulate the following outcomes:") + "</h2></center>", &dummyDialog);
    layout.addWidget(&title);
    layout.addWidget(&subTitle);

    QHBoxLayout hl(&layout);
    QPushButton pbSuccess("Success", &dummyDialog);
    QPushButton pbSkip("Skip", &dummyDialog);
    QPushButton pbFailure("Failure", &dummyDialog);
    hl.addWidget(&pbSuccess, 0, Qt::AlignHCenter);
    hl.addWidget(&pbSkip, 0, Qt::AlignHCenter);
    hl.addWidget(&pbFailure, 0, Qt::AlignHCenter);
    
    /* Linking our pushbuttons to exit functions
     * (each result button here has a corresponding slot)
     */
    QObject::connect(&pbSuccess, SIGNAL(clicked()), this, SLOT(success()));
    QObject::connect(&pbFailure, SIGNAL(clicked()), this, SLOT(failure()));
    QObject::connect(&pbSkip, SIGNAL(clicked()), this, SLOT(skip()));
    
    /* The value of the signal these three slots will emit corresponds to
     * the different values we want to return
     */
    QObject::connect(this, SIGNAL(emitCode(int)), &dummyDialog, SLOT(done(int)));
    return dummyDialog.exec();
}
