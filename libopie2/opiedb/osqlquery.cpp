
#include "osqlquery.h"

using namespace Opie::DB;

OSQLQuery::OSQLQuery() {
}
OSQLQuery::~OSQLQuery() {
}

OSQLRawQuery::OSQLRawQuery(const QString& query)
    : OSQLQuery(), m_query( query ) {

}
OSQLRawQuery::~OSQLRawQuery() {
}
QString OSQLRawQuery::query()const {
    return m_query;
}
