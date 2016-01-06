all: dcstsolver

scip:
	wget http://scip.zib.de/download/release/scipoptsuite-3.2.0.tgz
	tar -xf scipoptsuite-3.2.0.tgz
	cd scipoptsuite-3.2.0
	make

src/dcstsolver.o: src/dcstsolver.cpp
	g++ -Iscipoptsuite-3.2.0/scip-3.2.0/src -Wall -g -c src/dcstsolver.cpp -o src/dcstsolver.o

src/main.o: src/main.cpp
	g++ -Iscipoptsuite-3.2.0/scip-3.2.0/src -Wall -g -c src/main.cpp -o src/main.o

dcstsolver: src/dcstsolver.o src/dcstsolver.hpp src/scip_exception.hpp src/main.o
	g++ \
	  -Wall \
	  -g \
	  src/main.o src/dcstsolver.o \
	  -Lscipoptsuite-3.2.0/scip-3.2.0/lib \
	  -lscip.linux.x86_64.gnu.opt \
	  -lobjscip.linux.x86_64.gnu.opt \
	  -llpispx.linux.x86_64.gnu.opt \
	  -lnlpi.cppad.linux.x86_64.gnu.opt \
	  -fomit-frame-pointer \
	  -mtune=native \
	  -lsoplex.linux.x86_64.gnu.opt \
	  -lm \
	  -lz \
	  -lzimpl.linux.x86_64.gnu.opt \
	  -lgmp -lreadline -lncurses\
	  -o dcstsolver

clean:
	rm -rf dcstsolver
	rm -rf src/dcstsolver.o
	rm -rf src/main.o
