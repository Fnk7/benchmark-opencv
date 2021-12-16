WIDTH=1920
HEIGHT=1080

.PHONY: clean all

CXX=clang++
FLAGS=-march=native -O3 -ffast-math -DINPUT_WIDTH=${WIDTH} -DINPUT_HEIGHT=${HEIGHT}
BASEFLAGS=-fno-tree-vectorize
SSEFLAGS=-mprefer-vector-width=128 -mno-avx2
AVX2FLAGS=-mprefer-vector-width=256 -mno-avx512f
AVX512FLAGS=-mprefer-vector-width=512

SRC=$(wildcard ./benchmark/*.cpp)
OBJ=$(patsubst %.cpp, %.o, ${SRC})
OBJSSE=$(patsubst %.cpp, %.sse.o, ${SRC})
OBJAVX2=$(patsubst %.cpp, %.avx2.o, ${SRC})
OBJAVX512=$(patsubst %.cpp, %.avx512.o, ${SRC})

all: clean opencv-sse opencv-avx2 opencv-avx512 opencv-base
	@echo ${FLAGS}

opencv-base: main.o ${OBJ}
	${CXX} ${FLAGS} -O0 -o $@ $^

opencv-sse: main.o ${OBJSSE}
	${CXX} ${FLAGS} -O0 -o $@ $^

opencv-avx2: main.o ${OBJAVX2}
	${CXX} ${FLAGS} -O0 -o $@ $^

opencv-avx512: main.o ${OBJAVX512}
	${CXX} ${FLAGS} -O0 -o $@ $^

main.o: main.cpp
	${CXX} ${FLAGS} -O0 -I./ -c -o $@ $<

${OBJ}: %.o:%.cpp
	${CXX} ${FLAGS} ${BASEFLAGS} -I./ -c -o $@ $<

${OBJSSE}: %.sse.o:%.cpp
	${CXX} ${FLAGS} ${SSEFLAGS} -I./ -c -o $@ $<

${OBJAVX2}: %.avx2.o:%.cpp
	${CXX} ${FLAGS} ${AVX2FLAGS} -I./ -c -o $@ $<

${OBJAVX512}: %.avx512.o:%.cpp
	${CXX} ${FLAGS} ${AVX512FLAGS} -I./ -c -o $@ $<

clean:
	rm -f benchmark/*.o *.o opencv-*
