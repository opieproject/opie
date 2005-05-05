#include "TableDialog.h"

CTableDialog::CTableDialog(const QFont& f, const QString& tabtext, bool fs, QWidget* parent, const char* name) : QDialog(parent, name, true)
{
  setCaption("Table View");
  QVBoxLayout *tmp = new QVBoxLayout(this);
  QTextView* qtv = new QTextView(this);
  qtv->setFont(f);
  tmp->addWidget(qtv);
  qtv->setText(tabtext);
  if (fs) showMaximized();
  else if (parent != NULL)
    {
      resize(parent->size());
      move(parent->pos());
    }
}
