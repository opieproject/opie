#include "qcheckdetails.h"

QCheckDetails::QCheckDetails(int row, int col, const QStringList item)
	: QMainWindow(),
	m_view()
{
	m_view = new QCheckDetailsBase(this);
	setCentralWidget(m_view);

	m_row = row;
	m_col = col;

	QToolBar *bar = new QToolBar(this);
	bar->setHorizontalStretchable( TRUE );

	QPixmap newIcon = Resource::loadPixmap( "edit" );
	QPixmap trashIcon = Resource::loadPixmap( "trash" );
	QToolButton *nb1 = new QToolButton( newIcon, "Edit", QString::null, this, SLOT(editCheck()), bar, "edit transaction" );
	QToolButton *nb2 = new QToolButton( trashIcon, "Delete", QString::null, this, SLOT(deleteCheck()), bar, "delete transaction" );
	addToolBar(bar);

	QString text = "";
	if (item[0] == "true")
	{
		text.append("<b>Payment</b> to <b>");
		text.append(item[1]);
	}
	if (item[0] == "false")
	{
		text.append("<b>Deposit</b> from <b>");
		text.append(item[1]);
	}
	text.append("</b> on <b>");
	text.append(item[7]);
	text.append("</b> for <b>");
	text.append(QString("$" + item[5]));

	text.append("</b>, to make your balance <b>$");
	text.append(item[9]);
	text.append("</b>.");

	text.append("<br><br>");
	text.append("<b>Category: </b>");
	text.append(item[2]);
	text.append("<br>");
	text.append("<b>Type: </b>");

	QString type = "No Type";
	if (item[0] == "true")
	{
		if(item[3] == "0")
		{
			type = "Debit Charge";
		}
		if(item[3] == "1")
		{
			type = "Written Check";
		}
		if(item[3] == "2")
		{
			type = "Transfer";
		}
		if(item[3] == "3")
		{
			type = "Credit Card";
		}
	}

	if (item[0] == "false")
	{
		if(item[3] == "0")
		{
			type = "Written Check";
		}
		if(item[3] == "1")
		{
			type = "Automatic Payment";
		}
		if(item[3] == "2")
		{
			type = "Transfer";
		}
		if(item[3] == "3")
		{
			type = "Cash";
		}
	}

	text.append(type);
	text.append("<br>");
	if (item[4] != "0")
	{
		text.append("<b>Check Number: </b>");
		text.append(item[4]);
		text.append("<br>");
	}
	if (item[6] != ".00")
	{
		text.append("<b>Extra Fee: </b>");
		text.append(QString("$" + item[6]));
		m_view->checkDetails->setText(text);
	}
	if (item[8] != "")
	{
		text.append("<br><b>Additional Comments: </b>");
		text.append(item[8]);
	}
	m_view->checkDetails->setText(text);
}

void QCheckDetails::editCheck()
{
	emit editClicked(m_row, m_col);
}

void QCheckDetails::deleteCheck()
{
	emit deleteClicked(m_row, m_col);
}
