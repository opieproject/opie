//comandeditdialog.h
#ifndef COMMAND_EDIT_DIALOG_H
#define COMMAND_EDIT_DIALOG_H
#include "commandeditdialogbase.h"
class CommandEditDialog : public CommandEditDialogBase
{
Q_OBJECT

public:

  CommandEditDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
  ~CommandEditDialog();

protected:
  void accept();
  
public slots:
  void showEditDialog();
  void showAddDialog();
  
signals:
  void commandsEdited();  
};

#endif
