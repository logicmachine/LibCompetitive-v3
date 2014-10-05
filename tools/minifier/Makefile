CC=clang++
PROJECT_DIR = .
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
	-lclangFormat \
	-lclangRewriteCore \
	-lclangRewriteFrontend \
	-lclangASTMatchers \
	-lclangFrontendTool \
	-lclangDynamicASTMatchers \
	-lclangApplyReplacements

SAMPLE_DIR = $(PROJECT_DIR)/sample

MAIN_SRC = main.cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)

ANALYZER_SRC = DependencyAnalyzer.cpp
ANALYZER_OBJ = $(ANALYZER_SRC:.cpp=.o)

MINIFIER_SRC = Minifier.cpp
MINIFIER_OBJ = $(MINIFIER_SRC:.cpp=.o)

TOOL = $(BIN_DIR)/minifier
CONFIG = llvm-config-3.5
LFLAGS = --ldflags --libs
CFLAGS = --cxxflags


all:$(MAIN_OBJ) $(ANALYZER_OBJ) $(MINIFIER_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) -g $(MAIN_OBJ) $(ANALYZER_OBJ) $(MINIFIER_OBJ) $(LIBS) `$(CONFIG) $(LFLAGS)` -static -ltinfo -lncurses -lstdc++ -lpthread -lm -ldl -lz -o $(TOOL) 

$(MAIN_OBJ):$(MAIN_SRC)
	$(CC) -g $(MAIN_SRC) `$(CONFIG) $(CFLAGS)` -fno-rtti -c -o $(MAIN_OBJ) 

$(ANALYZER_OBJ):$(ANALYZER_SRC)
	$(CC) -g $(ANALYZER_SRC) `$(CONFIG) $(CFLAGS)` -fno-rtti -c -o $(ANALYZER_OBJ) 

$(MINIFIER_OBJ):$(MINIFIER_SRC)
	$(CC) -g $(MINIFIER_SRC) `$(CONFIG) $(CFLAGS)` -fno-rtti -c -o $(MINIFIER_OBJ) 

clean:
	rm -rf $(MAIN_OBJ) $(ANALYZER_OBJ) $(MINIFIER_OBJ) $(TOOL)

