include ../Makefile.mk

.DEFAULT_GOAL := default
.PHONY: default

SRC_FONT := hankaku.txt
ASM_FONT := ../$(OS_BIN_DIR)/hankaku.S
OBJ_FONT := ../$(OS_BIN_DIR)/hankaku.o
MAKEFONT := ../$(TOOL_DIR)/makefont

default: font

font: $(ASM_FONT)
	$(AS) $(ASFLAGS) -o $(OBJ_FONT) $^

$(ASM_FONT): $(SRC_FONT)
	$(MAKEFONT) -n _hankaku -o $@ $^

