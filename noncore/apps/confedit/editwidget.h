#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QListViewItem;
class ListViewItemConfigEntry;
class QTimer;
class QPopupMenu;

class EditWidget : public QWidget
{ 
    Q_OBJECT

public:
    EditWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~EditWidget();

		QLabel *TextLabelKey;
    QLabel *TextLabelValue;
    QLabel *TextFileName;
    QLineEdit *LineEditGroup;
    QLineEdit *LineEditKey;
    QLineEdit *LineEditValue;
    void isKey(bool h);
protected:
    QGridLayout* EditWidgetLayout;

private:
};

#endif // EDITWIDGET_H
