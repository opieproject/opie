#ifndef __CANNOEDIT_H
#define __CANNOEDIT_H
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmultilineedit.h>

class CAnnoEdit : public QWidget
{
    Q_OBJECT

    QLineEdit* m_name;
    QMultiLineEdit* m_anno;
    size_t m_posn;
 public:
    void setPosn(size_t p) { m_posn = p; }
    size_t getPosn() { return m_posn; }
    void setName(const QString& name)
	{
	    m_name->setText(name);
	}
    void setAnno(const QString& name)
	{
	    m_anno->setText(name);
	    m_anno->setEdited(false);
	}
    bool edited() { return m_anno->edited(); }
    CAnnoEdit(QWidget *parent=0, const char *name=0, WFlags f = 0) :
	QWidget(parent, name, f)
	{
	    QVBoxLayout* grid = new QVBoxLayout(this);
	    m_name = new QLineEdit(this, "Name");
	    m_anno = new QMultiLineEdit(this, "Annotation");
	    QPushButton* exitButton = new QPushButton("Okay", this);
	    connect(exitButton, SIGNAL( released() ), this, SLOT( slotOkay() ) );
	    QPushButton* cancelButton = new QPushButton("Cancel", this);
	    connect(cancelButton, SIGNAL( released() ), this, SLOT( slotCancel() ) );
	    QLabel *l = new QLabel("Text",this);
	    grid->addWidget(l);
	    grid->addWidget(m_name);
	    l = new QLabel("Annotation",this);
	    grid->addWidget(l);
	    grid->addWidget(m_anno,1);
	    QHBoxLayout* hgrid = new QHBoxLayout(grid);
	    hgrid->addWidget(cancelButton);
	    hgrid->addWidget(exitButton);
	}
    private slots:
	void slotOkay() { emit finished(m_name->text(), m_anno->text()); }
	void slotCancel() { emit cancelled(); }
 public:
 signals:
    void finished(const QString&, const QString&);
    void cancelled();
};

#endif
