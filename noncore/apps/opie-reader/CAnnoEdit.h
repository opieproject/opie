#ifndef __CANNOEDIT_H
#define __CANNOEDIT_H
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qcombobox.h>

class CAnnoEdit : public QWidget
{
    Q_OBJECT

    QLineEdit* m_name;
    QMultiLineEdit* m_anno;
    size_t m_posn, m_posn2;
    QComboBox* colorbox;
 public:
    void setPosn(size_t p) { m_posn = p; }
    void setPosn2(size_t p) { m_posn2 = p; }
    size_t getPosn() { return m_posn; }
    size_t getPosn2() { return m_posn2; }
    QColor getColor();
    void setColor(QColor);
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
    CAnnoEdit(QWidget *parent=0, const char *name=0, WFlags f = 0);
    private slots:
	void slotOkay()
      {
	emit finished(m_name->text(), m_anno->text());
      }
	void slotCancel() { emit cancelled(); }
 public:
 signals:
    void finished(const QString&, const QString&);
    void cancelled();
};

#endif
