#ifndef __QPDF_H__
#define __QPDF_H__

#include "aconf.h"

#include <qmainwindow.h>


class QPEOutputDev;
class PDFDoc;

class DocLnk;
class FileSelector;
class QWidgetStack;
class QLineEdit;
class QToolButton;

class QPdfDlg : public QMainWindow {
	Q_OBJECT

public:
	QPdfDlg ( );
	virtual ~QPdfDlg ( );
	
public slots:
	void firstPage ( );
	void prevPage ( );
	void nextPage ( );
	void lastPage ( );
	
	void gotoPage ( int n );
	
	void setZoom ( int z );
	
	void gotoPageDialog ( );

	void toggleFullscreen ( );
	void toggleFindBar ( );	

	void findText ( const QString & );	
	void findText ( );
	
	void openFile ( );
	void openFile ( const QString & );
	void openFile ( const DocLnk & );	
	
	void setDocument ( const QString & );
	
private slots:
	void delayedInit ( );
	void closeFileSelector ( );

	void updateCaption ( );

	void copyToClipboard ( const QRect & );

protected:
	void setFullscreen ( bool b = true );

	void setBusy ( bool b = true );
	bool busy ( ) const;

	void renderPage ( );
	
	virtual void resizeEvent ( QResizeEvent *e );
	virtual void focusInEvent ( QFocusEvent *e );
	
private:
	QWidgetStack *m_stack;
	QPEOutputDev *m_outdev;	
	FileSelector *m_filesel;
	
	QToolBar *m_tb_menu, *m_tb_tool, *m_tb_find;
	QLineEdit *m_findedit;
	QPopupMenu *m_pm_zoom;

	QToolButton *m_to_find, *m_to_full;
	
	bool m_fullscreen;

	bool m_busy;
	bool m_renderok;
	
	int m_currentpage;
	int m_pages;
	int m_zoom;
	
	PDFDoc *m_doc;
	
	QString m_currentdoc;
};


#endif
