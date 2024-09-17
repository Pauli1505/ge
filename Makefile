CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/SDL2 -I/usr/include/GL
LDFLAGS = -lSDL2 -lGLEW -lGL

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)
EXEC = build/GE

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	cp -r src/fragment.glsl build/
	cp -r src/vertex.glsl build/

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f build/*.o $(EXEC) build/*

