#include "metal.h"


extern "C" {
    QStyle* allocate ( );
    int minor_version ( );
    int major_version ( );
    const char *description ( );
}

QStyle* allocate ( )
{
    return new MetalStyle ( );
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
    return "Metal style plugin";
}
