#include "noticeConfigWidget.h"

#include <opie2/odebug.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qgroupbox.h>

/// constructs the widget, filling the noticeMLE QMultiLineEdit with the "noticeText" entry
NoticeConfigWidget::NoticeConfigWidget(QWidget* parent = 0, const char* name = "Notice configuration widget") : MultiauthConfigWidget(parent, name)
{
    QVBoxLayout *baseLayout = new QVBoxLayout( this);
    baseLayout->setSpacing(11);
    baseLayout->setMargin(6);
    baseLayout->setAlignment( Qt::AlignTop );

    QGroupBox *configBox = new QGroupBox(0, Qt::Vertical, tr("Set the message the user must accept"), this);
    baseLayout->addWidget(configBox);
    QVBoxLayout *boxLayout = new QVBoxLayout( configBox->layout() );

    QLabel * comment1 = new QLabel("<p><em>" + tr("You may want to consult your legal department for proper wording here.") + "</em></p>", configBox);
    boxLayout->addWidget(comment1);

    // Set the multilineedit box text to getNoticeText()
    noticeMLE = new QMultiLineEdit(configBox, "notice text");
    noticeMLE->setWordWrap(QMultiLineEdit::WidgetWidth);
    noticeMLE->setFocus();
    noticeMLE->setText(getNoticeText());
    boxLayout->addWidget(noticeMLE);

    resetNoticeButton = new QPushButton( tr("Reset notice to default"), configBox, "reset Notice Button" );
    connect(resetNoticeButton, SIGNAL( clicked() ), this, SLOT( resetNotice() ));
    boxLayout->addWidget(resetNoticeButton, 0, Qt::AlignHCenter);

    QLabel * comment2 = new QLabel("<p>" + tr("Note: you can use HTML tags to improve its layout (example: text between &lt;em&gt; and &lt;/em&gt; will be <em>emphasized</em>)") + "</p>", configBox);
    boxLayout->addWidget(comment2);

}

/// nothing to do
NoticeConfigWidget::~NoticeConfigWidget()
{}

/// write the notice text in the multiauth.conf Config file
void NoticeConfigWidget::writeConfig()
{
    if ( noticeMLE->edited() ) { 
        odebug << "writing new notice text in Security.conf" << oendl;
        setNoticeText(noticeMLE->text());
    }
}

/// reset the notice text to the hard-coded example defaultNoticeText
void NoticeConfigWidget::resetNotice()
{
    noticeMLE->setText(defaultNoticeText);
}

/// get the notice text from the config file (with true new lines)
/**
 * if no text has been defined yet returns defaultNoticeText
 */
QString NoticeConfigWidget::getNoticeText() {
    m_config = new Config("Security");
    m_config->setGroup("NoticePlugin");
    // Note: C++ processes '\' character, so we have to type \\\\ to mean \\ to QRegExp
    QString noticeText = m_config->readEntry("noticeText", defaultNoticeText).replace( QRegExp("\\\\n"), "\n" );
    delete m_config;
    return noticeText;
}

/// set the notice text in our m_config config file (escaping new lines)
void NoticeConfigWidget::setNoticeText(QString noticeText) {
    m_config = new Config("Security");
    m_config->setGroup("NoticePlugin");
    // since Config files do not allow true newlines, we replace them with litteral "\n"
    m_config->writeEntry("noticeText", noticeText.replace( QRegExp("\n"), "\\n" ));
    delete m_config;
}
