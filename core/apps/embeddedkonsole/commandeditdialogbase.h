/****************************************************************************
** Form interface generated from reading ui file 'commandeditdialogbase.ui'
**
** Created: Sun Feb 3 15:23:28 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef COMMANDEDITDIALOGBASE_H
#define COMMANDEDITDIALOGBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class PlayListSelection;
class QLabel;
class QListView;
class QListViewItem;
class QToolButton;

class CommandEditDialogBase : public QDialog
{ 
    Q_OBJECT

public:
    CommandEditDialogBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~CommandEditDialogBase();

    QLabel* TextLabel1;
    QToolButton* ToolButton1;
    QToolButton* ToolButton2;
    QToolButton* ToolButton3;
    QToolButton* ToolButton4;
    QToolButton* ToolButton5;
    QLabel* TextLabel1_2;
    PlayListSelection* m_PlayListSelection;
    QListView* m_SuggestedCommandList;

protected:
    QGridLayout* CommandEditDialogBaseLayout;
    QVBoxLayout* Layout2;
};

#endif // COMMANDEDITDIALOGBASE_H
