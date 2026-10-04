DIR := $(subst /,\,${CURDIR})
BUILD_DIR := bin
OBJ_DIR := Object

ASSEMBLY := Fractal
EXTENSION := .dll

# Removed /LD from COMPILER_FLAGS (handled by /DLL in LINKER_FLAGS)
COMPILER_FLAGS := /Z7 /W4 /WX /TC -Werror=vla /std:clatest /clang:-MMD /clang:-MF /clang:"$@.d"
INCLUDE_FLAGS := /IEngine\src /I"$(VULKAN_SDK)\Include" /I"..\vcpkg_installed\x64-windows\include"

# Added /DLL right after /link
LINKER_FLAGS := /link /DEBUG /PDB:"$(BUILD_DIR)\$(ASSEMBLY).pdb" /DLL /LIBPATH:"$(VULKAN_SDK)\Lib" /LIBPATH:"..\vcpkg_installed\x64-windows\lib" user32.lib vulkan-1.lib /OUT:"$(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)" /IMPLIB:"$(BUILD_DIR)\$(ASSEMBLY).lib"
DEFINES := /D_DEBUG /DFEXPORT /D_CRT_SECURE_NO_WARNINGS

Wildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call Wildcard,$d/,$2))

SRC_FILES := $(call Wildcard,Engine/src/,*.c)
OBJ_FILES := $(SRC_FILES:%.c=$(OBJ_DIR)/%.c.obj)

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Scaffolding folder structure...
	-@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	-@if not exist "$(OBJ_DIR)\Engine\src" mkdir "$(OBJ_DIR)\Engine\src"
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
	@if exist "$(BUILD_DIR)\$(ASSEMBLY).lib" del /f /q "$(BUILD_DIR)\$(ASSEMBLY).lib"
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"

$(OBJ_DIR)/%.c.obj: %.c
	@echo Compiling $<...
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	@clang-cl $< $(COMPILER_FLAGS) /c /Fo:"$@" $(DEFINES) $(INCLUDE_FLAGS)

-include $(OBJ_FILES:.c.obj=.d)