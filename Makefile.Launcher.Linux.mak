BUILD_DIR := bin
OBJ_DIR := Object

# Matches assembly name 'FractalEngine' from Build.sh
ASSEMBLY := FractalEngine
EXTENSION := 

# Aligned with Build.sh compiler & linker flags
COMPILER_FLAGS := -g -Wall -Werror -std=c23 -fdeclspec -fPIC
INCLUDE_FLAGS := -ILauncher/src -IEngine/src -I$(VULKAN_SDK)/include
LINKER_FLAGS := -L./$(BUILD_DIR) -lFractal -Wl,-rpath,'$$ORIGIN'
DEFINES := -D_DEBUG -DFIMPORT -D_GNU_SOURCE

SRC_FILES := $(shell find Launcher/src -name "*.c")
DIRECTORIES := $(shell find Launcher/src -type d)
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
	@clang $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: clean
clean:
	@rm -f $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION)
	@rm -rf $(OBJ_DIR)/Launcher

$(OBJ_DIR)/%.c.o: %.c
	@echo Compiling $<...
	@clang $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)