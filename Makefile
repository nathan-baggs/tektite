CXX = cl.exe
LD = link.exe
CXXFLAGS = /nologo /std:c++latest /GS- /Qspectre- /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS /DWIN32_LEAN_AND_MEAN /DNOMINMAX
LDFLAGS = /nologo /ENTRY:main /SUBSYSTEM:CONSOLE /NODEFAULTLIB

SOURCES = main.cpp window.cpp
INC_LIBS = kernel32.lib user32.lib gdi32.lib opengl32.lib
OBJECTS = $(SOURCES:.cpp=.obj)
TARGET = game.exe
IMAGE = game.png

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) /OUT:$(TARGET) $(INC_LIBS)

%.obj: %.cpp
	$(CXX) $(CXXFLAGS) /c $< /Fo$@

clean:
	rm -f $(OBJECTS) $(TARGET)

image:
	ls -alh $(TARGET)
	truncate -s 6220800 $(TARGET)
	cat $(TARGET) | convert -size 1920x1080 -depth 8 rgb:- $(IMAGE)
	ls -alh $(IMAGE)


.PHONY: all clean

