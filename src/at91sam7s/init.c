#include <gyros/at91sam7s/aic.h>

#include "../private.h"

void
gyros__target_init(void)
{
    aic_init();
}
