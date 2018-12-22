CC := g++
FLAGS := -std=c++11 -g -Wall
BUL_DIR := build
BIN_DIR := bin
SRC_DIR := src

$(BIN_DIR)/main: $(BUL_DIR)/main.o\
					$(BUL_DIR)/greedy.o\
					$(BUL_DIR)/SA.o\
					$(BUL_DIR)/hillClimbing.o\
					$(BUL_DIR)/randomGreedy.o
	$(CC) $(FLAGS) $^ -o $@

$(BUL_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(FLAGS) -c $^ -o $@

clean:
	rm $(BUL_DIR)/* -f
	rm $(BIN_DIR)/* -f

run: $(BIN_DIR)/main
	@$(BIN_DIR)/main

play:
	@echo "Hello, world"

rerun: clean run
