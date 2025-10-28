# Makefile for Enterprise Storage Manager
# Final Project - Linux Systems Programming

# ============================================================================
# CONFIGURATION
# ============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE
CFLAGS += -I./include
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# Linker flags
LDFLAGS = -pthread

# Directories
SRC_DIR = src
INC_DIR = include
CLI_DIR = cli
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
CLI_FILES = $(wildcard $(CLI_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Object files
SRC_OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
CLI_OBJS = $(patsubst $(CLI_DIR)/%.c,$(OBJ_DIR)/cli_%.o,$(CLI_FILES))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/test_%.o,$(TEST_FILES))

# Executables
DAEMON = $(BUILD_DIR)/storage_daemon
CLI = $(BUILD_DIR)/storage_cli
TESTS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%,$(TEST_FILES))

# Install directories (for 'make install')
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
SBINDIR = $(PREFIX)/sbin
SYSCONFDIR = /etc
RUNDIR = /var/run
LOGDIR = /var/log

# ============================================================================
# TARGETS
# ============================================================================

.PHONY: all clean debug release test install uninstall help dirs

# Default target
all: debug

# Help target
help:
	@echo "Enterprise Storage Manager - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build debug version (default)"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  daemon       - Build storage daemon only"
	@echo "  cli          - Build CLI client only"
	@echo "  test         - Build and run tests"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install binaries (requires root)"
	@echo "  uninstall    - Uninstall binaries (requires root)"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  CC=$(CC)"
	@echo "  PREFIX=$(PREFIX)"

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: dirs $(DAEMON) $(CLI)
	@echo "Debug build complete!"

# Release build
release: CFLAGS += $(RELEASE_FLAGS)
release: dirs $(DAEMON) $(CLI)
	@echo "Release build complete!"

# Build daemon
daemon: dirs $(DAEMON)

# Build CLI
cli: dirs $(CLI)

# ============================================================================
# DAEMON BUILD
# ============================================================================

$(DAEMON): $(SRC_OBJS)
	@echo "Linking daemon: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Daemon built successfully!"

# ============================================================================
# CLI BUILD
# ============================================================================

$(CLI): $(CLI_OBJS) $(filter-out $(OBJ_DIR)/main.o,$(SRC_OBJS))
	@echo "Linking CLI: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "CLI built successfully!"

# ============================================================================
# OBJECT FILES
# ============================================================================

# Source object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

# CLI object files
$(OBJ_DIR)/cli_%.o: $(CLI_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

# Test object files
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

# ============================================================================
# TESTING
# ============================================================================

test: debug
	@echo "Running tests..."
	@if [ -d $(TEST_DIR) ] && [ -n "$(TEST_FILES)" ]; then \
		for test_file in $(TEST_FILES); do \
			test_name=$$(basename $$test_file .c); \
			test_exe=$(BUILD_DIR)/$$test_name; \
			if [ -f $$test_exe ]; then \
				echo "Running $$test_name..."; \
				$$test_exe || exit 1; \
			fi; \
		done; \
		echo "All tests passed!"; \
	else \
		echo "No tests found in $(TEST_DIR)"; \
	fi

# Individual test executables
$(BUILD_DIR)/test_%: $(OBJ_DIR)/test_%.o $(filter-out $(OBJ_DIR)/main.o,$(SRC_OBJS))
	@echo "Linking test: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lcunit

# ============================================================================
# KERNEL MODULE
# ============================================================================

.PHONY: kernel-module kernel-module-clean

kernel-module:
	@echo "Building kernel module..."
	@if [ -d kernel_module ]; then \
		$(MAKE) -C kernel_module; \
	else \
		echo "No kernel_module directory found"; \
	fi

kernel-module-clean:
	@if [ -d kernel_module ]; then \
		$(MAKE) -C kernel_module clean; \
	fi

# ============================================================================
# INSTALLATION
# ============================================================================

install: release
	@echo "Installing storage manager..."
	install -d $(DESTDIR)$(SBINDIR)
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SYSCONFDIR)
	install -m 755 $(DAEMON) $(DESTDIR)$(SBINDIR)/storage_daemon
	install -m 755 $(CLI) $(DESTDIR)$(BINDIR)/storage_cli
	@echo "Installation complete!"
	@echo ""
	@echo "To start the daemon:"
	@echo "  sudo $(SBINDIR)/storage_daemon"
	@echo ""
	@echo "To use the CLI:"
	@echo "  $(BINDIR)/storage_cli status"

uninstall:
	@echo "Uninstalling storage manager..."
	rm -f $(DESTDIR)$(SBINDIR)/storage_daemon
	rm -f $(DESTDIR)$(BINDIR)/storage_cli
	@echo "Uninstall complete!"

# ============================================================================
# CLEANUP
# ============================================================================

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	@$(MAKE) -s kernel-module-clean 2>/dev/null || true
	@echo "Clean complete!"

# ============================================================================
# DEVELOPMENT HELPERS
# ============================================================================

.PHONY: run run-daemon run-cli format check-style

# Run daemon in foreground (debug mode)
run-daemon: debug
	@echo "Starting daemon in foreground..."
	sudo $(DAEMON) --foreground --debug

# Run CLI with status command
run-cli: debug
	@echo "Running CLI..."
	$(CLI) status

# Format code (requires clang-format)
format:
	@echo "Formatting code..."
	@find $(SRC_DIR) $(INC_DIR) $(CLI_DIR) -name "*.c" -o -name "*.h" | \
		xargs clang-format -i
	@echo "Formatting complete!"

# Check code style
check-style:
	@echo "Checking code style..."
	@find $(SRC_DIR) $(INC_DIR) $(CLI_DIR) -name "*.c" -o -name "*.h" | \
		xargs clang-format --dry-run --Werror
	@echo "Style check passed!"

# ============================================================================
# CODE ANALYSIS
# ============================================================================

.PHONY: analyze valgrind cppcheck

# Static analysis with cppcheck
cppcheck:
	@echo "Running static analysis..."
	cppcheck --enable=all --suppress=missingIncludeSystem \
		-I$(INC_DIR) $(SRC_DIR) $(CLI_DIR)

# Memory leak detection with valgrind
valgrind: debug
	@echo "Running valgrind..."
	valgrind --leak-check=full --show-leak-kinds=all \
		--track-origins=yes $(DAEMON) --foreground --debug

# ============================================================================
# DOCUMENTATION
# ============================================================================

.PHONY: docs docs-clean

# Generate documentation with Doxygen
docs:
	@echo "Generating documentation..."
	@if command -v doxygen >/dev/null 2>&1; then \
		doxygen Doxyfile 2>&1 | grep -v "warning:"; \
		echo "Documentation generated in docs/html/"; \
	else \
		echo "Doxygen not found. Install with: sudo apt install doxygen"; \
	fi

docs-clean:
	@echo "Cleaning documentation..."
	@rm -rf docs/html docs/latex

# ============================================================================
# DEPENDENCIES
# ============================================================================

# Generate dependencies automatically
-include $(SRC_OBJS:.o=.d)
-include $(CLI_OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

# Dependency generation
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -MM -MT $(OBJ_DIR)/$*.o -MF $@ $<

# ============================================================================
# INFO
# ============================================================================

.PHONY: info

info:
	@echo "Build Information:"
	@echo "  CC:       $(CC)"
	@echo "  CFLAGS:   $(CFLAGS)"
	@echo "  LDFLAGS:  $(LDFLAGS)"
	@echo "  SRC_DIR:  $(SRC_DIR)"
	@echo "  BUILD_DIR: $(BUILD_DIR)"
	@echo "  PREFIX:   $(PREFIX)"
	@echo ""
	@echo "Source Files:"
	@echo "  $(SRC_FILES)"
	@echo ""
	@echo "Targets:"
	@echo "  DAEMON:   $(DAEMON)"
	@echo "  CLI:      $(CLI)"


