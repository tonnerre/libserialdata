#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libserialdata], [0.2], [caoimhechaos@protonmail.com])
AC_CONFIG_SRCDIR([serialdatareader.cc])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
LT_INIT

AC_ARG_WITH([qt-includes], [AC_HELP_STRING([--with-qt-includes=DIR],
	    [Path to the QT headers])], [INCLUDES="$INCLUDES -I${withval}"])
AC_ARG_WITH([qt-libs], [AC_HELP_STRING([--with-qt-libs=DIR],
	    [Path to the QT libraries])],
	    LDFLAGS="${LDFLAGS} -L${withval} -L${withval}"])
AC_SUBST(INCLUDES)
AC_SUBST(LDFLAGS)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_LANG(C++)

LIBRARY_VERSION=`echo $VERSION | $SED -e's/\./:/g'`
AC_SUBST(LIBRARY_VERSION)

OLDCXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=c++11"
AC_MSG_CHECKING([whether $CXX accepts -std=c++11])
AC_LINK_IFELSE([AC_LANG_CALL([], [main])], [AC_MSG_RESULT(yes)],
	[AC_MSG_RESULT(no); CXXFLAGS="$OLDCXXFLAGS"])
unset OLDCXXFLAGS
CXXFLAGS="${CXXFLAGS}"
AC_SUBST(CXXFLAGS)

# Checks for libraries.
GTEST_LIBS=""
QT_LIBS=""
AC_CHECK_LIB([z], [crc32],
	     [AC_LIBS="$AC_LIBS -lz"])
AC_CHECK_LIB([pthread], [pthread_once],
	     [AC_LIBS="$AC_LIBS -lpthread"])
ORIGLIBS="${LIBS}"
LIBS="${LIBS} ${AC_LIBS}"
AC_CHECK_LIB([protobuf], [main],
	     [AC_LIBS="$AC_LIBS -lprotobuf"])
LIBS="${ORIGLIBS}"
AC_CHECK_LIB([gtest], [main],
	     [GTEST_LIBS="$GTEST_LIBS -lgtest"])
AC_CHECK_LIB([gtest_main], [main],
	     [GTEST_LIBS="$GTEST_LIBS -lgtest_main"])
AC_CHECK_LIB([QtCore], [main],
	     [QT_LIBS="$QT_LIBS -lQtCore"])
AC_CHECK_LIB([QtNetwork], [main],
	     [QT_LIBS="$QT_LIBS -lQtNetwork"])
AC_LIBS="$AC_LIBS $QT_LIBS"
AC_SUBST(GTEST_LIBS)
AC_SUBST(QT_LIBS)
AC_SUBST(AC_LIBS)

# Checks for header files.
old_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $INCLUDES"
AC_CHECK_HEADERS([QtCore/QByteArray QtCore/QString QtCore/QFile])
CPPFLAGS="$old_CPPFLAGS"

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
