DIR := $(subst /,\,${CURDIR})
BUILD_DIR := bin
OBJ_DIR := Object

ASSEMBLY := Tests
EXTENSION := .exe

# Compiler flags aligned with C23 zero-CRT engine setup
COMPILER_FLAGS := /Z7 /W4 /WX /TC -Werror=vla /std:clatest /clang:-MMD /clang:-MF /clang:"$@.d"
INCLUDE_FLAGS := /IEngine\src /ITests\src /I"$(VULKAN_SDK)\Include"

# Linker flags
LINKER_FLAGS := /link /DEBUG /PDB:"$(BUILD_DIR)\$(ASSEMBLY).pdb" /LIBPATH:"$(BUILD_DIR)" Fractal.lib user32.lib /OUT:"$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)"
DEFINES := /D_DEBUG /DFIMPORT /D_CRT_SECURE_NO_WARNINGS

# Recursive wildcard function for nested test suites
Wildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call Wildcard,$d/,$2))

SRC_FILES := $(call Wildcard,Tests/src/,*.c)
OBJ_FILES := $(SRC_FILES:%.c=$(OBJ_DIR)/%.c.obj)

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Scaffolding folder structure...
	-@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	-@if not exist "$(OBJ_DIR)\Tests\src" mkdir "$(OBJ_DIR)\Tests\src"
	@echo Done.

.PHONY: compile
compile: $(OBJ_FILES)
	@echo Compiling done.

.PHONY: link
link: scaffold compile
	@echo Linking $(ASSEMBLY)...
	@clang-cl $(OBJ_FILES) $(LINKER_FLAGS)

.PHONY: clean
clean:
	@if exist "$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)" del /f /q "$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)"
	@if exist "$(OBJ_DIR)\Tests" rmdir /s /q "$(OBJ_DIR)\Tests"

# Object compilation rule matching engine .c.obj pattern
$(OBJ_DIR)/%.c.obj: %.c
	@echo Compiling $<...
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	@clang-cl $< $(COMPILER_FLAGS) /c /Fo:"$@" $(DEFINES) $(INCLUDE_FLAGS)

-include $(OBJ_FILES:.c.obj=.d)