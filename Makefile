# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20

# Project name and version
PROJECT_NAME := Unnamed-Roguelike
PROJECT_VERSION := 0.1.0

# Source files
SRCS := menutest.cpp

# FTXUI Library
FTXUI_DIR := ftxui
FTXUI_REPO := https://github.com/ArthurSonzogni/ftxui
FTXUI_TAG := v4.0.0

# FetchContent settings
FETCHCONTENT_DIR := $(FTXUI_DIR)/build
FETCHCONTENT_INCLUDE := -I$(FETCHCONTENT_DIR)/include
FETCHCONTENT_LIB := -L$(FETCHCONTENT_DIR)/lib -lftxui_screen -lftxui_dom -lftxui_component

# Target executable
TARGET := $(PROJECT_NAME)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(FETCHCONTENT_INCLUDE) -o $@ $^ $(FETCHCONTENT_LIB)

.PHONY: clean

clean:
	rm -f $(TARGET)

# Fetch and build the FTXUI library
fetch_ftxui:
	@if [ ! -d "$(FTXUI_DIR)" ]; then \
		echo "Fetching FTXUI library..."; \
		git clone $(FTXUI_REPO) --branch $(FTXUI_TAG) --depth 1 $(FTXUI_DIR); \
	fi
	@echo "Building FTXUI library..."
	@mkdir -p $(FETCHCONTENT_DIR)
	@cd $(FETCHCONTENT_DIR) && cmake -DCMAKE_BUILD_TYPE=Release ../../$(FTXUI_DIR)
	@make -C $(FETCHCONTENT_DIR)

.PHONY: fetch_ftxui

run: all
	./$(TARGET)

.PHONY: run
