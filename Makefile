.PHONY: clean all

CXX=g++
FLAGS=-march=native -O3 -ffast-math
BASEFLAGS=-fno-tree-vectorize
OPTFLAGS=-mprefer-vector-width=256

SRC=$(wildcard ./benchmark/*.cpp)
OBJ=$(patsubst %.cpp, %.o, ${SRC})
OBJVEC=$(patsubst %.cpp, %.vec.o, ${SRC})

all: clean opencv-vec opencv-base
	@echo ${SRC}

opencv-base: main.o ${OBJ}
	${CXX} ${FLAGS} -O0 -o $@ $^

opencv-vec: main.o ${OBJVEC}
	${CXX} ${FLAGS} -O0 -o $@ $^

main.o: main.cpp
	${CXX} ${FLAGS} -O0 -I./ -c -o $@ $<

${OBJ}: %.o:%.cpp
	${CXX} ${FLAGS} ${BASEFLAGS} -I./ -c -o $@ $<

${OBJVEC}: %.vec.o:%.cpp
	${CXX} ${FLAGS} ${OPTFLAGS} -I./ -c -o $@ $<

clean:
	rm -f benchmark/*.o *.o opencv-vec opencv-base
