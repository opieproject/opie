#include "liquid.h"


extern "C" {
    QStyle* allocate ( );
    int minor_version ( );
    int major_version ( );
    const char *description ( );
}

QStyle* allocate ( )
{
    return new LiquidStyle ( );
}

int minor_version ( )
{
    return 0;
}

int major_version ( )
{
    return 1;
}

const char *description ( )
{
    return "High Performance Liquid";
}
