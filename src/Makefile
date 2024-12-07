CXX = g++
CXXFLAGS  = -I /ucrt64/include/SDFL2 -I ../include/
LDFLAGS = -L /ucrt64/lib
LIBS = -lSDL2main -lSDL2 -lopengl32 -lglew32
TARGET = main
CLASSES = classes/Display.cpp classes/EventHandler.cpp classes/Shader.cpp classes/Cube.hpp classes/Renderer.cpp classes/Camera.cpp classes/Blocks/DirtBlock.cpp
SOURCES = main.cpp $(CLASSES)

all = $(TARGET)

$(TARGET) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)