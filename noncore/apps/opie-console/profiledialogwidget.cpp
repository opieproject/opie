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
