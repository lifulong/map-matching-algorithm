
#src=./.log.h ./common.cpp ./error.h ./common-struct.h ./build-grid-index.cpp ./common.h
src=./build-grid-index.cpp ./common.cpp
obj=./build-grid-index.o ./common.o

BUILD_MACRO=-D TEST_MAPINDEX
COMPILER=g++
CPP=cpp
CPP_FLAGS=-g -Wall

build_grid_index: $(obj)
	$(COMPILER)  $(BUILD_MACRO) $(CPP_FLAGS) $(obj) -o $@

#$(obj): $(src)
.cpp.o: $(src)
	$(COMPILER)  $(BUILD_MACRO) $(CPP_FLAGS) -c $^ -o $@

clean:
	-rm *.o

