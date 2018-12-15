AC_INIT([libmateweather], [1.20.2], [http://www.mate-desktop.org])
AC_PREREQ(2.59)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz tar-ustar check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

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

GLIB_REQUIRED=2.50.0
GTK_REQUIRED=3.22.0
LIBSOUP_REQUIRED=2.34.0
GIO_REQUIRED=2.25.0
LIBXML_REQUIRED=2.6.0

LT_PREREQ([2.2.6])
LT_INIT([dlopen disable-static])
LT_LIB_M

IT_PROG_INTLTOOL([0.50.1])

PKG_PROG_PKG_CONFIG([0.19])

AC_PROG_CC

GLIB_GSETTINGS

AC_ARG_ENABLE(all-translations-in-one-xml,
        [AS_HELP_STRING([--enable-all-translations-in-one-xml],
                        [Put all translations in a big Locations.xml file (slow to parse)])],
        [enable_big_xml=yes],
        [enable_big_xml=no])
AM_CONDITIONAL(USE_ONE_BIG_XML, test "x$enable_big_xml" = "xyes")

AC_ARG_ENABLE(locations-compression,
        [AS_HELP_STRING([--enable-locations-compression],
                        [Compress Locations.xml files])],
        [enable_locations_compression=yes],
        [enable_locations_compression=no])
if test "x$enable_locations_compression" = "xyes"; then
    COMPRESS_EXT=.gz
else
    COMPRESS_EXT=
fi
AC_SUBST(COMPRESS_EXT)

MATE_COMPILE_WARNINGS([maximum])

dnl -- Check for GTK+ (required) ------------------------------------------
PKG_CHECK_MODULES(GTK, gtk+-3.0 >= $GTK_REQUIRED)

dnl -- Check for libxml (required) ------------------------------------------
PKG_CHECK_MODULES(LIBXML, libxml-2.0 >= $LIBXML_REQUIRED)

dnl -- check for libsoup (required) -----------------------------------------
PKG_CHECK_MODULES(LIBSOUP, [libsoup-2.4 >= $LIBSOUP_REQUIRED])

dnl -- check for gio (required) -----------------------------------------
PKG_CHECK_MODULES(GIO,
		  [gio-2.0 >= $GIO_REQUIRED])
AC_SUBST(GIO_CFLAGS)

dnl -- check for glib; redundant at this point, but sets $GLIB_MKENUMS
AM_PATH_GLIB_2_0($GLIB_REQUIRED)

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
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <langinfo.h>]], [[
char c;
c = *((unsigned char *)  nl_langinfo(_NL_MEASUREMENT_MEASUREMENT));
]])],[gtk_ok=yes],[gtk_ok=no])
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
dnl *** Honour aclocal flags                                                ***
dnl ***************************************************************************
ACLOCAL_AMFLAGS="\${ACLOCAL_FLAGS}"
AC_SUBST(ACLOCAL_AMFLAGS)

AC_SUBST(VERSION)
AC_SUBST(PACKAGE)
AC_SUBST(DATADIR)
AC_SUBST(LIBDIR)
AC_SUBST(GLADEDIR)

##################################################
# Checks for gtk-doc and docbook-tools
##################################################
GTK_DOC_CHECK([1.11], [--flavour no-tmpl])

dnl ***************************************************************************
dnl *** Output Makefiles et al                                              ***
dnl ***************************************************************************
AC_CONFIG_FILES([
Makefile
doc/Makefile
po/Makefile.in
po-locations/Makefile
libmateweather/Makefile
libmateweather/mateweather.pc
libmateweather/mateweather-uninstalled.pc
libmateweather/org.mate.weather.gschema.xml
data/Makefile
icons/Makefile
])
AC_OUTPUT
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
	Locations.xml translations:	${LOCATIONS_XML_TRANSLATIONS}
	Locations.xml compression:	${enable_locations_compression}
" >&2
