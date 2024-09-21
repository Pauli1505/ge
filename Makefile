CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/SDL2 -I/usr/include/GL
LDFLAGS = -lSDL2 -lGLEW -lGL
SU = sudo

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=build/%.o)
EXEC = build/GE

all: $(EXEC)

shaders:
	$(SU) mkdir -p /usr/local/games/shaders/

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	$(SU)	cp -r src/fragment.glsl /usr/local/games/shaders/fragment.glsl
	$(SU)	cp -r src/vertex.glsl /usr/local/games/shaders/vertex.glsl

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f build/*.o $(EXEC) build/*
	$(SU) rm -rf /usr/local/games/shaders
