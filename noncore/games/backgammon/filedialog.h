#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qstring.h>


class FileDialog : public QDialog
{
    Q_OBJECT
private:
    QListView* dirselector;
    QLineEdit* fileinput;
	QString ext;
	QString file_name;

public:
    FileDialog(QWidget* parent,QString header,QString extension,const char* name=0,bool modal=TRUE,WFlags f=0);
    ~FileDialog();
private slots:
	void selectorclicked(QListViewItem* entry);
private:
    void getCurrentDir();
public:
    QString filename();
};

#endif //FILEWIDGET_H
