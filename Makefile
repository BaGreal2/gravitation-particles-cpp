CXX = g++
CXXFLAGS = -O3 -ffunction-sections -fdata-sections -flto -march=native -std=c++20 -I/opt/homebrew/Cellar/sfml/2.6.1/include -L/opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system
SRC = src/*.cpp
OUT = main

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
