COMMON_CFLAGS =			\
	-ggdb			\
	-Wno-long-long -Wall    \
	$(THIS_CFLAGS)

if !PLATFORM_WIN32
    COMMON_CFLAGS += -rdynamic
endif

AM_CFLAGS   += -std=gnu99 $(COMMON_CFLAGS)
# -fno-default-inline is useful for debugging. It allows
# for getting meaningful backtraces for faults in default-inline functions
# defined in header files.
AM_CXXFLAGS += $(COMMON_CFLAGS) # -fno-default-inline

INCLUDES = -I$(top_srcdir) -I$(top_builddir)

mylang_target_headers =		\
	util.h			\
				\
	file_position.h		\
	token.h			\
				\
	token_stream.h		\
	file_token_stream.h	\
				\
	byte_stream.h		\
	file_byte_stream.h	\
				\
	unichar_stream.h	\
	utf8_unichar_stream.h

mylang_linux_target_headesr =

mylang_win32_target_headers =

mylang_private_headers =

if PLATFORM_WIN32
mylang_target_headers  += $(mylang_win32_target_headers)
mylang_private_headers += $(mylang_linux_target_headers)
else
mylang_target_headers  += $(mylang_linux_target_headers)
mylang_private_headers += $(mylang_win32_target_headers)
endif

mylangincludedir = $(includedir)/mylang-1.0/mylang
mylanginclude_HEADERS = $(mylang_target_headers)

lib_LTLIBRARIES = libmylang-1.0.la

MYLANG_GENFILES =
BUILT_SOURCES = $(MYLANG_GENFILES)
CLEANFILES = $(MYLANG_GENFILES)
nodist_libmylang_1_0_la_SOURCES = $(MYLANG_GENFILES)

if PLATFORM_WIN32
else
endif

libmylang_1_0_la_SOURCES =		\
	util.cpp			\
					\
	token_stream.cpp		\
	file_token_stream.cpp		\
					\
	file_byte_stream.cpp		\
					\
	unichar_stream.cpp		\
	utf8_unichar_stream.cpp

mylang_linux_sources =

mylang_win32_sources =

mylang_extra_dist =

if PLATFORM_WIN32
    libmylang_1_0_la_SOURCES += $(mylang_win32_sources)
    mylang_extra_dist += $(mylang_linux_sources)
else
    libmylang_1_0_la_SOURCES += $(mylang_linux_sources)
    mylang_extra_dist += $(mylang_win32_sources)
endif

libmylang_1_0_la_LDFLAGS = -no-undefined -version-info "0:0:0"
libmylang_1_0_la_LIBADD = $(THIS_LIBS)
if PLATFORM_WIN32
libmylang_1_0_la_LIBADD +=
endif

EXTRA_DIST = $(mylang_private_headers) $(mylang_extra_dist)

