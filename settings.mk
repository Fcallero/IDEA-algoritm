# Libraries
LIBS=global commons readline

# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=

# Compiler flags
CDEBUG=-g -Wall -DDEBUG -fcommon -fdiagnostics-color=always
CRELEASE=-O3 -Wall -DNDEBUG -fcommon

# Arguments when executing with start, memcheck or helgrind
ARGS=bin/contenido.txt 1234ACB980154D89F1B123F011208965

# Valgrind flags
MEMCHECK_FLAGS=--track-origins=yes
HELGRIND_FLAGS=

# Source files (*.c) to be excluded from tests compilation
TEST_EXCLUDE=src/main.c
