# Copyright (C) 2010 Richard Hughes <richard@hughsie.com>
AC_PREREQ(2.63)

m4_define([libddc_major_version], [0])
m4_define([libddc_minor_version], [0])
m4_define([libddc_micro_version], [1])
m4_define([libddc_version],
          [libddc_major_version.libddc_minor_version.libddc_micro_version])

AC_INIT([libddc],[libddc_version],[richard@hughsie.com],[libddc])
AC_CONFIG_SRCDIR(libddc-glib)
AM_INIT_AUTOMAKE([1.9 tar-ustar dist-bzip2])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

GOBJECT_INTROSPECTION_CHECK([0.6.7])

# enable nice build output on automake1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

LIBDDC_MAJOR_VERSION=libddc_major_version
LIBDDC_MINOR_VERSION=libddc_minor_version
LIBDDC_MICRO_VERSION=libddc_micro_version
LIBDDC_VERSION=libddc_version
AC_SUBST(LIBDDC_MAJOR_VERSION)
AC_SUBST(LIBDDC_MINOR_VERSION)
AC_SUBST(LIBDDC_MICRO_VERSION)
AC_SUBST(LIBDDC_VERSION)

# libtool versioning - this applies to liblibddc
#
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details
#
# increment;
# CURRENT	If the API or ABI interface has changed (reset REVISION to 0)
# REVISION	If the API and ABI remains the same, but bugs are fixed.
# AGE		If liblibddc can be linked into executables which can be
# 		built with previous versions of this library. Don't use.
LT_CURRENT=1
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_PROG_CC
AC_PROG_INSTALL
AC_SEARCH_LIBS([strerror],[cposix])
AC_HEADER_STDC
LT_INIT
AM_PROG_CC_C_O

# set up gtk-doc
GTK_DOC_CHECK(1.9)

dnl ---------------------------------------------------------------------------
dnl - Extra verbose warning switches
dnl ---------------------------------------------------------------------------
CPPFLAGS="$CPPFLAGS -DG_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES"
CPPFLAGS="$CPPFLAGS -DG_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED"
CPPFLAGS="$CPPFLAGS -DGDK_MULTIHEAD_SAFE -DGTK_MULTIHEAD_SAFE"
CPPFLAGS="$CPPFLAGS -DGSEAL_ENABLE"

dnl ---------------------------------------------------------------------------
dnl - Use strict options (default enabled for devs, disabled in releases)
dnl ---------------------------------------------------------------------------
dnl if .git directory is present, considering we are working in the repository
if test -d ".git"; then
	default_strict=yes
else
	default_strict=no
fi

AC_ARG_ENABLE(strict, AS_HELP_STRING([--enable-strict],[Enable strict compilation options]), enable_strict=$enableval,
							enable_strict=$default_strict)
if test x$enable_strict != xno; then
	if test "$GCC" = "yes"; then
		WARNINGFLAGS_C="$WARNINGFLAGS_C -Werror"
	fi
fi

if test "$GCC" = "yes"; then
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wall"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wcast-align -Wno-uninitialized"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wmissing-declarations"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wredundant-decls"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wpointer-arith"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wcast-align"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wwrite-strings"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Winit-self"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wreturn-type"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wformat-nonliteral"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wformat-security"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wmissing-include-dirs"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wmissing-format-attribute"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wclobbered"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wempty-body"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wignored-qualifiers"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wsign-compare"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wtype-limits"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wuninitialized"
	WARNINGFLAGS_C="$WARNINGFLAGS_C"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Waggregate-return"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wdeclaration-after-statement"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wshadow"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wno-strict-aliasing"
	WARNINGFLAGS_C="$WARNINGFLAGS_C -Winline"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Wmissing-parameter-type"
#	WARNINGFLAGS_C="$WARNINGFLAGS_C -Woverride-init"
else
	WARNINGFLAGS_C=""
	WARNINGFLAGS_C=""
fi
AC_SUBST(WARNINGFLAGS_C)

dnl ---------------------------------------------------------------------------
dnl - Library dependencies
dnl ---------------------------------------------------------------------------
GLIB_REQUIRED=2.22.0

dnl ---------------------------------------------------------------------------
dnl - Check library dependencies
dnl ---------------------------------------------------------------------------
PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0)

dnl ---------------------------------------------------------------------------
dnl - Generate man pages ? (default enabled)
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE(man_pages, AS_HELP_STRING([--disable-man-pages],[Disable man pages generation]), enable_man_pages=$enableval)
if test x$enable_man_pages != xno; then
	AC_PATH_PROG(XSLTPROC, xsltproc, no)
	if test x$enable_man_pages = xyes; then
		if test "$XSLTPROC" = "no" ; then
			AC_MSG_ERROR([xsltproc not found, it's needod to build man pages])
		fi
	fi
else
	XSLTPROC=no
fi
AM_CONDITIONAL(HAVE_XSLTPROC, [test "$XSLTPROC" != "no"])

dnl ---------------------------------------------------------------------------
dnl - Make paths available for source files
dnl ---------------------------------------------------------------------------
AC_SUBST(LIBDIR, $libdir)
AC_SUBST(DATADIR, $datadir)
AC_SUBST(BINDIR, $bindir)
AC_SUBST(SBINDIR, $sbindir)

dnl ---------------------------------------------------------------------------
dnl - Build self tests
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE(tests, AS_HELP_STRING([--enable-tests],[enable unit test code]),
	      enable_tests=$enableval,enable_tests=yes)
AM_CONDITIONAL(EGG_BUILD_TESTS, test x$enable_tests = xyes)
if test x$enable_tests = xyes; then
	AC_DEFINE(EGG_BUILD_TESTS,1,[Build test code])
fi

dnl ---------------------------------------------------------------------------
dnl - Makefiles, etc.
dnl ---------------------------------------------------------------------------
AC_CONFIG_FILES([
Makefile
docs/Makefile
docs/api/Makefile
docs/api/version.xml
libddc-glib/Makefile
libddc-glib/libddc-version.h
libddc-glib/libddc-glib.pc
tools/Makefile
])
AC_OUTPUT

dnl ==========================================================================
echo "
                    libddc $VERSION
                  ====================

        prefix:                    ${prefix}
        datadir:                   ${datadir}
        compiler:                  ${CC}
        cflags:                    ${CFLAGS}
        cppflags:                  ${CPPFLAGS}
        Building unit tests:       ${enable_tests}
"
