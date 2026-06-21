AC_INIT([meshrenderer], [2023], [Cannibalized from NiHo´s code ;)])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_AUX_DIR([auxx])

AM_INIT_AUTOMAKE(dist-bzip2 no-dist-gzip)

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX(11)

AC_CHECK_LIB([GL], [main])
AC_CHECK_LIB([GLEW], [main])
AC_CHECK_LIB([glfw], [main])

AC_DEFINE_UNQUOTED(OPENGL_INCLUDE_PATH, ["$(pkg-config --variable=includedir gl)"], [foo])
AC_DEFINE_UNQUOTED(GLEW_INCLUDE_PATH, ["$(pkg-config --variable=includedir glew)"], [foo])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
