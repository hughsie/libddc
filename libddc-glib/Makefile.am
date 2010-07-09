NULL =

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

INCLUDES = \
	$(GLIB_CFLAGS)						\
	-I$(top_srcdir)/libddc-glib				\
	-I$(top_builddir)/libddc-glib				\
	-I.							\
	-DLIBDDC_COMPILATION					\
	-DLOCALSTATEDIR=\""$(localstatedir)"\" 			\
	-DTESTDATADIR=\""$(top_srcdir)/data/tests"\"		\
	-DI_KNOW_THE_LIBDDC_GLIB_API_IS_SUBJECT_TO_CHANGE	\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"			\
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

libddcgconfigdir = $(libdir)/libddcgconfig
libddcgconfig_DATA = 						\
	libddc-glib.pc

lib_LTLIBRARIES =						\
	libddc-glib.la

libddc_glib_includedir = $(includedir)/libddc/libddc-glib

libddc_glib_include_HEADERS =					\
	libddc.h						\
	libddc-client.h						\
	libddc-device.h						\
	libddc-control.h					\
	libddc-version.h					\
	libddc-common.h						\
	$(NULL)

libddc_glib_la_SOURCES =					\
	libddc.h						\
	libddc-client.c						\
	libddc-client.h						\
	libddc-device.c						\
	libddc-device.h						\
	libddc-control.c					\
	libddc-control.h					\
	libddc-version.h					\
	libddc-common.h						\
	$(NULL)

libddc_glib_la_LIBADD =						\
	$(GLIB_LIBS)

libddc_glib_la_LDFLAGS =					\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	-export-dynamic						\
	-no-undefined						\
	-export-symbols-regex '^libddc_.*'			\
	$(NULL)

libddc_glib_la_CFLAGS =						\
	$(WARNINGFLAGS_C)					\
	$(NULL)

if EGG_BUILD_TESTS
check_PROGRAMS =						\
	libddc-self-test

libddc_self_test_SOURCES =					\
	libddc-self-test.c

libddc_self_test_LDADD =					\
	libddc-glib.la						\
	$(GLIB_LIBS)

libddc_self_test_CFLAGS = -DEGG_TEST $(AM_CFLAGS) $(WARNINGFLAGS_C)

TESTS = libddc-self-test
endif

EXTRA_DIST =							\
	libddc-glib.pc.in					\
	libddc-version.h.in

CLEANFILES = $(BUILT_SOURCES)

if HAVE_INTROSPECTION
introspection_sources = $(libddc_glib_la_SOURCES)

libddcGlib-1.0.gir: libddc-glib.la
libddcGlib_1_0_gir_INCLUDES = GObject-2.0
libddcGlib_1_0_gir_CFLAGS = $(INCLUDES) -DI_KNOW_THE_LIBDDC_GLIB_API_IS_SUBJECT_TO_CHANGE
libddcGlib_1_0_gir_LIBS = libddc-glib.la
libddcGlib_1_0_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
INTROSPECTION_GIRS += libddcGlib-1.0.gir

girdir = $(datadir)/gir-1.0
dist_gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(dist_gir_DATA) $(typelib_DATA)
endif

clean-local:
	rm -f *~
	rm -f $(CLEANFILES)

