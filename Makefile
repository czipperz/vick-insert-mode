CFLAGS=-std=c++11 -I../../src -I../../test -Isrc
LDFLAGS=`find ../../out -type f -not \( -name main.o -o -name configuration.o \)` \
        -lncurses ../../testout/test_main.o `find ../vick-move/out -type f`
O=out
S=src
T=test
TO=testout
CXX=clang++

files=$O/insert_mode.o

testfiles=

all: ${files}

begin:
	git pull
	[ -d ../vick-move ] || git clone "https://github.com/czipperz/vick-move" ../vick-move
	cd ../vick-move && make begin && make

$O/%.o: $S/%.cc $S/%.hh
	@mkdir -p $O
	${CXX} -o $@ -c $< ${CFLAGS}

$O/%.o: $S/%.cc
	@mkdir -p $O
	${CXX} -o $@ -c $< ${CFLAGS}

${TO}/%.o: $T/%.cc
	@mkdir -p ${TO}
	${CXX} -o $@ -c $< ${CFLAGS}

clean:
	[ ! -d $O ] || rm -R $O
	[ -z "`find -name '*~'`" ] || rm `find -name '*~'`
	[ ! -d ${TO} ] || rm -R ${TO}

test: ${files} ${testfiles}
	@mkdir -p $T
	${CXX} -o $T/out ${files} ${testfiles} ${CFLAGS} ${LDFLAGS} ../../src/configuration.cc -Dtesting
	./$T/out

.PHONY: all begin clean test
