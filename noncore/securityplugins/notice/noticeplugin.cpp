#include "noticeplugin.h"

#include <opie2/oapplication.h>

#include <qmessagebox.h>
#include <qregexp.h>

/// creates and initializes the m_config Config object
NoticePlugin::NoticePlugin() : MultiauthPluginObject(), noticeW(0) {
    m_config = new Config("Security");
    m_config->setGroup("NoticePlugin");
}

/// deletes the m_config Config object and noticeW if necessary
NoticePlugin::~NoticePlugin() {
    delete m_config;
    if (noticeW != 0)
        delete noticeW;
}

/// Simply return its name (Notice plugin)
QString NoticePlugin::pluginName() const {
    return "Notice plugin";
}

/// return the Notice widget configuration widget
/**
 * \return noticeW, the NoticeConfigWidget
 */
MultiauthConfigWidget * NoticePlugin::configWidget(QWidget * parent) {
    if (noticeW == 0)
        noticeW = new NoticeConfigWidget(parent, "Notice configuration widget");
    return noticeW;
}

/// return the path of the small tab icon
QString NoticePlugin::pixmapNameConfig() const {
    return "security/noticeplugin_small";
}

/// return the path of the big icon for the active/order checklist
QString NoticePlugin::pixmapNameWidget() const {
    return "security/noticeplugin";
}

/// Displays the configured message and an 'Accept' button
/**
 * \return the outcome code of this  authentication (can be only success)
 */
int NoticePlugin::authenticate() {
    QMessageBox noticeDialog("Notice plugin",
                            getNoticeText(),
                            QMessageBox::Warning,
                            QMessageBox::Yes,
                            0,
                            0,
                            0,
                            "notice plugin dialog",
                            true,
                            Qt::WStyle_NoBorder | Qt::WStyle_Customize | Qt::WStyle_StaysOnTop);

    noticeDialog.setButtonText(QMessageBox::Yes, "I accept");
    
    QRect desk = oApp->desktop()->geometry();
    noticeDialog.setGeometry( 0, 0, desk.width(), desk.height() );
    
    switch (noticeDialog.exec())
    {
        case QMessageBox::Yes:
            return MultiauthPluginObject::Success;
    }
    return 255; //should not be returned anyway
}

/// get the notice text from our m_config config file (with true new lines)
/**
 * if no text has been defined yet returns defaultNoticeText
 */
QString NoticePlugin::getNoticeText() {
      // Note: C++ processes '\' character, so we have to type \\\\ to mean \\ to QRegExp
      return m_config->readEntry("noticeText", defaultNoticeText).replace( QRegExp("\\\\n"), "\n" );
}

