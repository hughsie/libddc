AUTOMAKE_OPTIONS = 1.7

ACLOCAL_AMFLAGS = -I m4

NULL =

SUBDIRS = 						\
	libddc-glib					\
	tools						\
	docs						\
	$(NULL)

clean-local :
	rm -f *~

security-check:
	flawfinder `find -name "*.c"`

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)-`date +"%Y%m%d"`

DISTCLEANFILES =					\
	libddc-*.tar.gz					\
	$(NULL)

MAINTAINERCLEANFILES =					\
	*~						\
	compile						\
	configure					\
	INSTALL						\
	install-sh					\
	missing						\
	mkinstalldirs					\
	config.guess					\
	ltmain.sh					\
	config.sub					\
	depcomp						\
	Makefile.in					\
	config.h.*					\
	aclocal.m4					\
	$(NULL)

EXTRA_DIST =						\
	COPYING						\
	MAINTAINERS					\
	AUTHORS						\
	INSTALL						\
	README						\
	NEWS						\
        autogen.sh					\
	config.h					\
	$(NULL)

DISTCHECK_CONFIGURE_FLAGS = 				\
	--enable-gtk-doc				\
	--enable-introspection				\
	--enable-tests					\
	$(NULL)

distclean-local:
	if test $(srdcir) = .; then :; else \
		rm -f ChangeLog; \
	fi

ChangeLog:
	@echo Creating $@
	@if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git ./missing --run git log LIBDDC_0_5_X.. --stat -M -C --name-status  --date=short --no-color) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi

.PHONY: ChangeLog
