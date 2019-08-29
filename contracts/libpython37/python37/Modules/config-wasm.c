/* Generated automatically from ./Modules/config.c.in by makesetup. */
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


extern PyObject* PyInit_array(void);
extern PyObject* PyInit_math(void);
extern PyObject* PyInit_cmath(void);
extern PyObject* PyInit__contextvars(void);
extern PyObject* PyInit__struct(void);
extern PyObject* PyInit__random(void);
extern PyObject* PyInit__bisect(void);
extern PyObject* PyInit__datetime(void);
extern PyObject* PyInit__heapq(void);
extern PyObject* PyInit__json(void);
extern PyObject* PyInit__csv(void);
extern PyObject* PyInit_unicodedata(void);
extern PyObject* PyInit__pickle(void);
extern PyObject* PyInit_parser(void);
extern PyObject* PyInit__socket(void);
extern PyObject* PyInit_select(void);
extern PyObject* PyInit__posixsubprocess(void);
extern PyObject* PyInit_binascii(void);
extern PyObject* PyInit_zlib(void);
extern PyObject* PyInit_pyexpat(void);
extern PyObject* PyInit__sha1(void);
extern PyObject* PyInit__sha256(void);
extern PyObject* PyInit__sha512(void);
extern PyObject* PyInit__sha3(void);
extern PyObject* PyInit__md5(void);
extern PyObject* PyInit__blake2(void);
extern PyObject* PyInit__queue(void);
extern PyObject* PyInit__multibytecodec(void);
extern PyObject* PyInit__codecs_cn(void);
extern PyObject* PyInit__codecs_hk(void);
extern PyObject* PyInit__codecs_iso2022(void);
extern PyObject* PyInit__codecs_jp(void);
extern PyObject* PyInit__codecs_kr(void);
extern PyObject* PyInit__codecs_tw(void);
extern PyObject* PyInit__lsprof(void);
extern PyObject* PyInit__decimal(void);
extern PyObject* PyInit_posix(void);
extern PyObject* PyInit_errno(void);
extern PyObject* PyInit_pwd(void);
extern PyObject* PyInit__sre(void);
extern PyObject* PyInit__codecs(void);
extern PyObject* PyInit__weakref(void);
extern PyObject* PyInit__functools(void);
extern PyObject* PyInit__operator(void);
extern PyObject* PyInit__collections(void);
extern PyObject* PyInit__abc(void);
extern PyObject* PyInit_itertools(void);
extern PyObject* PyInit_atexit(void);
extern PyObject* PyInit__signal(void);
extern PyObject* PyInit__stat(void);
extern PyObject* PyInit_time(void);
extern PyObject* PyInit__thread(void);
extern PyObject* PyInit__locale(void);
extern PyObject* PyInit__io(void);
extern PyObject* PyInit_zipimport(void);
extern PyObject* PyInit_faulthandler(void);
extern PyObject* PyInit__tracemalloc(void);
extern PyObject* PyInit__symtable(void);
extern PyObject* PyInit_xxsubtype(void);
/* -- ADDMODULE MARKER 1 -- */

extern PyObject* PyMarshal_Init(void);
extern PyObject* PyInit__imp(void);
extern PyObject* PyInit_gc(void);
extern PyObject* PyInit__ast(void);
extern PyObject* _PyWarnings_Init(void);
extern PyObject* PyInit__string(void);
extern PyObject* PyInit__db(void);
extern PyObject* PyInit__eosiolib(void);

struct _inittab _PyImport_Inittab[] = {

{"array", PyInit_array},
{"math", PyInit_math},
{"cmath", PyInit_cmath},
//{"_contextvars", PyInit__contextvars},
{"_struct", PyInit__struct},
//{"_random", PyInit__random},
{"_bisect", PyInit__bisect},
{"_datetime", PyInit__datetime},
//{"_heapq", PyInit__heapq},
{"_json", PyInit__json},
#if 0
{"_csv", PyInit__csv},
{"unicodedata", PyInit_unicodedata},
#endif
{"_pickle", PyInit__pickle},
#if 0
{"parser", PyInit_parser},
{"_socket", PyInit__socket},
{"select", PyInit_select},
{"_posixsubprocess", PyInit__posixsubprocess},
#endif
{"binascii", PyInit_binascii},
#if 0
{"zlib", PyInit_zlib},
{"pyexpat", PyInit_pyexpat},
#endif
{"_sha1", PyInit__sha1},
{"_sha256", PyInit__sha256},
{"_sha512", PyInit__sha512},
{"_sha3", PyInit__sha3},
{"_md5", PyInit__md5},
{"_blake2", PyInit__blake2},
#if 0
{"_queue", PyInit__queue},
{"_multibytecodec", PyInit__multibytecodec},
{"_codecs_cn", PyInit__codecs_cn},
{"_codecs_hk", PyInit__codecs_hk},
{"_codecs_iso2022", PyInit__codecs_iso2022},
{"_codecs_jp", PyInit__codecs_jp},
{"_codecs_kr", PyInit__codecs_kr},
{"_codecs_tw", PyInit__codecs_tw},
#endif
#if 0
{"_lsprof", PyInit__lsprof},
{"_decimal", PyInit__decimal},
#endif
//{"posix", PyInit_posix},
#if 0
{"errno", PyInit_errno},
{"pwd", PyInit_pwd},
#endif
{"_sre", PyInit__sre},
{"_codecs", PyInit__codecs},
{"_weakref", PyInit__weakref},
#if 0
{"_functools", PyInit__functools},
{"_operator", PyInit__operator},
{"_collections", PyInit__collections},
#endif
{"_abc", PyInit__abc},
#if 0
{"itertools", PyInit_itertools},
{"atexit", PyInit_atexit},
{"_signal", PyInit__signal},
{"_stat", PyInit__stat},
#endif
{"time", PyInit_time},
//{"_thread", PyInit__thread},
#if 0
{"_locale", PyInit__locale},
#endif
{"_io", PyInit__io},
#if 0
{"zipimport", PyInit_zipimport},
{"faulthandler", PyInit_faulthandler},
{"_tracemalloc", PyInit__tracemalloc},
{"_symtable", PyInit__symtable},
{"xxsubtype", PyInit_xxsubtype},
#endif
/* -- ADDMODULE MARKER 2 -- */

    /* This module lives in marshal.c */
    {"marshal", PyMarshal_Init},

    /* This lives in import.c */
    {"_imp", PyInit__imp},

    /* This lives in Python/Python-ast.c */
//    {"_ast", PyInit__ast},

    /* These entries are here for sys.builtin_module_names */
    {"builtins", NULL},
    {"sys", NULL},

    /* This lives in gcmodule.c */
//    {"gc", PyInit_gc},

    /* This lives in _warnings.c */
    {"_warnings", _PyWarnings_Init},

    /* This lives in Objects/unicodeobject.c */
    {"_string", PyInit__string},
    {"_db", PyInit__db},
//    {"eosiolib", PyInit__eosiolib},
    /* Sentinel */
    {0, 0}
};


#ifdef __cplusplus
}
#endif
