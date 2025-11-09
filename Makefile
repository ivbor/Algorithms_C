.PHONY: all build test coverage format tidy stress clean

BUILD_DIR ?= build

all: build

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

stress: build
	cmake --build $(BUILD_DIR) --target stress
	"$(BUILD_DIR)/sorting_stress"

coverage:
	bash scripts/run_coverage.sh

format:
	clang-format -i $(shell find include src tests -name '*.c' -o -name '*.h')

tidy: build
	cmake --build $(BUILD_DIR) --target clang-tidy

clean:
	rm -rf $(BUILD_DIR)
