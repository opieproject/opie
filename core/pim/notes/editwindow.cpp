#include "editwindow.h"

editWindowWidget::editWindowWidget( QWidget* parent,  const char* name, bool modal, WFlags fl ) : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Information:" ) );
    QGridLayout *gridLayout = new QGridLayout(this, 1, 1, 5, 5);
    editArea = new QMultiLineEdit(this, "editArea");
    gridLayout->addWidget(editArea, 0, 0);
    editArea->setWordWrap(QMultiLineEdit::WidgetWidth);

    showMaximized();
}

void editWindowWidget::loadFile(QString fileName)
{
    QFileInfo fileinfo(fileName);
    setCaption(fileinfo.fileName());

    QFile file(fileName);

    if (file.exists()) 
    {
        if (!file.open(IO_ReadOnly)) 
        {
            QMessageBox::warning(0, tr("File i/o error"), fileName.sprintf(tr("Could not read file '%s'"), fileName));
        } 
        else 
        {
            QTextStream inStream(&file);
            inStream.setEncoding(QTextStream::UnicodeUTF8);
            editArea->setText(inStream.read());
            file.close();
        }
    }
}

void editWindowWidget::saveFile(QString fileName)
{
    QFile file(fileName);

    if(!file.open(IO_WriteOnly)) 
    {
        QMessageBox::warning(0, tr("File i/o error"), fileName.sprintf(tr("Could not write file '%s'"), fileName));
    }
    else 
    {
        QTextStream outStream(&file);
        outStream.setEncoding(QTextStream::UnicodeUTF8);
        outStream << editArea->text();
        file.close();
        this->accept();
    }
}

editWindowWidget::~editWindowWidget()
{
}
