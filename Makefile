.PHONY: all build test coverage format format-check tidy stress clean algorithms structures utils

SOURCE_DIR ?= .
PROJECT_DIR ?= Algorithms_C
BUILD_DIR ?= build

all: build

build:
	cmake -S $(SOURCE_DIR) -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

algorithms: build
	cmake --build $(BUILD_DIR) --target algorithms_c_algorithms

structures: build
	cmake --build $(BUILD_DIR) --target algorithms_c_structures

utils: build
	cmake --build $(BUILD_DIR) --target algorithms_c_utils

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

stress: build
	cmake --build $(BUILD_DIR) --target stress
	$(BUILD_DIR)/Algorithms_C/sorting_stress

coverage:
	bash $(PROJECT_DIR)/scripts/run_coverage.sh

format:
	bash $(PROJECT_DIR)/scripts/check-format.sh --apply

format-check:
	bash $(PROJECT_DIR)/scripts/check-format.sh

tidy: build
	cmake --build $(BUILD_DIR) --target clang-tidy

clean:
	rm -rf $(BUILD_DIR)
