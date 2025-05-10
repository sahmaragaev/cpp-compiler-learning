CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I$(SRC_DIR)
TARGET = nova
SRC_DIR = src
BUILD_DIR = build
EXAMPLES_DIR = examples

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: $(TARGET)
	@echo "Installing Nova compiler..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Nova compiler installed successfully!"

install-completion:
	@echo "Installing shell completion..."
	@if [ "$$SHELL" = "/bin/zsh" ] || [ "$$SHELL" = "/usr/bin/zsh" ]; then \
		echo "Installing zsh completion..."; \
		sudo mkdir -p /usr/local/share/zsh/site-functions; \
		sudo cp completions/_nova /usr/local/share/zsh/site-functions/_nova; \
		echo "Zsh completion installed! Restart your shell."; \
	else \
		echo "Installing bash completion..."; \
		if [ -d /etc/bash_completion.d ]; then \
			sudo cp completions/nova-completion.sh /etc/bash_completion.d/nova; \
			echo "Bash completion installed to /etc/bash_completion.d/"; \
		else \
			mkdir -p $$HOME/.nova; \
			cp completions/nova-completion.sh $$HOME/.nova/; \
			echo "Bash completion installed to $$HOME/.nova/"; \
			echo "Add this line to your .bashrc:"; \
			echo "source $$HOME/.nova/nova-completion.sh"; \
		fi \
	fi

install-all: install install-completion install-vscode

install-vscode:
	@echo "Installing VS Code extension..."
	@if [ -d "$$HOME/.vscode/extensions" ]; then \
		cp -r vscode-extension "$$HOME/.vscode/extensions/nova-language"; \
		echo "VS Code extension installed!"; \
		echo "Restart VS Code to activate."; \
	else \
		echo "VS Code extensions directory not found."; \
		echo "Please install VS Code first."; \
	fi

run-examples: $(TARGET)
	@echo "=== Factorial Example ==="
	./$(TARGET) $(EXAMPLES_DIR)/factorial.nova
	@echo
	@echo "=== Fibonacci Example ==="
	./$(TARGET) $(EXAMPLES_DIR)/fibonacci.nova

.PHONY: all clean install install-completion install-all install-vscode run-examples