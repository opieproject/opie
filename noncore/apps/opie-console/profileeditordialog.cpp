
#include "profileeditordialog.h"


ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact,
                                          const Profile& prof )
    : QDialog(0, 0, TRUE), m_fact( fact ), m_prof( prof )
{
    initUI();
    /* now set the widgets */
}
ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact )
    : QDialog(0, 0, TRUE), m_fact( fact )
{

}
ProfileEditorDialog::~ProfileEditorDialog() {

}

