#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libprotest], [0.0.1], [nu11p0.6477@gmail.com])

AC_CONFIG_MACRO_DIRS([m4])

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CONFIG_SRCDIR([protest.cpp])

dnl # This library depends on C++14 features, so you may want to use
dnl # clang++ and libcpp instead of g++ and libstdc++.
AC_ARG_WITH(
	[stdlibcpp],
	AS_HELP_STRING([--with-stdlibcpp], [Use specified C++ standard library instead of the default]),
	[stdlibcpp=${withval}])
AS_IF([test "x$stdlibcpp" != x], [CXXFLAGS+="-stdlib=${stdlibcpp}"])
AS_BOX([stdlib(c++): ${stdlibcpp}])
AS_BOX([CXXFLAGS: ${CXXFLAGS}])

AC_ARG_ENABLE(
	[debug],
	AS_HELP_STRING([--enable-debug], [Turn on debugging [yes,no](default: no)]),
	AS_CASE([${enableval}],
		[yes], [debug=true],
		[no], [debug=false],
		[
			debug=true
			AC_MSG_WARN([bad value ${enableval} for --enable-debug, using --enable-debug=yes])
		]
	),
	[debug=false])
AM_CONDITIONAL(DEBUG, [test x$debug != xfalse])

# Checks for programs.

dnl # Disable default CFLAGS of AC_PROG_CXX.
dnl # For detail, see
dnl # https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/C_002b_002b-Compiler.html .
: ${CXXFLAGS=""}
AM_COND_IF([DEBUG], [CXXFLAGS="-O0 -g3 ${CXXFLAGS}"], [CXXFLAGS="-O2 ${CXXFLAGS}"])
AC_PROG_CXX

AM_PROG_AR
AC_PROG_LIBTOOL
AX_CXX_COMPILE_STDCXX_14(noext, mandatory)

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN" ; then
	AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

# Checks for libraries.

# Checks for header files.

dnl # use boost::any until compilers support std::experimental::any.
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([boost/any.hpp], [], [AC_MSG_ERROR([boost::any required but not found])])
AC_LANG_POP([C++])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
