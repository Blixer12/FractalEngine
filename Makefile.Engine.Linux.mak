BUILD_DIR := bin
OBJ_DIR := Object

ASSEMBLY := Fractal
EXTENSION := .so

# Aligned with Build.sh compiler flags
COMPILER_FLAGS := -g -Wall -Werror -std=c23 -fdeclspec -fPIC
INCLUDE_FLAGS := -IEngine/src -I$(VULKAN_SDK)/include
LINKER_FLAGS := -g -shared -lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$(VULKAN_SDK)/lib -L/usr/X11R6/lib
DEFINES := -D_DEBUG -DFEXPORT -D_GNU_SOURCE

SRC_FILES := $(shell find Engine/src -name "*.c")
DIRECTORIES := $(shell find Engine/src -type d)
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Scaffolding folder structure...
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
	@echo Done.

.PHONY: compile
compile: $(OBJ_FILES)
	@echo Compiling done.

.PHONY: link
link: scaffold compile
	@echo Linking $(ASSEMBLY)...
	@clang $(OBJ_FILES) -o $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: clean
clean:
	@rm -f $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION)
	@rm -rf $(OBJ_DIR)/Engine

$(OBJ_DIR)/%.c.o: %.c
	@echo Compiling $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)