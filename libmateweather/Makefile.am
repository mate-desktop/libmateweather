lib_LTLIBRARIES = libmateweather.la

libmateweatherincdir = $(includedir)/libmateweather
mateweather_old_headers = \
	weather.h mateweather-prefs.h mateweather-xml.h
mateweather_new_headers = \
	mateweather-location.h location-entry.h \
	mateweather-timezone.h timezone-menu.h
libmateweatherinc_HEADERS = \
	$(mateweather_old_headers)		\
	$(mateweather_new_headers)		\
	mateweather-enum-types.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mateweather.pc

libmateweather_la_SOURCES = \
	weather.c weather.h weather-priv.h \
	weather-metar.c weather-iwin.c weather-met.c \
	weather-bom.c weather-wx.c \
	weather-sun.c weather-moon.c \
	mateweather-enum-types.c \
	mateweather-prefs.c mateweather-prefs.h \
	mateweather-xml.c mateweather-xml.h \
	mateweather-location.c mateweather-location.h \
	mateweather-timezone.c mateweather-timezone.h \
	mateweather-win32.h \
	location-entry.c location-entry.h \
	timezone-menu.c timezone-menu.h \
	parser.c parser.h

if OS_WIN32
libmateweather_la_SOURCES += mateweather-win32.c
else
EXTRA_libmateweather_la_SOURCES = mateweather-win32.c
endif

libmateweather_la_CFLAGS = \
	-I$(top_srcdir)			\
	-I$(srcdir)			\
	$(WARN_CFLAGS)			\
	$(GTK_CFLAGS)			\
	$(LIBXML_CFLAGS)		\
	$(LIBSOUP_CFLAGS)		\
	$(GIO_CFLAGS)			\
	-DG_LOG_DOMAIN=\"MateWeather\"	\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	-DMATEWEATHER_XML_LOCATION_DIR=\""$(pkgdatadir)"\"

libmateweather_la_LIBADD = \
	-lm		\
	$(GTK_LIBS)	\
	$(LIBXML_LIBS)	\
	$(LIBSOUP_LIBS)	\
	$(GIO_LIBS) \
	$(REGEX_LIBS)

libmateweather_la_LDFLAGS = \
	-version-info $(LT_VERSION) -no-undefined

# something is wrong with glib-mkenums, please research the issue
MKENUMS_GENERATED = mateweather-enum-types.c mateweather-enum-types.h

mateweather-enum-types.h: $(mateweather_new_headers)
#	$(AM_V_GEN)( cd $(srcdir) && $(GLIB_MKENUMS) --template mateweather-enum-types.h.tmpl \
#		$(mateweather_new_headers) ) > mateweather-enum-types.h.tmp \
#	&& mv mateweather-enum-types.h.tmp mateweather-enum-types.h \
#	|| rm -f mateweather-enum-type.h.tmp

mateweather-enum-types.c: $(libmateweatherinclude_HEADERS)
#	$(AM_V_GEN)( cd $(srcdir) && $(GLIB_MKENUMS) --template mateweather-enum-types.c.tmpl \
#		$(mateweather_new_headers) ) > mateweather-enum-types.c.tmp \
#	&& mv mateweather-enum-types.c.tmp mateweather-enum-types.c \
#	|| rm -f mateweather-enum-type.c.tmp

BUILT_SOURCES = $(MKENUMS_GENERATED)

test_metar_SOURCES = test_metar.c

test_metar_CFLAGS = \
	-I$(top_srcdir)			\
	-I$(srcdir)			\
	$(WARN_CFLAGS)			\
	$(GTK_CFLAGS)			\
	$(LIBSOUP_CFLAGS)		\
	-DG_LOG_DOMAIN=\"MateWeather\"

test_metar_LDADD = \
	$(GTK_LIBS) \
	$(LIBSOUP_LIBS)	\
	$(REGEX_LIBS) \
	libmateweather.la

test_locations_SOURCES = test_locations.c

test_locations_CFLAGS = \
	-I$(top_srcdir)			\
	-I$(srcdir)			\
	$(WARN_CFLAGS)			\
	$(GTK_CFLAGS)			\
	$(MATE_VFS_APPLETS_CFLAGS)	\
	-DG_LOG_DOMAIN=\"MateWeather\"

test_locations_LDADD = libmateweather.la $(GTK_LIBS)

test_sun_moon_SOURCES = \
	test_sun_moon.c

test_sun_moon_CFLAGS = \
	-I$(top_srcdir)			\
	-I$(srcdir)			\
	$(WARN_CFLAGS)			\
	$(GTK_CFLAGS)			\
	$(LIBSOUP_CFLAGS)		\
	-DG_LOG_DOMAIN=\"MateWeather\"

test_sun_moon_LDADD = \
	$(GTK_LIBS)	\
	$(LIBSOUP_LIBS)	\
	-lm	\
	libmateweather.la

noinst_HEADERS = weather-priv.h mateweather-win32.h
noinst_PROGRAMS = test_metar test_locations test_sun_moon

@INTLTOOL_XML_NOMERGE_RULE@
gsettings_SCHEMAS = org.mate.weather.gschema.xml
@GSETTINGS_RULES@

%.gschema.xml.in: %.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

EXTRA_DIST = mateweather.pc.in mateweather-uninstalled.pc.in $(gsettings_SCHEMAS).in.in

EXTRA_PROGRAMS = test_metar test_sun_moon

CLEANFILES = $(EXTRA_PROGRAMS) $(gsettings_SCHEMAS).in $(gsettings_SCHEMAS) *.gschema.valid
#$(MKENUMS_GENERATED)

-include $(top_srcdir)/git.mk
