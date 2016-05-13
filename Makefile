#Makefile from http://stackoverflow.com/questions/7004702/how-can-i-create-a-makefile-for-c-projects-with-src-obj-and-bin-subdirectories

TARGET = raytrace

CC = g++
CFLAGS = -std=c++11 -g -O3 -DCMAKE_BUILD_TYPE=Release

LINKER = g++ -o

SRCDIR = src
OBJDIR = o
RSRCDIR = resources

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(OBJECTS)
   
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
   
clean :
	rm *.tga *.exe

wipe :
	rm *.tga *.exe $(OBJDIR)/*.o
   
