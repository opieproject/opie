#include <qwidgetstack.h>
#include <qlabel.h>
#include <qaction.h>

#include <qpe/ir.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>


#include "mainwindow.h"


using namespace Datebook;

MainWindow::MainWindow()
    : OPimMainWindow( "Datebook", 0, 0 ) {
    initUI();
    initConfig();
    initView();
    initManagers();

    raiseCurrentView();
    QTimer::singleShot(0, this, SLOT(populate() ) );
}
MainWindow::~MainWindow() {

}
void MainWindow::doSetDocument( const QString& str ) {

}
void MainWindow::flush() {

}
void MainWindow::reload() {

}
int MainWindow::create() {

}
bool MainWindow::remove( int uid ) {

}
void MainWindow::beam( int uid ) {

}
void MainWindow::show( int uid ) {

}
void MainWindow::add( const OPimRecord& ) {

}
void MainWindow::edit( int uid ) {

}
void MainWindow::initUI() {

}
void MainWindow::initConfig() {

}
void MainWindow::initView() {

}
void MainWindow::initManagers() {

}
void MainWindow::raiseCurrentView() {

}
