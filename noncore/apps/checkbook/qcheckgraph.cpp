#include "qcheckgraph.h"

#include <qpainter.h>
#include <qmessagebox.h>
#include <qfontmetrics.h>
#include <qdir.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qmenubar.h>
#include <qpopupmenu.h>

QCheckGraph::QCheckGraph(const QString filename)
	: QCheckGraphBase()
{
	QMenuBar *bar = new QMenuBar(this);
	bar->setMargin(0);
	QPopupMenu *popup = new QPopupMenu;
	popup->insertItem("&Save Graph...", this, SLOT(saveGraphAsPicture()));
	bar->insertItem("&File", popup);

	pixmapready = false;
	config = new Config(filename, Config::File);
	QString forresult = "";
	QString forresult2 = "";
	int i = 1;
	for (; forresult != "Not Found"; i++)
	{
		config->setGroup(QString::number(i));
		forresult = config->readEntry("Description", QString("Not Found"));
		forresult2 = config->readEntry("Category", QString("Not Found"));
		if (forresult != "Not Found")
		{
			if (list.contains(forresult2) == 0)
			{
				config->setGroup("Totals");
				QString larger = config->readEntry("Spent", QString("no"));
				QString smaller = config->readEntry(forresult2, QString("no"));
				if (larger != "no" && smaller != "no")
				{
					list << forresult2;
					QString percentage = calculator(smaller, larger, true);

					// Here we calculate how many pixels tall it will be by multiplying it by 200.
					QString pixels = calculator(percentage, QString("200"), false);

					// This is done because it really doesn't need to have a decimal... just an int, and
					// QString rounds doubles up to the nearest whole number in order to convert
					// to an int (which is correct).
					pixels = pixels.remove(pixels.find('.'), pixels.length());

					if (pixels.toInt() <= 5)
					{
						pixels = "6";
					}

					list2 << pixels;
				}
			}
		} else {
			continue;
		}
		// We want to break this loop on the 7th (remember, starts at 0->6 == 7) item.
		if (list.count() == 6)
		{
			break;
		}
	}
	for (QStringList::Iterator it = list.begin(); it != list.end(); it++)
	{
		if ((*it).length() > 11)
		{
			(*it).truncate(8);
			(*it).append("...");
		}
	}
	graphPixmap();
}

void QCheckGraph::graphPixmap()
{
	pixmapready = false;
	graph = QPixmap(240,250);
	QPainter p;
	p.begin(&graph);
	p.fillRect(0, 0, 240, 300, QColor(255,255,255));
	// Draw the graph lines

	// Y
	p.drawLine( 40, 50, 40, 252 );

	// X
	p.drawLine( 40, 252, 203, 252 );

	// Y stepup lines
	p.drawLine( 40, 50, 37, 50);
	p.drawLine( 40, 70, 37, 70);
	p.drawLine( 40, 90, 37, 90);
	p.drawLine( 40, 110, 37, 110);
	p.drawLine( 40, 130, 37, 130);
	p.drawLine( 40, 150, 37, 150);
	p.drawLine( 40, 170, 37, 170);
	p.drawLine( 40, 190, 37, 190);
	p.drawLine( 40, 210, 37, 210);
	p.drawLine( 40, 230, 37, 230);
	p.drawLine( 40, 245, 37, 245);


	// Y stepup values
	p.drawText((35 - p.fontMetrics().width("100")), (50 + (p.fontMetrics().height() / 2)), "100");
	p.drawText((35 - p.fontMetrics().width("90")), (70 + (p.fontMetrics().height() / 2)), "90");
	p.drawText((35 - p.fontMetrics().width("80")), (90 + (p.fontMetrics().height() / 2)), "80");
	p.drawText((35 - p.fontMetrics().width("70")), (110 + (p.fontMetrics().height() / 2)), "70");
	p.drawText((35 - p.fontMetrics().width("60")), (130 + (p.fontMetrics().height() / 2)), "60");
	p.drawText((35 - p.fontMetrics().width("50")), (150 + (p.fontMetrics().height() / 2)), "50");
	p.drawText((35 - p.fontMetrics().width("40")), (170 + (p.fontMetrics().height() / 2)), "40");
	p.drawText((35 - p.fontMetrics().width("30")), (190 + (p.fontMetrics().height() / 2)), "30");
	p.drawText((35 - p.fontMetrics().width("20")), (210 + (p.fontMetrics().height() / 2)), "20");
	p.drawText((35 - p.fontMetrics().width("10")), (230 + (p.fontMetrics().height() / 2)), "10");
	p.drawText((35 - p.fontMetrics().width("<10")), (245 + (p.fontMetrics().height() / 2)), "<10");

	// Draw the axis[sic?] labels
	QString ylabel = "Percentage";
	int pixel = 100;
	for (unsigned int i = 0; i != ylabel.length(); i++)
	{
		p.setBrush(QColor(0,0,0));
		p.drawText(5,pixel, QString(ylabel[i]));
		pixel = pixel + p.fontMetrics().height();
	}
	p.drawText(95, 265, "Category");

	int i = 0;
	// Hack: Using if()'s... switch was acting all wierd :{
	QStringList::Iterator it2 = list2.begin();
	for (QStringList::Iterator it = list.begin(); it != list.end(); it++)
	{
		qWarning(QString::number(i));
		if (i ==0)
		{
			// For the color code:
			p.setBrush(QColor(255,0,0));
			p.drawRect(8,12, 8, 8);

			// Now the text:
			p.setBrush(QColor(0,0,0));
			p.drawText(18,20, (*it));

			// Last, but not least, we have the actual bar graph height.
			p.setBrush(QColor(255,0,0));
			p.drawRect(47, ((202 - (*it2).toInt()) + 50), 15, (*it2).toInt());
		}
		if (i ==1)
		{
			p.setBrush(QColor(255,255,0));
			p.drawRect(78,12, 8, 8);

			p.setBrush(QColor(0,0,0));
			p.drawText(88,20, (*it));

			p.setBrush(QColor(255,255,0));
			p.drawRect(70, ((202 - (*it2).toInt()) + 50), 15, (*it2).toInt());
		}
		if (i==2)
		{
			p.setBrush(QColor(0,255,0));
			p.drawRect(153,12, 8, 8);
			p.setBrush(QColor(0,0,0));
			p.drawText(163,20, (*it));

			p.setBrush(QColor(0,255,0));
			p.drawRect(98, ((202 - (*it2).toInt()) + 50), 15, (*it2).toInt());
		}
		if (i==3)
		{
			p.setBrush(QColor(89,12,54));
			p.drawRect(8,27, 8, 8);
			p.setBrush(QColor(0,0,0));
			p.drawText(18,35, (*it));

			p.setBrush(QColor(89,12,54));
			p.drawRect(126, ((202 - (*it2).toInt()) + 50), 15, (*it2).toInt());
		}
		if (i==4)
		{
			p.setBrush(QColor(0,0,255));
			p.drawRect(78,27, 8, 8);
			p.setBrush(QColor(0,0,0));
			p.drawText(88,35, (*it));
			p.setBrush(QColor(0,0,255));
			p.drawRect(154, ((202 - (*it2).toInt()) + 50), 15, (*it2).toInt());
		}
		if (i==5)
		{
			p.setBrush(QColor(100,40,0));
			p.drawRect(153,27, 8, 8);
			p.setBrush(QColor(0,0,0));
			p.drawText(163,35, (*it));
			p.setBrush(QColor(100,40,0));
			p.drawRect(182, ((202 - (*it2).toInt()) + 50), 15, (*it2).toInt());
		}
		i++;
		it2++;
	}

	p.end();
	pixmapready = true;
	graphWidget->setBackgroundPixmap(graph);
}

QString QCheckGraph::calculator(QString largervalue, QString smallervalue, bool divide)
{
//	largervalue = largervalue.remove(largervalue.find(".", 0, false), 1);
//	smallervalue = smallervalue.remove(smallervalue.find(".", 0, false), 1);

	double largercents = largervalue.toDouble();
	double smallercents = smallervalue.toDouble();

	double finalammount = 0;

	if (divide == true)
	{
		finalammount = (largercents / smallercents);
	} else {
		finalammount = (largercents * smallercents);
	}

	qWarning(QString::number(finalammount));

	return QString::number(finalammount);
}

/*void QCheckGraph::paintEvent(QPaintEvent *e)
{
	if (pixmapready == true)
	{
		bitBlt((QPaintDevice *)(graphWidget), 0, 0, graph, 0,0);
		QWidget::paintEvent(e);
	}
}
*/
void QCheckGraph::saveGraphAsPicture()
{
	QString homedir = QDir::homeDirPath();
	QDate current = QDate::currentDate();
	QString datestring = QString::number(current.month());
	datestring.append(QString::number(current.day()));
	datestring.append(QString::number(current.year()));
	QString filename = homedir;
	filename.append("/Documents/graph");
	filename.append(datestring);
	filename.append(".png");
	graph.save(filename, "PNG", 85);
	QString graphmessage = "<qt>The graph was saved as graph";
	graphmessage.append(datestring);
	graphmessage.append(".png. You may access it by returning to your home screen and clicking on the \"Documents\" tab in the upper right hand screen</qt>");
	QMessageBox::information(0, "File Saved", graphmessage);
}
