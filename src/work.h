#include "config.h"


#ifdef ROLE_SLAVE
    #include "slave.h"
#elif defined ROLE_MASTER
    #include "master.h"
#endif
