#include "osqlerror.h"

OSQLError::OSQLError( const QString& driverText,
                      const QString& driverDatabaseText,
                      int type, int subType )
    : m_drvText( driverText ), m_drvDBText( driverDatabaseText ),
      m_type( type ), m_number( subType )
{
}
OSQLError::~OSQLError() {
}
QString OSQLError::driverText()const {
    return m_drvText;
}
QString OSQLError::databaseText()const {
    return m_drvDBText;
}
int OSQLError::type()const {
    return m_type;
}
int OSQLError::subNumber()const {
    return m_number;
}
