# =============================================================================
# File:     Makefile
# Project:  Shasta Firmware
# -----------------------------------------------------------------------------
# Brief:    This global makefile builds the default targets of all (configured)
#           project directories.
# =============================================================================

# Project Directory List where to build the default target
PROJ_DIRS := ${sort ${dir ${wildcard */}}}

# Directories to filter out for build all
PROJ_DRIS_FILTER := version/

# Filtered list of directories which should be build or cleared
PROJ_DIRS_FILTERED := $(filter-out $(PROJ_DRIS_FILTER), $(PROJ_DIRS))

# All patch projects depend on the ROM- or RAM- image as predecessor.
ROM_PROJ_DIR := rom_image/

RAM_PROJ_DIR := ram_image/

PATCH_PROJ_DIRS := $(filter-out $(ROM_PROJ_DIR) $(RAM_PROJ_DIR), $(PROJ_DIRS_FILTERED))
$(PATCH_PROJ_DIRS:%=build-%): $(ROM_PROJ_DIR:%=build-%) $(RAM_PROJ_DIR:%=build-%)

# Generate build-rules for each project directory
BUILDDIRS = $(PROJ_DIRS_FILTERED:%=build-%)

# Generate clean-rules for each project directory
CLEANDIRS = $(PROJ_DIRS_FILTERED:%=clean-%)

# Default target: Print out help
help:
	@echo "Supported targets:"
	@echo ""
	@echo "   all      Build all subtargets:"
	@echo "            '$(PROJ_DIRS_FILTERED)'"
	@echo ""
	@echo "   clean    Delete all build targets"
	@echo ""

# Build default targets of all projects from the list $(PROJ_DIRS)
all: $(BUILDDIRS)
$(BUILDDIRS):
	@$(MAKE) -C $(@:build-%=%)

# Clean up in all projects from the list $(PROJ_DIRS_FILTERED)
clean: $(CLEANDIRS)
$(CLEANDIRS):
	@$(MAKE) -C $(@:clean-%=%) clean

.PHONY: subdirs $(PROJ_DIRS_FILTERED)
.PHONY: $(BUILDDIRS)
.PHONY: $(CLEANDIRS)
.PHONY: all clean
