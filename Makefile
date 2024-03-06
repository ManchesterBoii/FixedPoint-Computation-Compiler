LLVM_CONFIG ?= llvm-config
CXX ?= clang++

SRC_DIR := src
BIN_DIR := bin
BUILD_DIR := build

CXXFLAGS := -std=c++11 -I ./src

fpc-tests: fpc
	$(BIN_DIR)/$^ ./fpc_programs/test_file_1.fpc > temp.txt
	@if diff -q temp.txt ./expected_results/test_file_1.txt; then echo "Success!"; else echo "test diff mismatch"; fi;

fpc:
	@mkdir -p $(BIN_DIR)
	$(CXX) -g -O0 compiler.cpp `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs all` -o $(BIN_DIR)/fpc

clean:
	\rm -rf $(BUILD_DIR) $(BIN_DIR)

test: fpc-tests