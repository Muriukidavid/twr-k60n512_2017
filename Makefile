#
# File: Makefile
#
# Copyright (c) 2015 David Muriuki
# see the LICENCE file
#

# toolchain specific
TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
CXX = $(TOOLCHAIN)g++
AS = $(TOOLCHAIN)gcc -x assembler-with-cpp
LD = $(TOOLCHAIN)gcc
OBJCP = $(TOOLCHAIN)objcopy
AR = $(TOOLCHAIN)ar
GDB = $(TOOLCHAIN)gdb
GDBSERVER = openocd_osbdm -d 0 -c "interface osbdm" -f /usr/local/share/openocd/scripts/board/twr-k60n512.cfg
OPENOCDPID = $(shell pgrep openocd)

# application specific
CPU := cortex-m4
INSTRUCTION_MODE := thumb
TARGET := k60_demo
TARGET_EXT := elf
LD_SCRIPT := k60n512_flash.ld

LIBS :=

# directories
INC_DIRS = .
SRC_DIRS := . 
OUT_DIR := out
#INC_DIRS_F = -I. $(patsubst %, -I%, $(INC_DIRS))

# add traling / if not empty
ifeq ($(strip $(OUT_DIR)), )
	OBJ_FOLDER =
else
	OBJ_FOLDER = $(strip $(OUT_DIR))/
endif

COMPILER_OPTIONS = -O3 -ggdb -Wall -fno-strict-aliasing -fmessage-length=0 -fno-builtin -m$(INSTRUCTION_MODE) -mcpu=$(CPU) -MD -MP

DEPEND_OPTS = -MF $(OBJ_FOLDER)$(@F:.o=.d)

# Flags
#CFLAGS = $(COMPILER_OPTIONS) $(DEPEND_OPTS) $(INC_DIRS_F) -c
CFLAGS = $(COMPILER_OPTIONS) $(DEPEND_OPTS) $(INC_DIRS_F) -c
CXXFLAGS = $(COMPILER_OPTIONS) $(INC_DIRS_F) -c
ASFLAGS = $(COMPILER_OPTIONS) $(INC_DIRS_F) -c
OBJCPFLAGS = -O ihex
ARFLAGS = cr

# Linker options
LD_OPTIONS = -Wl,-Map=$(OBJ_FOLDER)$(TARGET).map $(COMPILER_OPTIONS) -L ../ -T $(LD_SCRIPT) $(INC_DIRS_F)
LD_OPTIONS += -nostartfiles

RM := rm -rf

USER_OBJS :=
C_SRCS :=
S_SRCS :=
C_OBJS :=
S_OBJS :=

# All source/object files inside SRC_DIRS
C_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
C_OBJS := $(patsubst %.c,$(OBJ_FOLDER)%.o,$(notdir $(C_SRCS)))
S_SRCS := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))
S_OBJS := $(patsubst %.c,$(OBJ_FOLDER)%.o,$(notdir $(S_SRCS)))

VPATH := $(SRC_DIRS)

$(OBJ_FOLDER)%.o : %.c
	@echo 'Building file: $(@F)'
	@echo '--------------------'
	@echo 'Invoking: MCU C Compiler'
	$(CC) $(CFLAGS) $< -o $@
	@echo 'Finished building: $(@F)'
	@echo ' '

$(OBJ_FOLDER)%.o : %.s
	@echo 'Building file: $(@F)'
	@echo '--------------------'
	@echo 'Invoking: MCU Assembler'
	$(AS) $(ASFLAGS) $< -o $@
	@echo 'Finished building: $(@F)'
	@echo ' '
# TODO: generate assembly lisiting
debug: all
ifeq ($(OPENOCDPID), ) # start openocd if not already running
	#$(GDBSERVER) &
	@echo "error: openocd server not running"
	exit()
endif
	$(GDB) -q -x .gdbcmds 
	 
all: clean create_outputdir $(OBJ_FOLDER)$(TARGET).$(TARGET_EXT) print_size

create_outputdir:
	$(shell mkdir $(OBJ_FOLDER) 2>/dev/null)

# Tool invocations
$(OBJ_FOLDER)$(TARGET).$(TARGET_EXT): $(C_OBJS) $(S_OBJS)
	@echo 'Building target: $@'
	@echo '-------------------'
	@echo 'Invoking: MCU Linker'
	$(LD) $(LD_OPTIONS) $(C_OBJS) $(S_OBJS) $(USER_OBJS) $(LIBS) -o $(OBJ_FOLDER)$(TARGET).$(TARGET_EXT)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	@echo 'Removing entire out directory'
	@echo '-----------------------------'
	$(RM) $(TARGET).$(TARGET_EXT) $(TARGET).bin $(TARGET).map $(OBJ_FOLDER)*.* $(OBJ_FOLDER)
	@echo ' '

print_size:
	@echo 'Printing size'
	@echo '-------------'
	arm-none-eabi-size --totals $(OBJ_FOLDER)$(TARGET).$(TARGET_EXT); arm-none-eabi-objcopy -O binary $(OBJ_FOLDER)$(TARGET).$(TARGET_EXT) $(OBJ_FOLDER)$(TARGET).bin ;
	@echo ' '

.PHONY: all clean dependents print_size
