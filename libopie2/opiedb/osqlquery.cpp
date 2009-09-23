
#include "osqlquery.h"

using namespace Opie::DB;

OSQLQuery::OSQLQuery()
    : d( 0 )
{
}

OSQLQuery::~OSQLQuery()
{
}

OSQLRawQuery::OSQLRawQuery(const QString& query)
    : OSQLQuery()
    , d( 0 )
    , m_query( query )
{

}
OSQLRawQuery::~OSQLRawQuery() {
}
QString OSQLRawQuery::query()const {
    return m_query;
}
