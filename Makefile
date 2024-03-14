LLVM_CONFIG ?= llvm-config
CXX ?= clang++

BIN_DIR := bin
PROGRAM_DIR := fpc_programs
RESULT_DIR := expected_results
OBJ_DIR := obj

PREFLAGS := -g -O3
LLVMCOMPFLAGS := `$(LLVM_CONFIG) --cxxflags`
LLVMLINKFLAGS := `$(LLVM_CONFIG) --ldflags --system-libs --libs core`
CXXFLAGS := -g -O0 $(LLVMCOMPFLAGS) -std=c++17

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

$(OBJ_DIR)/compiler.o: compiler.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c compiler.cpp -o $(OBJ_DIR)/compiler.o

lexertest: $(OBJ_DIR)/lexer.o lexer/lexertest.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/lexer.o lexer/lexertest.cpp -o $(BIN_DIR)/lexertest $(LLVMLINKFLAGS)
	./$(BIN_DIR)/lexertest < $(PROGRAM_DIR)/fpc_program_1.fpc > temp.txt
	@if diff -q temp.txt $(RESULT_DIR)/lexer_result_1.txt; then echo "Lexer test passed"; else echo "Lexer test failed"; fi;
	@rm temp.txt

parsertest: $(OBJ_DIR)/lexer.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/parser.o parser/parsertest.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/lexer.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/parser.o parser/parsertest.cpp -o $(BIN_DIR)/parsertest $(LLVMLINKFLAGS)
	./$(BIN_DIR)/parsertest < $(PROGRAM_DIR)/fpc_program_1.fpc

compiler: $(OBJ_DIR)/compiler.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/parser.o compiler.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)/compiler.o $(OBJ_DIR)/lexer.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/parser.o -o $(BIN_DIR)/compiler $(LLVMLINKFLAGS)
	./$(BIN_DIR)/compiler < $(PROGRAM_DIR)/fpc_program_1.fpc

clean:
	rm -f $(BIN_DIR)/lexer $(BIN_DIR)/lexertest $(OBJ_DIR)/*.o
