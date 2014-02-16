SUBDIRS = po po-locations libmateweather doc data python icons

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --disable-icon-update

EXTRA_DIST = \
	autogen.sh

MAINTAINERCLEANFILES = \
	$(srcdir)/INSTALL \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/compile \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in \
	$(srcdir)/config.sub \
	$(srcdir)/depcomp \
	$(srcdir)/install-sh \
	$(srcdir)/ltmain.sh \
	$(srcdir)/missing \
	$(srcdir)/mkinstalldirs \
	`find "$(srcdir)" -type f -name Makefile.in -print` \
	$(srcdir)/configure \
	$(srcdir)/gtk-doc.make \
	$(srcdir)/m4/gtk-doc.m4 \
	$(srcdir)/m4/intltool.m4

-include $(top_srcdir)/git.mk
