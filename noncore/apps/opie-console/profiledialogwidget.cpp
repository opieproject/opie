#include <qlayout.h>
#include <qlabel.h>

#include "profiledialogwidget.h"

ProfileDialogWidget::ProfileDialogWidget( const QString&, QWidget* parent,
                                          const char* name )
    : QWidget( parent, name ) {
}
ProfileDialogWidget::~ProfileDialogWidget() {
}

ProfileDialogTerminalWidget::ProfileDialogTerminalWidget( const QString& na,
                                                          QWidget* parent,
                                                          const char* name )
    : ProfileDialogWidget( na, parent, name )
{
}
ProfileDialogTerminalWidget::~ProfileDialogTerminalWidget() {
}
ProfileDialogWidget::Type ProfileDialogTerminalWidget::type()const {
    return Terminal;
}

ProfileDialogConnectionWidget::ProfileDialogConnectionWidget( const QString& na,
                                                              QWidget* parent,
                                                              const char* name )
    : ProfileDialogWidget(na, parent, name )
{
}
ProfileDialogConnectionWidget::~ProfileDialogConnectionWidget() {
}
ProfileDialogWidget::Type ProfileDialogConnectionWidget::type()const {
    return Connection;
}
ProfileDialogKeyWidget::ProfileDialogKeyWidget( const QString &na,
                                                QWidget *parent,
                                                const char *name)
    : ProfileDialogWidget(na, parent, name )
{
}
ProfileDialogKeyWidget::~ProfileDialogKeyWidget() {
}
ProfileDialogWidget::Type ProfileDialogKeyWidget::type() const{
    return Keyboard;
}

NoOptions::NoOptions( const QString& name, QWidget* parent, const char* na )
    : ProfileDialogWidget( name, parent, na ) {
    QHBoxLayout* lay = new QHBoxLayout(this);
    QLabel* lbl = new QLabel( this );
    lbl->setText( tr("This Plugin does not support any configurations") );
    lbl->setTextFormat( RichText );

    lay->addWidget( lbl );
}
void NoOptions::load( const Profile& ) {

}
void NoOptions::save( Profile& ) {

}
