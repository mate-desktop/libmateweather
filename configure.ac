AC_INIT([libmateweather], [1.8.0], [http://www.mate-desktop.org])
AC_PREREQ(2.59)

AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

# Before making a release, the LT_VERSION string should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A

LT_VERSION=7:9:6
AC_SUBST(LT_VERSION)

AC_CANONICAL_HOST
AC_MSG_CHECKING([for Win32])
case "$host" in
  *-*-mingw*)
    mateweather_native_win32="yes"
    ;;
  *)
    mateweather_native_win32="no"
    ;;
esac
AC_MSG_RESULT([$mateweather_native_win32])
AM_CONDITIONAL(OS_WIN32, [test "$mateweather_native_win32" = "yes"])

AC_MSG_CHECKING([which gtk+ version to compile against])
AC_ARG_WITH([gtk],
  [AS_HELP_STRING([--with-gtk=2.0|3.0],[which gtk+ version to compile against (default: 2.0)])],
  [case "$with_gtk" in
     2.0|3.0) ;;
     *) AC_MSG_ERROR([invalid gtk version specified]) ;;
   esac],
  [with_gtk=2.0])
AC_MSG_RESULT([$with_gtk])

case "$with_gtk" in
  2.0) GTK_API_VERSION=2.0
       GTK_REQUIRED=2.11.0
       ;;
  3.0) GTK_API_VERSION=3.0
       GTK_REQUIRED=3.0.0
       ;;
esac
AC_SUBST(GTK_API_VERSION)

GLIB_REQUIRED=2.13.0
LIBSOUP_REQUIRED=2.34.0
GIO_REQUIRED=2.25.0
LIBXML_REQUIRED=2.6.0

AM_MAINTAINER_MODE
MATE_MAINTAINER_MODE_DEFINES

IT_PROG_INTLTOOL([0.40.3])
PKG_PROG_PKG_CONFIG([0.19])

AC_PROG_CC
AM_PROG_CC_C_O
AC_ISC_POSIX
AC_STDC_HEADERS
AM_PROG_LIBTOOL

GLIB_GSETTINGS

AC_ARG_ENABLE(all-translations-in-one-xml,
        [AC_HELP_STRING([--enable-all-translations-in-one-xml],
                        [Put all translations in a big Locations.xml file (slow to parse)])],
        [enable_big_xml=yes],
        [enable_big_xml=no])
AM_CONDITIONAL(USE_ONE_BIG_XML, test "x$enable_big_xml" = "xyes")

AC_ARG_ENABLE(locations-compression,
        [AC_HELP_STRING([--enable-locations-compression],
                        [Compress Locations.xml files])],
        [enable_locations_compression=yes],
        [enable_locations_compression=no])
if test "x$enable_locations_compression" = "xyes"; then
    COMPRESS_EXT=.gz
else
    COMPRESS_EXT=
fi
AC_SUBST(COMPRESS_EXT)

MATE_COMPILE_WARNINGS

dnl ***************************************************************************
dnl *** --enable-more-warnings turns on more compiler warnings              ***
dnl ***************************************************************************
AC_ARG_ENABLE(more-warnings,
[  --enable-more-warnings  Maximum compiler warnings],
set_more_warnings="$enableval",[
    set_more_warnings=no
])
AC_MSG_CHECKING(for more warnings, including -Werror)
if test "$GCC" = "yes" -a "$set_more_warnings" != "no"; then
    AC_MSG_RESULT(yes)
    CFLAGS="\
    -Wall \
    -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes \
    -Wnested-externs -Wpointer-arith \
    -Wcast-align -Wsign-compare \
    -std=gnu89 \
    $CFLAGS"

    for option in -Wno-strict-aliasing -Wno-sign-compare; do
        SAVE_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $option"
        AC_MSG_CHECKING([whether gcc understands $option])
        AC_TRY_COMPILE([], [],
            has_option=yes,
            has_option=no,)
        if test $has_option = no; then
            CFLAGS="$SAVE_CFLAGS"
        fi
        AC_MSG_RESULT($has_option)
        unset has_option
        unset SAVE_CFLAGS
    done
    unset option
else
    AC_MSG_RESULT(no)
fi

if test "x$enable_maintainer_mode" = "xyes"; then
	CFLAGS="$CFLAGS $DISABLE_DEPRECATED"
fi

dnl -- Check for GTK+ (required) ------------------------------------------
PKG_CHECK_MODULES(GTK, gtk+-$GTK_API_VERSION >= $GTK_REQUIRED)
GTK_CFLAGS="$GTK_CFLAGS -DGTK_DISABLE_SINGLE_INCLUDES"
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

dnl -- Check for libxml (required) ------------------------------------------
PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= $LIBXML_REQUIRED)
AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)

dnl -- check for libsoup (required) -----------------------------------------
PKG_CHECK_MODULES(LIBSOUP, [libsoup-2.4 >= $LIBSOUP_REQUIRED])

dnl -- check for gio (required) -----------------------------------------
PKG_CHECK_MODULES(GIO,
		  [gio-2.0 >= GIO_REQUIRED])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

dnl -- check for glib; redundant at this point, but sets $GLIB_MKENUMS
AM_PATH_GLIB_2_0($GLIB_REQUIRED)
CFLAGS="$CFLAGS -DG_DISABLE_SINGLE_INCLUDES"

dnl gettext

GETTEXT_PACKAGE=libmateweather
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext Package])
AM_GLIB_GNU_GETTEXT

dnl icon cache
AC_ARG_ENABLE(icon-update, AC_HELP_STRING([--disable-icon-update],
[Disable icon cache update]))
if (test "$enable_icon_update" != no); then
AC_PATH_PROG(UPDATE_ICON_CACHE, [gtk-update-icon-cache])
fi
AM_CONDITIONAL([ICON_UPDATE], [test -n "$UPDATE_ICON_CACHE"])

dnl ***************************************************************************
dnl ***  REGEX                                                              ***
dnl ***************************************************************************
AC_CHECK_FUNCS(regexec,,[AC_CHECK_LIB(regex,regexec,
               [REGEX_LIBS=-lregex],
               [AC_MSG_ERROR([No regex library found])])])
AC_SUBST(REGEX_LIBS)


dnl ***************************************************************************
dnl *** Check for presence of tm.tm_gmtoff on the system                    ***
dnl ***************************************************************************
AC_CHECK_MEMBER([struct tm.tm_gmtoff],[struct_tm_tm_gmtoff=true],[struct_tm_tm_gmtoff=false],[#include <time.h>])
case $struct_tm_tm_gmtoff in
        true)
                AC_DEFINE_UNQUOTED(HAVE_TM_TM_GMOFF, 1, Have the tm.tm_gmtoff member.)
                ;;
        *)
                AC_CHECK_DECL(timezone,AC_DEFINE(HAVE_TIMEZONE,1,Have timezone),,
[
#include <time.h>
])
                ;;
esac

dnl ***************************************************************************
dnl *** _NL_MEASUREMENT_MEASUREMENT is an enum and not a define             ***
dnl ***************************************************************************
AC_MSG_CHECKING([for _NL_MEASUREMENT_MEASUREMENT])
AC_TRY_LINK([#include <langinfo.h>], [
char c;
c = *((unsigned char *)  nl_langinfo(_NL_MEASUREMENT_MEASUREMENT));
], gtk_ok=yes, gtk_ok=no)
AC_MSG_RESULT($gtk_ok)
if test "$gtk_ok" = "yes"; then
  AC_DEFINE([HAVE__NL_MEASUREMENT_MEASUREMENT], [1],
            [Define if _NL_MEASUREMENT_MEASUREMENT is available])
fi

dnl ***************************************************************************
dnl *** Find tzdata                                                         ***
dnl ***************************************************************************
AC_ARG_WITH(zoneinfo-dir,
	    [  --with-zoneinfo-dir     tzdata zoneinfo directory],
	    ZONEINFO_DIR="$withval")
if test -z "$ZONEINFO_DIR"; then
    if test -d /usr/share/zoneinfo; then
	ZONEINFO_DIR="/usr/share/zoneinfo"
    elif test -d /usr/share/lib/zoneinfo/tab; then
	ZONEINFO_DIR="/usr/share/lib/zoneinfo/tab"
    else
	AC_MSG_ERROR(Can't find zoneinfo directory. Use --with-zoneinfo-dir)
    fi
fi
AC_DEFINE_UNQUOTED(ZONEINFO_DIR, "$ZONEINFO_DIR", [zoneinfo directory])

dnl ***************************************************************************
dnl *** Python bindings
dnl ***************************************************************************
AC_ARG_ENABLE(python,
        [AC_HELP_STRING([--enable-python],
                        [Build libmateweather python bindings])],
        [enable_python=$enableval],
        [enable_python=no])
if test "$enable_python" = "yes"; then
   AM_PATH_PYTHON()
   AM_CHECK_PYTHON_HEADERS(,enable_python=no)
   PKG_CHECK_MODULES(PYGOBJECT, pygobject-2.0)
   pygtk_defsdir=`$PKG_CONFIG --variable=defsdir pygtk-2.0`
   PYGTK_DEFS="$pygtk_defsdir/gtk.defs"
   AC_SUBST(PYGTK_DEFS)
fi
AM_CONDITIONAL(BUILD_PYTHON, test "x$enable_python" = "xyes")

dnl ***************************************************************************
dnl *** Honour aclocal flags                                                ***
dnl ***************************************************************************
ACLOCAL_AMFLAGS="\${ACLOCAL_FLAGS}"
AC_SUBST(ACLOCAL_AMFLAGS)

AC_SUBST(VERSION)
AC_SUBST(PACKAGE)
AC_SUBST(DATADIR)
AC_SUBST(LIBDIR)
AC_SUBST(PYTHONDIR)
AC_SUBST(GLADEDIR)

##################################################
# Checks for gtk-doc and docbook-tools
##################################################
GTK_DOC_CHECK([1.9])

dnl ***************************************************************************
dnl *** Output Makefiles et al                                              ***
dnl ***************************************************************************
AC_OUTPUT([
Makefile
doc/Makefile
po/Makefile.in
po-locations/Makefile
libmateweather/Makefile
libmateweather/mateweather.pc
libmateweather/mateweather-uninstalled.pc
data/Makefile
python/Makefile
icons/Makefile
])

if test "x$enable_big_xml" = "xyes"; then
	LOCATIONS_XML_TRANSLATIONS="one big file"
else
	LOCATIONS_XML_TRANSLATIONS="one file per translation"
fi

dnl ***************************************************************************
dnl *** Display Summary                                                     ***
dnl ***************************************************************************
echo "
libmateweather-$VERSION configure summary:

	Prefix:				${prefix}
	Source code location:		${srcdir}
	Compiler:			${CC}
	Python bindings:		${enable_python}
	Locations.xml translations:	${LOCATIONS_XML_TRANSLATIONS}
	Locations.xml compression:	${enable_locations_compression}
	Gtk+ version:			${GTK_API_VERSION}
" >&2
