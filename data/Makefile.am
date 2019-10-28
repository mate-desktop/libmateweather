libmateweatherdtddir = $(pkgdatadir)
libmateweatherdtd_DATA = locations.dtd

libmateweatherlocationsdir = $(pkgdatadir)
libmateweatherlocations_in_files = Locations.xml.in

if USE_ONE_BIG_XML

LOCATIONS_STAMP = 

libmateweatherlocations_DATA = $(libmateweatherlocations_in_files:.xml.in=.xml$(COMPRESS_EXT))

%.xml$(COMPRESS_EXT): %.xml.in $(wildcard $(top_srcdir)/po-locations/*.po)
	$(AM_V_GEN) GETTEXTDATADIR=$(top_srcdir) $(MSGFMT) --xml --template $< -d $(top_srcdir)/po-locations -o `echo $@ | sed "s/.xml$(COMPRESS_EXT)/.xml/"` && \
	if test "x$(COMPRESS_EXT)" = "x.gz"; then				\
		gzip -n --force `echo $@ | sed "s/.xml$(COMPRESS_EXT)/.xml/"`;	\
	fi

else # USE_ONE_BIG_XML

LOCATIONS_STAMP = stamp-Locations.xml

PO_LOCATIONS = $(shell if test -n "$(LINGUAS)"; then for lang in $(LINGUAS); do if test -f "$(top_srcdir)/po-locations/$$lang.po"; then echo "$(top_srcdir)/po-locations/$$lang.po "; fi; done; else for pofile in $(top_srcdir)/po-locations/*.po; do echo $$pofile; done; fi)

# Helper variable
libmateweatherlocations_data = $(libmateweatherlocations_in_files:.xml.in=.xml)

libmateweatherlocations_DATA = $(shell echo $(PO_LOCATIONS) | sed "s|$(top_srcdir)/po-locations/|Locations.|g;s|\.po|.xml$(COMPRESS_EXT)|g") $(libmateweatherlocations_data)$(COMPRESS_EXT)

# We need this step so that we merge all the make Locations.xy.xml destinations
# into one unique destination. This makes -j2 work. (Else, we end up with
# multiple and conflicting calls to intltool-merge)
$(libmateweatherlocations_DATA): $(LOCATIONS_STAMP)

$(LOCATIONS_STAMP): $(libmateweatherlocations_in_files) $(PO_LOCATIONS) Makefile
	$(AM_V_GEN) for pofile in $(PO_LOCATIONS); do \
		locale=`echo $$pofile | sed "s;$(top_srcdir)/po-locations/\(.*\)\.po;\1;"`;	\
		GETTEXTDATADIR=$(top_srcdir) $(MSGFMT) -l $$locale --xml --template $< $(top_srcdir)/po-locations/$$locale.po -o $(libmateweatherlocations_data) && \
		xmllint --noblanks -o Locations.$$locale.xml $(libmateweatherlocations_data); \
		rm -f $(libmateweatherlocations_data);			\
		if test "x$(COMPRESS_EXT)" = "x.gz"; then		\
			gzip -n --force Locations.$$locale.xml;		\
		fi;							\
	done &&								\
	xmllint --noblanks --dtdvalid $(top_srcdir)/data/$(libmateweatherdtd_DATA) -o Locations.xml $(top_srcdir)/data/$(libmateweatherlocations_in_files) && \
	if test "x$(COMPRESS_EXT)" = "x.gz"; then			\
		gzip -n --force Locations.xml;				\
	fi &&								\
	touch $@

endif # USE_ONE_BIG_XML

### Locations.xml.in rebuild
rebuild-locations: locationdb.sqlite update-locations.py
	$(AM_V_GEN)($(srcdir)/update-locations.py > Locations.xml.in.new && mv Locations.xml.in.new Locations.xml.in) || rm -f Locations.xml.in.new

locationdb.sqlite: build-locationdb.pl major-cities.txt sources/nsd_cccc.txt sources/POP_PLACES.txt sources/US_CONCISE.txt sources/geonames_dd_dms_date_*.txt city-fixups.pl station-fixups.pl
	$(AM_V_GEN)$(srcdir)/build-locationdb.pl

check:
	xmllint --valid --noout $(top_srcdir)/data/Locations.xml.in
	$(srcdir)/check-timezones.sh $(srcdir)/Locations.xml.in

EXTRA_DIST =	\
	$(libmateweatherlocations_in_files)	\
	$(libmateweatherdtd_DATA)			\
	check-timezones.sh			\
	README					\
	README.timezones

CLEANFILES =	\
	$(libmateweatherlocations_DATA)		\
	$(LOCATIONS_STAMP)

-include $(top_srcdir)/git.mk
