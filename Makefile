CC = gcc
CXX = g++
CFLAGS = $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

TARGET = snake
WRAPPER = libsdl_wrapper.so
WRAPPER_SRC = sdl_wrapper.cpp
MAIN_SRC = main.c

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(WRAPPER)
	$(CC) $(MAIN_SRC) -o $(TARGET) -L. -lsdl_wrapper $(CFLAGS) $(LDFLAGS)

$(WRAPPER): $(WRAPPER_SRC)
	$(CXX) -c $(WRAPPER_SRC) -o sdl_wrapper.o $(CFLAGS)
	$(CXX) -shared -o $(WRAPPER) sdl_wrapper.o $(LDFLAGS)

clean:
	rm -f $(TARGET) sdl_wrapper.o $(WRAPPER)


