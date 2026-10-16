CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude
TARGET = pergantian_jadwal
OBJECTS = src/main.o src/data_manager.o src/file_manager.o src/menu.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

src/main.o: src/main.cpp include/models.h include/data_manager.h include/file_manager.h include/menu.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o src/main.o

src/data_manager.o: src/data_manager.cpp include/data_manager.h include/models.h
	$(CXX) $(CXXFLAGS) -c src/data_manager.cpp -o src/data_manager.o

src/file_manager.o: src/file_manager.cpp include/file_manager.h include/data_manager.h include/models.h
	$(CXX) $(CXXFLAGS) -c src/file_manager.cpp -o src/file_manager.o

src/menu.o: src/menu.cpp include/menu.h include/data_manager.h include/models.h
	$(CXX) $(CXXFLAGS) -c src/menu.cpp -o src/menu.o

run: all
	./$(TARGET)

clean:
	-del /Q $(TARGET).exe $(TARGET) src\\*.o 2>nul || exit 0
