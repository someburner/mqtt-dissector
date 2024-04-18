#
### Makefile for src
#

### Setup
PROJ_ROOT = $(PWD)

### Settings
TARGET    := app
BUILD_DIR := build
CXX       := g++

#THREADED  :=
THREADED  := -j2

### Flags
BASEFLAGS := -pedantic -O2 -std=c++17 -D_REETRANT

### Libs
LIBS := -ltins -lpcap -lpthread
#-lboost_system

### Objects
OBJ_LIST := build/main.o  build/dissect.o

ifneq ("$(wildcard $(PROJ_ROOT)/config.mk)","")
include $(PROJ_ROOT)/config.mk
DEFINES += -DAPP_NAME=\"$(TARGET)\"
else
$(error Please copy config.example.mk to config.mk and edit)
endif

### Logging
WITH_LOGURU ?= n

ifeq ("$(WITH_LOGURU)","y")
BASEFLAGS +=-DLOGURU_EN=1 -DLOGURU_WITH_FILEABS
LIBS +=-ldl
else
BASEFLAGS +=-DLOGURU_EN=0
endif

### Collect
DEFINES   := $(LOCAL_DEFINES)
CXXFLAGS  := $(BASEFLAGS) $(CXX_EXTRA)
CPPSRCS   = $(wildcard src/*.cpp) $(wildcard src/*.cpp)
CPPOBJS   = $(patsubst src/%.cpp,%.o, $(CPPSRCS))

### Default targets

default: prebuild
	make $(TARGET) $(THREADED)

.PHONY: prebuild postbuild
prebuild:
	rm -rf build
	@mkdir -p $(BUILD_DIR)

postbuild:
	@true

### Objects
build/%.o: src/%.cpp
	$(CXX) $(DEFINES) $(CFLAGS) $(CXXFLAGS) $(INC_DIRS) $(LD_DIRS) -c $<
	mv $*.o build/$*.o

### Apps
.PHONY: $(TARGET)
$(TARGET): $(OBJ_LIST)
	$(CXX) $(DEFINES) $(INC_DIRS) $(CXXFLAGS) $(OBJ_LIST) -o app $(LD_DIRS) $(LIBS)

### Cleanup
clean:
	rm -f *.o build/*.o src/*.o
	rm -f app
	rm -f src/mqtt/*.o

# EOF