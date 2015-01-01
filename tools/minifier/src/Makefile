CC=clang++
PROJECT_DIR = ..
BIN_DIR = $(PROJECT_DIR)/bin
LIBS=\
	-lclangFrontend \
	-lclangParse \
	-lclangSema \
	-lclangAnalysis \
	-lclangAST \
	-lclangBasic \
	-lclangDriver \
	-lclangSerialization \
	-lclangTooling \
	-lclangIndex \
	-lclangEdit \
	-lclangLex \
	-lclangFrontendTool

MAIN_SRC = main.cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

ANALYZER_SRC = dependency_analyzer.cpp
ANALYZER_HPP = $(ANALYZER_SRC:.cpp=.hpp)
ANALYZER_OBJ = $(ANALYZER_SRC:.cpp=.o)

MINIFIER_HPP = minifier.hpp

TOOL = $(BIN_DIR)/minifier
CONFIG = llvm-config-3.5
LFLAGS = --ldflags --libs
CFLAGS = --cxxflags


$(TOOL):$(MAIN_OBJ) $(ANALYZER_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) -std=c++11 -g $(MAIN_OBJ) $(ANALYZER_OBJ) $(LIBS) `$(CONFIG) $(LFLAGS)` -static -ltinfo -lncurses -lstdc++ -lpthread -lm -ldl -lz -o $(TOOL)

$(MAIN_OBJ):$(MAIN_SRC) $(ANALYZER_HPP) $(MINIFIER_HPP)
	$(CC) -std=c++11 -g $(MAIN_SRC) `$(CONFIG) $(CFLAGS)` -fno-rtti -c -o $(MAIN_OBJ) 

$(ANALYZER_OBJ):$(ANALYZER_SRC) $(ANALYZER_HPP)
	$(CC) -std=c++11 -g $(ANALYZER_SRC) `$(CONFIG) $(CFLAGS)` -fno-rtti -c -o $(ANALYZER_OBJ) 

.PHONY: test
test:$(TOOL)
	python ../test/run_test.py $(TOOL)

clean:
	rm -rf $(MAIN_OBJ) $(ANALYZER_OBJ) $(MINIFIER_OBJ) $(TOOL)

