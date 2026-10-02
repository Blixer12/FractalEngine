DIR := $(subst /,\,${CURDIR})
BUILD_DIR := bin
OBJ_DIR := Object

ASSEMBLY := FractalEngine
EXTENSION := .exe

COMPILER_FLAGS := /Zi /W4 /WX /TC /std:clatest
INCLUDE_FLAGS := /ILauncher\src /IEngine\src /I"$(VULKAN_SDK)\Include"
LINKER_FLAGS := /link /LIBPATH:"$(BUILD_DIR)" Fractal.lib user32.lib /OUT:"$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)"
DEFINES := /D_DEBUG /DFIMPORT /D_CRT_SECURE_NO_WARNINGS

Wildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call Wildcard,$d/,$2))

SRC_FILES := $(call Wildcard,Launcher/src/,*.c)
OBJ_FILES := $(SRC_FILES:%.c=$(OBJ_DIR)/%.c.obj)

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Scaffolding folder structure...
	-@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	-@if not exist "$(OBJ_DIR)\Launcher\src" mkdir "$(OBJ_DIR)\Launcher\src"
	@echo Done.

.PHONY: compile
compile: $(OBJ_FILES)
	@echo Compiling done.

.PHONY: link
link: scaffold compile
	@echo Linking $(ASSEMBLY)...
	@clang-cl $(OBJ_FILES) $(COMPILER_FLAGS) $(DEFINES) $(INCLUDE_FLAGS) $(LINKER_FLAGS)

.PHONY: clean
clean:
	@if exist "$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)" del /f /q "$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)"
	@if exist "$(OBJ_DIR)\Launcher" rmdir /s /q "$(OBJ_DIR)\Launcher"

$(OBJ_DIR)/%.c.obj: %.c
	@echo Compiling $<...
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	@clang-cl $< $(COMPILER_FLAGS) /c /Fo:"$@" $(DEFINES) $(INCLUDE_FLAGS)