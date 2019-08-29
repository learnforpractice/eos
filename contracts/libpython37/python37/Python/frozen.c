
/* Dummy frozen modules initializer */
#include "Python.h"

static const struct _frozen _PyImport_FrozenModules[] = {
    {0, 0, 0} /* sentinel */
};

/* Embedding apps may change this pointer to point to their favorite
   collection of frozen modules: */

//const struct _frozen *PyImport_FrozenModules = _PyImport_FrozenModules;
