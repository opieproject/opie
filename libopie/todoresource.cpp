
#include "todoresource.h"

using namespace Opie;


ToDoResource::ToDoResource(const QString& appName)
    : m_appName( appName )
{
}
ToDoResource::~ToDoResource() {
}
bool ToDoResource::wasChangedExternally()const {
    return false;
}
QString ToDoResource::appName()const {
    return m_appName;
}
