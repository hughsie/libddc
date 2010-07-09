## We require new-style dependency handling.
AUTOMAKE_OPTIONS = 1.7

NULL =

EXTRA_DIST =						\
	libddc-util.xml

if HAVE_XSLTPROC
man_MANS =						\
	libddc-util.1
endif

if HAVE_XSLTPROC
libddc-util.1: libddc-util.xml
	xsltproc http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $? &> /dev/null
endif

INCLUDES =						\
	$(GLIB_CFLAGS)					\
	$(DBUS_CFLAGS)					\
	$(SQLITE_CFLAGS)				\
	-DBINDIR=\"$(bindir)\"			 	\
	-DDATADIR=\"$(datadir)\"			\
	-DPREFIX=\""$(prefix)"\" 			\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 			\
	-DVERSION="\"$(VERSION)\"" 			\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"		\
	-DLIBDDC_DATA=\"$(libddcgdatadir)\"			\
	-DI_KNOW_THE_LIBDDC_GLIB_API_IS_SUBJECT_TO_CHANGE	\
	-I$(top_srcdir)/libddc-glib			\
	$(NULL)

bin_PROGRAMS =						\
	libddc-util

LIBDDC_GLIB_LIBS =					\
	$(top_builddir)/libddc-glib/libddc-glib.la

libddc_util_SOURCES =					\
	libddc-util.c					\
	$(NULL)

libddc_util_LDADD =					\
	$(GLIB_LIBS)					\
	$(LIBDDC_GLIB_LIBS)

libddc_util_CFLAGS =					\
	$(WARNINGFLAGS_C)				\
	$(NULL)

clean-local:
	rm -f *~
	rm -f *.1
	rm -f manpage.*
	rm -f *.out

CLEANFILES = $(BUILT_SOURCES)

DISTCLEANFILES =					\
	$(NULL)

MAINTAINERCLEANFILES =					\
	*~			      			\
	Makefile.in					\
	$(NULL)

