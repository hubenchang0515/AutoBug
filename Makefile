TARGET := autobug
LIBS := -lOpenCL
CXXFLAGS := -W -Wall -Wextra -Werror -O3 -std=c++11

PREFIX := /usr/local

SRCS := $(wildcard *.cpp)
HEADERS := $(wildcard *.h)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))
INSTALL_PATH := $(DESTDIR)$(PREFIX)

.PHONY: all clean install uninstall print

all: $(TARGET)

install: all
	install -m0644 $(TARGET) $(INSTALL_PATH)/bin

uninstall:
	$(RM) $(INSTALL_PATH)/bin/$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJS)

print:
	@echo "DESTDIR : $(DESTDIR)"
	@echo "PREFIX  : $(PREFIX)"
	@echo "SRCS    : $(SRCS)"
	@echo "HEADERS : $(HEADERS)"
	@echo "OBJS    : $(OBJS)"