/* -*- C -*- ***********************************************
Copyright (c) 2000, BeOpen.com.
Copyright (c) 1995-2000, Corporation for National Research Initiatives.
Copyright (c) 1990-1995, Stichting Mathematisch Centrum.
All rights reserved.

See the file "Misc/COPYRIGHT" for information on usage and
redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
******************************************************************/

/* Module configuration */

/* !!! !!! !!! This file is edited by the makesetup script !!! !!! !!! */

/* This file contains the table of built-in modules.
   See create_builtin() in import.c. */

#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif


/* -- ADDMODULE MARKER 1 -- */
extern PyObject* PyInit__abc(void);
extern PyObject* PyInit__codecs(void);
extern PyObject* PyInit__collections(void);
extern PyObject* PyInit__functools(void);
extern PyObject* PyInit__io(void);
extern PyObject* PyInit__locale(void);
extern PyObject* PyInit__operator(void);
extern PyObject* PyInit__signal(void);
extern PyObject* PyInit__sre(void);
extern PyObject* PyInit__stat(void);
extern PyObject* PyInit__string(void);
extern PyObject* PyInit__thread(void);
extern PyObject* PyInit__tracemalloc(void);
extern PyObject* PyInit__weakref(void);
extern PyObject* PyInit_atexit(void);
extern PyObject* PyInit_errno(void);
extern PyObject* PyInit_itertools(void);
extern PyObject* PyInit_posix(void);
extern PyObject* PyInit_pwd(void);
extern PyObject* PyInit_time(void);
extern PyObject* PyInit_zipimport(void);

extern PyObject* PyMarshal_Init(void);
extern PyObject* PyInit__imp(void);
extern PyObject* PyInit_gc(void);
extern PyObject* PyInit__ast(void);
extern PyObject* _PyWarnings_Init(void);
extern PyObject* PyInit__string(void);

struct _inittab _PyImport_Inittab[] = {

/* -- ADDMODULE MARKER 2 -- */
	{"_abc", PyInit__abc},
	{"_codecs", PyInit__codecs},
	{"_collections", PyInit__collections},
	{"_functools", PyInit__functools},
	{"_io", PyInit__io},
	{"_locale", PyInit__locale},
	{"_operator", PyInit__operator},
	{"_signal", PyInit__signal},
	{"_sre", PyInit__sre},
	{"_stat", PyInit__stat},
	{"_string", PyInit__string},
	{"_thread", PyInit__thread},
	{"_tracemalloc", PyInit__tracemalloc},
	{"_weakref", PyInit__weakref},
	{"atexit", PyInit_atexit},
	{"errno", PyInit_errno},
	{"itertools", PyInit_itertools},
	{"posix", PyInit_posix},
	{"pwd", PyInit_pwd},
	{"time", PyInit_time},
	{"zipimport", PyInit_zipimport},

    /* This module lives in marshal.c */
    {"marshal", PyMarshal_Init},

    /* This lives in import.c */
    {"_imp", PyInit__imp},

    /* This lives in Python/Python-ast.c */
    {"_ast", PyInit__ast},

    /* These entries are here for sys.builtin_module_names */
    {"builtins", NULL},
    {"sys", NULL},

    /* This lives in gcmodule.c */
    {"gc", PyInit_gc},

    /* This lives in _warnings.c */
    {"_warnings", _PyWarnings_Init},

    /* This lives in Objects/unicodeobject.c */
    {"_string", PyInit__string},

    /* Sentinel */
    {0, 0}
};


#ifdef __cplusplus
}
#endif
