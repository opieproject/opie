#include "kglobalsettings.h"

QFont KGlobalSettings::generalFont()
{
  return QFont("fixed",12);
}

QColor KGlobalSettings::baseColor()
{
	return Qt::white;
}

QColor KGlobalSettings::highlightColor()
{
	return Qt::blue;
}

