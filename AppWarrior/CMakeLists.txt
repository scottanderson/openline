# AppWarrior: the cross-platform (Mac/Windows) application framework HLClient sits on.
# We only build the Windows-relevant subset here: platform-agnostic sources (no suffix)
# plus "(W)"-suffixed sources; "(M)"-suffixed (Mac) sources are excluded entirely.

# --- zlib 1.1.3, vendored -----------------------------------------------------------
# Source list taken from the vendored nt/Makefile.gcc's $(OBJS) list.
set(ZLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source/Libs/zlib-1.1.3)
set(ZLIB_SOURCES
	${ZLIB_DIR}/adler32.c
	${ZLIB_DIR}/compress.c
	${ZLIB_DIR}/crc32.c
	${ZLIB_DIR}/gzio.c
	${ZLIB_DIR}/uncompr.c
	${ZLIB_DIR}/deflate.c
	${ZLIB_DIR}/trees.c
	${ZLIB_DIR}/zutil.c
	${ZLIB_DIR}/inflate.c
	${ZLIB_DIR}/infblock.c
	${ZLIB_DIR}/inftrees.c
	${ZLIB_DIR}/infcodes.c
	${ZLIB_DIR}/infutil.c
	${ZLIB_DIR}/inffast.c
)
add_library(zlib STATIC ${ZLIB_SOURCES})
target_include_directories(zlib PUBLIC ${ZLIB_DIR})

# --- libjpeg (IJG), vendored under Source/Images/Jpeg -----------------------------
# The full IJG source distribution (encoder + decoder + shared code) is checked in
# here already; CDecompressJpeg.cp only uses the decoder half, but there's no
# cjpeg.c/djpeg.c-style command-line tool source mixed in, so just build everything.
file(GLOB JPEG_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/Images/Jpeg/*.c)
# AppWarrior's jerror.h pulls in AppWarrior/Headers/ImageTypes.h (a local customization
# hooking libjpeg's error strings into AppWarrior's own error-message system), and
# jpegint.h assumes "bool"/size_t are already visible -- i.e. like zlib above, this
# vendored copy is meant to be built as C++, not plain C, despite the .c extension.
set_source_files_properties(${JPEG_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(jpeg STATIC ${JPEG_SOURCES})
target_include_directories(jpeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source/Images/Jpeg)
target_include_directories(jpeg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Headers)
# Same story as appwarrior itself: this vendored/customized libjpeg relies on
# AppWarrior's typedefs (Uint8/Uint32/etc) being visible everywhere via the project's
# old prefix-file mechanism, so force-include the same stand-in header here too.
target_compile_options(jpeg PRIVATE
	-include "${CMAKE_CURRENT_SOURCE_DIR}/Headers/_AWPrefix(W).h"
)
target_compile_definitions(jpeg PRIVATE
	WIN32=1
	_WIN32_WINNT=0x0500
	$<$<BOOL:${HL_NO_QUICKTIME}>:HL_NO_QUICKTIME>
)

# --- appwarrior ------------------------------------------------------------------
file(GLOB_RECURSE AW_ALL_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cp)

# Exclude anything under Source/Libs (zlib handled above; the rest -- QTDataHandler,
# UUID, Internet Config, etc -- are either standalone components we don't need or
# have no usable Windows source).
list(FILTER AW_ALL_SOURCES EXCLUDE REGEX "/Source/Libs/")

# Exclude Mac-only "(M).cp" sources.
list(FILTER AW_ALL_SOURCES EXCLUDE REGEX "\\(M\\)\\.cp$")

# QuickTime: pick exactly one of the real vs. stub CQuickTimeView implementation.
if(HL_NO_QUICKTIME)
	list(FILTER AW_ALL_SOURCES EXCLUDE REGEX "/CQuickTimeView\\.cp$")
else()
	list(FILTER AW_ALL_SOURCES EXCLUDE REGEX "/CQuickTimeView\\(NoQT\\)\\.cp$")
endif()

# Explicitly mark these as C++ -- ".cp" isn't a default-recognized CXX extension.
set_source_files_properties(${AW_ALL_SOURCES} PROPERTIES LANGUAGE CXX)

# ANSI.cp is a hand-rolled ANSI C runtime (malloc/free, memcpy/memmove/memset,
# strlen/strcpy/strcmp/strtol/etc.) written for the original classic-Mac CodeWarrior
# toolchain, which lacked a full ANSI libc. Its functions are deliberately named to
# match the standard library (e.g. `size_t strlen(const char *str)`).
#
# Without -fno-builtin, GCC recognizes these names as the builtins it already knows the
# semantics of, and can rewrite a hand-written definition into a self-referential call
# to "the builtin" -- which, since the symbol being defined *is* that builtin, becomes
# infinite tail recursion. At -O3 this collapsed strlen()'s body into a 2-byte `jmp $`
# (opcodes eb fe) at its own entry point: an unconditional jump to itself, i.e. a
# process-wide 100%-CPU hang. Confirmed via disassembly of the compiled ANSI.cp.obj
# (see the "_strlen" symbol) and reproduced/fixed in isolation by recompiling this one
# file with/without -fno-builtin.
#
# This is what caused hlclient.exe (and, since they share this same statically-linked
# appwarrior library, hlserver.exe and both hltracker*.exe) to hang at ~99% CPU
# immediately on launch, before WinMain ever runs: strlen() is called very early during
# CRT/static-init, on the first path that happened to route through it, and simply never
# returns.
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/ANSI.cp PROPERTIES
	COMPILE_OPTIONS "-fno-builtin"
)

add_library(appwarrior STATIC ${AW_ALL_SOURCES})

target_include_directories(appwarrior PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Headers
	${CMAKE_CURRENT_SOURCE_DIR}/Resources
)

# Private/colocated headers (e.g. Data/UMemory(priv).h) live next to their .cp files.
target_include_directories(appwarrior PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Data
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Files
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Graphics
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Hardware
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Images
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Images/Pict
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc
	${CMAKE_CURRENT_SOURCE_DIR}/Source/User\ Interface
	${CMAKE_CURRENT_SOURCE_DIR}/Source/Views
)

target_compile_definitions(appwarrior PUBLIC
	WIN32=1
	_WIN32_WINNT=0x0500
	$<$<BOOL:${HL_NO_QUICKTIME}>:HL_NO_QUICKTIME>
)

# Force-include the prefix header that stands in for the CodeWarrior "prefix file" /
# precompiled headers (AppWarrior/Libraries/*.pch) the original project relied on. See
# the comment at the top of that file for why this is needed project-wide.
target_compile_options(appwarrior PUBLIC
	-include "${CMAKE_CURRENT_SOURCE_DIR}/Headers/_AWPrefix(W).h"
)

target_link_libraries(appwarrior PUBLIC zlib jpeg)
