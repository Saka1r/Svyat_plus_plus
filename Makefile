CXX = clang++
CXXFLAGS = -std=c++17 -O2 -static
SRC = src/main.cpp
BIN = svyat.exe

.PHONY: all run clean

all: $(BIN)

$(BIN): $(SRC)
	@echo "Compiling and linking..."
	$(CXX) $(CXXFLAGS) $< -o $@

run: all
	./$(BIN)

clean:
	@echo Cleaning $(TARGET)...
	del svyat.exe 
