/**********************************************************************
** BenchmarkInfo
**
** A benchmark for Qt/Embedded
**
** Copyright (C) 2004 Michael Lauer <mickey@vanille.de>
** Inspired by ZBench (C) 2002 Satoshi <af230533@im07.alpha-net.ne.jp>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include <qwidget.h>
#include <qdialog.h>

class QClipboard;
class QCheckListItem;
class QPushButton;
class QListView;

class BenchmarkInfo : public QWidget
{
    Q_OBJECT

public:
    BenchmarkInfo( QWidget *parent = 0, const char *name = 0, int wFlags = 0 );
    ~BenchmarkInfo();

    QCheckListItem* test_alu;
    QCheckListItem* test_fpu;
    QCheckListItem* test_txt;
    QCheckListItem* test_gfx;
    QCheckListItem* test_ram;
    QCheckListItem* test_sd;
    QCheckListItem* test_cf;

    bool main_rd;
    bool main_wt;
    bool sd_rd;
    bool sd_wt;
    bool cf_rd;
    bool cf_wt;

    QClipboard* clb;

    QListView* tests;
    QPushButton* startButton;

    int textRendering( int );
    int gfxRendering( int );
    bool writeFile( const QString& );
    bool readFile( const QString& );

private slots:
    void run();

};
