CXX = x86_64-w64-mingw32-g++
LD = x86_64-w64-mingw32-ld
CXXFLAGS = -DM_PI=3.14159265358979323846 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DNOMINMAX -Os -std=c++23 -I/usr/x86_64-w64-mingw32/include -I. -nostdlib -nodefaultlibs  -fno-builtin
LDFLAGS = -L/usr/x86_64-w64-mingw32/lib -e my_entry -nostdlib -T linker_script.ld

SOURCES = main.cpp window.cpp buffer.cpp shader.cpp material.cpp mesh.cpp camera.cpp dyn_array.cpp sound_player.cpp
INC_LIBS = -lkernel32 -luser32 -lgdi32 -lopengl32 -ladvapi32 -lwinmm
OBJECTS = $(SOURCES:.cpp=.obj)
TARGET = game.exe
IMAGE = game.png

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(TARGET) $(INC_LIBS)

%.obj: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

image:
	ls -alh $(TARGET)
	truncate -s 6220800 $(TARGET)
	cat $(TARGET) | convert -size 1920x1080 -depth 8 rgb:- $(IMAGE)
	ls -alh $(IMAGE)


.PHONY: all clean

