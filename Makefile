CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 -Iinclude
BUILD_DIR := build
APP := $(BUILD_DIR)/motor_inferencias.exe
TEST_APP := $(BUILD_DIR)/inference_engine_tests.exe
GUI ?= 1
IMGUI_DIR ?= third_party/imgui
GLFW_CFLAGS ?= $(shell pkg-config --cflags glfw3 2>/dev/null)
GLFW_LIBS ?= $(shell pkg-config --libs glfw3 2>/dev/null)

LIB_SOURCES := \
	src/Formula.cpp \
	src/AtomExtractor.cpp \
	src/NaturalLanguageParser.cpp \
	src/Evaluator.cpp \
	src/SemanticTableau.cpp \
	src/ModelReporter.cpp

LIB_OBJECTS := $(LIB_SOURCES:src/%.cpp=$(BUILD_DIR)/src/%.o)
ifeq ($(GUI),1)
APP_SOURCES := \
	src/GuiApp.cpp \
	src/gui_main.cpp \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
APP_OBJECTS := $(APP_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
GUI_CXXFLAGS := $(GLFW_CFLAGS) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
APP_LIBS := $(GLFW_LIBS) -lopengl32 -lgdiplus -lwinmm
else
APP_SOURCES := src/main.cpp
APP_OBJECTS := $(BUILD_DIR)/src/main.o
GUI_CXXFLAGS :=
APP_LIBS :=
endif
TEST_SOURCES := \
	tests/test_main.cpp \
	tests/test_formulas.cpp \
	tests/test_atoms.cpp \
	tests/test_parser.cpp \
	tests/test_evaluator.cpp \
	tests/test_tableau.cpp
TEST_OBJECTS := $(TEST_SOURCES:tests/%.cpp=$(BUILD_DIR)/tests_%.o)

.PHONY: all run run-console test clean

all: $(APP)

run: $(APP)
	./$(APP)

run-console:
	$(MAKE) GUI=0 run

test: $(TEST_APP)
	./$(TEST_APP)

$(APP): $(LIB_OBJECTS) $(APP_OBJECTS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(GUI_CXXFLAGS) $^ $(APP_LIBS) -o $@

$(TEST_APP): $(LIB_OBJECTS) $(TEST_OBJECTS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/src/%.o: src/%.cpp
	mkdir -p "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(GUI_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(GUI_CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tests_%.o: tests/%.cpp | $(BUILD_DIR)
	mkdir -p "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p "$(BUILD_DIR)"

clean:
	rm -rf "$(BUILD_DIR)"
