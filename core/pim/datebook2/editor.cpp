#include "mainwindow.h"

#include "editor.h"

using namespace Datebook;

Editor::Editor( MainWindow* win, QWidget*)
    : m_win( win )
{}
Editor::~Editor() {

}
DescriptionManager Editor::descriptions()const {
    return m_win->descriptionManager();
}
LocationManager Editor::locations()const {
    return m_win->locationManager();
}
void Editor::setDescriptions( const DescriptionManager& dsc) {
    m_win->setDescriptionManager( dsc );
}
void Editor::setLocations( const LocationManager& loc) {
    m_win->setLocationManager( loc );
}
