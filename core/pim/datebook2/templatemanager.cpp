#include "templatemanager.h"

using namespace Datebook;


TemplateManager::TemplateManager() {

}
TemplateManager::~TemplateManager() {
}
bool TemplateManager::doSave() {
    return true;
}
bool TemplateManager::doLoad() {
    return true;
}

TemplateDialog::TemplateDialog( const TemplateManager& )
    : QDialog(0, 0, true ) {
}
TemplateDialog::~TemplateDialog() {
}
TemplateManager TemplateDialog::manager()const {
    return TemplateManager();
}
