#
# Helper script to build the project using CMake and CI/CD pipelines
#

# Variables
BUILD_DIR := build

.PHONY: all install dependency build run clean doc test

all: install dependency build run doc test

# Install dependencies
dependency:
	@echo "Installing dependencies..."
ifeq ($(OS), Windows_NT)
	@choco install gcc g++ cmake llvm ninja doxygen graphviz clang-format  lcov
else
	@sudo apt-get install -y gcc g++ cmake llvm ninja-build doxygen graphviz clang-format texlive
endif

# Dependency graph generation
dep_graph:
	@echo "Generating dependency graph..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. --graphviz=graph.dot
	@dot -Tpng $(BUILD_DIR)/graph.dot -o $(BUILD_DIR)/graphImage.png

# Prepare the build directory
clean:
	@echo "Preparing build directory..."
	@rm -rf $(BUILD_DIR)
	@mkdir $(BUILD_DIR)

	@echo "Cleaning cmake cache..."
	@rm -rf $(BUILD_DIR)/CMakeCache.txt $(BUILD_DIR)/CMakeFiles $(BUILD_DIR)/Makefile $(BUILD_DIR)/cmake_install.cmake

# Build debug
build-debug:
	@echo "Cleaning cmake cache..."
	@rm -rf $(BUILD_DIR)/CMakeCache.txt $(BUILD_DIR)/CMakeFiles $(BUILD_DIR)/Makefile $(BUILD_DIR)/cmake_install.cmake

	@echo "Configuring Debug build..."
	@cmake -DCMAKE_BUILD_TYPE=Debug \
	       -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
	       --no-warn-unused-cli \
	       -S . \
	       -B $(BUILD_DIR) \
	       -G "Ninja"

	@echo "Building Debug..."
	@cd $(BUILD_DIR) && cmake .. && cmake --build . --config Debug -j 4

# Build release
build-release:
	@echo "Cleaning cmake cache..."
	@rm -rf $(BUILD_DIR)/CMakeCache.txt $(BUILD_DIR)/CMakeFiles $(BUILD_DIR)/Makefile $(BUILD_DIR)/cmake_install.cmake

	@echo "Configuring release build..."
	@cmake -DCMAKE_BUILD_TYPE=Release \
	       -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
	       --no-warn-unused-cli \
	       -S . \
	       -B $(BUILD_DIR) \
	       -G "Ninja" \
		   -DENABLE_TESTS=OFF
		   
	@echo "Building release..."
	@cmake --build $(BUILD_DIR) --config Release --target all -- -j 4

# Run the project
run:
	@echo "Running the project..."
ifeq ($(OS), Windows_NT)
	@./$(BUILD_DIR)/build/Debug/*.exe
else
	@./$(BUILD_DIR)/build/Debug/*
endif

# Generate documentation (custom target)
html:
	@echo "Generating html doxygen documentation..."
	@cd $(BUILD_DIR) && cmake --build . --target html

pdf:
	@echo "Generating pdf-latex doxygen documentation..."
	@cd $(BUILD_DIR) && cmake --build . --target pdf

doc: html pdf
	@echo "All Documentation generated successfully!"

test:
	@echo "Running tests..."
	@cd $(BUILD_DIR) && cmake .. -DENABLE_TESTS=ON && cmake --build . && ctest --output-on-failure

# Define variables
PLANTUML_DIRS = docs/diagrams/plantuml docs/diagrams/plantuml/sequence-diagrams docs/diagrams/plantuml/sequence-diagrams/Modules docs/diagrams/plantuml/sequence-diagrams/Requests
MERMAID_DIRS = docs/diagrams/mermaid
SVG_DIRS = docs/diagrams/plantuml docs/diagrams/mermaid docs/diagrams/plantuml/sequence-diagrams/Modules docs/diagrams/plantuml/sequence-diagrams/Requests

# Target to generate diagrams
diagrams: build
ifeq ($(OS), Windows_NT)
	@echo "Generating diagrams for Windows..."
	mkdir -p $(PLANTUML_DIRS) $(MERMAID_DIRS)
	clang-uml -g plantuml -g json -g mermaid -p
	@echo "Converting .puml files to SVG and PNG..."
	plantuml -tsvg -nometadata -o plantuml docs/diagrams/*.puml
	plantuml -tpng -nometadata -o plantuml docs/diagrams/*.puml
	plantuml -tsvg -nometadata -o ../../plantuml/sequence-diagrams/Modules docs/diagrams/sequence-diagrams/Modules/*.puml
	plantuml -tpng -nometadata -o ../../plantuml/sequence-diagrams/Modules docs/diagrams/sequence-diagrams/Modules/*.puml
	plantuml -tsvg -nometadata -o ../../plantuml/sequence-diagrams/Requests docs/diagrams/sequence-diagrams/Requests/*.puml
	plantuml -tpng -nometadata -o ../../plantuml/sequence-diagrams/Requests docs/diagrams/sequence-diagrams/Requests/*.puml
	@echo "Converting .mmd files to SVG..."
	py utils/generate_mermaid.py "docs/diagrams/*.mmd"
	@echo "Formatting generated SVG files..."
	py utils/format_svg.py $(wildcard docs/diagrams/plantuml/*.svg) $(wildcard docs/diagrams/mermaid/*.svg) $(wildcard docs/diagrams/plantuml/sequence-diagrams/Modules/*.svg) $(wildcard docs/diagrams/plantuml/sequence-diagrams/Requests/*.svg)
	@echo "Done!"

else
	@echo "Installing dependencies for Unix-like systems..."
	sudo apt-get install -y plantuml npm python3 python3-pip python3-yaml
	npm install -g mermaid.cli
	pip install pyyaml cairosvg
	@echo "Installing clang-uml..."
	sudo add-apt-repository ppa:bkryza/clang-uml
	sudo apt update
	sudo apt install clang-uml
	@echo "Generating diagrams..."
	@mkdir -p $(PLANTUML_DIRS) $(MERMAID_DIRS)
	@clang-uml -g plantuml -g json -g mermaid -p
	@echo "Converting .puml files to SVG..."
	@plantuml -tsvg -nometadata -o plantuml docs/diagrams/*.puml
	@echo "Converting .mmd files to SVG..."
	@py utils/generate_mermaid.py "docs/diagrams/*.mmd"
	@echo "Formatting generated SVG files..."
	@py utils/format_svg.py $(wildcard docs/diagrams/plantuml/*.svg) $(wildcard docs/diagrams/mermaid/*.svg)
	@echo "Done!"
endif
