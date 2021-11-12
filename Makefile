TARGET := autobug
LIBS := -lOpenCL
CXXFLAGS := -W -Wall -Wextra -Werror -O3 -std=c++11

PREFIX := /usr/local
INSTALL_PATH := $(DESTDIR)$(PREFIX)

SRCS := $(wildcard *.cpp) Accelerator.cpp
HEADERS := $(wildcard *.h)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

.PHONY: prepare all clean install uninstall print profile

all: $(TARGET) prepare

profile: $(SRCS)
	$(CXX) -o $(TARGET) $^ $(LIBS) -g -pg
	./$(TARGET)
	gprof ./$(TARGET) gmon.out -p > profile.txt

install: all
	install -m0644 $(TARGET) $(INSTALL_PATH)/bin

uninstall:
	$(RM) $(INSTALL_PATH)/bin/$(TARGET)

$(TARGET): $(OBJS) 
	$(CXX) -o $@ $^ $(LIBS)

Accelerator.cpp: Accelerator.cxx kernel.cl prepare.sh
	bash -c ./prepare.sh

clean:
	$(RM) $(OBJS) Accelerator.cpp

print:
	@echo "DESTDIR : $(DESTDIR)"
	@echo "PREFIX  : $(PREFIX)"
	@echo "SRCS    : $(SRCS)"
	@echo "HEADERS : $(HEADERS)"
	@echo "OBJS    : $(OBJS)"