LLVM_CONFIG ?= llvm-config
CXX ?= clang++

BIN_DIR := bin
PROGRAM_DIR := fpc_programs
RESULT_DIR := expected_results
OBJ_DIR := obj

CXXFLAGS := -g -O0 `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs all` -std=c++17

.PHONY: lexer lexertest clean

lexer: $(OBJ_DIR)/lexer.o

parser: $(OBJ_DIR)/parser.o

$(OBJ_DIR)/lexer.o: lexer/lexer.cpp lexer/lexer.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c lexer/lexer.cpp -o $(OBJ_DIR)/lexer.o

$(OBJ_DIR)/parser.o: parser/parser.cpp parser/parser.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c parser/parser.cpp -o $(OBJ_DIR)/parser.o

$(OBJ_DIR)/ast.o: ast/ast.cpp ast/ast.h
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c ast/ast.cpp -o $(OBJ_DIR)/ast.o

lexertest: $(OBJ_DIR)/lexer.o lexer/lexertest.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/lexer.o lexer/lexertest.cpp -o $(BIN_DIR)/lexertest
	./$(BIN_DIR)/lexertest < $(PROGRAM_DIR)/fpc_program_1.fpc > temp.txt
	@if diff -q temp.txt $(RESULT_DIR)/lexer_result_1.txt; then echo "Lexer test passed"; else echo "Lexer test failed"; fi;
	@rm temp.txt

parsertest: $(OBJ_DIR)/lexer.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/parser.o parser/parsertest.cpp
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/lexer.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/parser.o parser/parsertest.cpp -o $(BIN_DIR)/parsertest
	./$(BIN_DIR)/parsertest < $(PROGRAM_DIR)/fpc_program_1.fpc


clean:
	rm -f $(BIN_DIR)/lexer $(BIN_DIR)/lexertest $(OBJ_DIR)/*.o
