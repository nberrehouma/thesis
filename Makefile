GXX=g++
SC=/home/nabil/programming_workspace/eclipse/systemc/systemc-2.3.3
ARCH=lib-linux64
FLAGS    = -Wall  -I $(SC)/include  -Wno-deprecated -O2 -L$(SC)/$(ARCH) -lsystemc
export LD_LIBRARY_PATH=$(SC)/$(ARCH)

build: main.cpp initiator.h my_mem.h top.h
	$(GXX) $(FLAGS) main.cpp -o sim
initiator.o : initiator.h initiator.cpp
	$(GXX) $(FLAGS) -o intiaitor.o -c initiator.cpp
top.o: top.h top.cpp
	$(GXX) $(FLAGS)-o top.o -c top.cpp
my_mem.o : my_mem.h my_mem.cpp
	$(GXX) $(FLAGS) -o my_mem.o my_mem.cpp
run: build
	./sim
