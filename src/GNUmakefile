# This is the name that our final kernel executable will have.
# Change as needed.
KERNELDIR = kernel
SYSDIR = system
DRIVERSDIR = drivers
LIBDIR = lib
MEMDIR = mem

override KERNEL := $(KERNELDIR)/kernel.elf

# Convenience macro to reliably declare overridable command variables.
define DEFAULT_VAR =
    ifeq ($(origin $1), default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1), undefined)
        override $(1) := $(2)
    endif
endef

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
$(eval $(call DEFAULT_VAR,CC,clang))

# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".
$(eval $(call DEFAULT_VAR,LD,ld))

# User controllable CFLAGS.
CFLAGS ?= -Ofast -g -Wall -Wextra -pipe

# User controllable linker flags. We set none by default.
LDFLAGS ?=

INCLUDES :=                  \
    -I.                      \
    -I$(KERNELDIR)/          \
	-I$(MEMDIR)/			 \
	-I$(LIBDIR)/             \
	-I$(SYSDIR)/cpu/         \
	-I$(SYSDIR)/mem/         \
	-I$(DRIVERSDIR)/display/ \
	-I$(DRIVERSDIR)/keyboard/\

# Internal C flags that should not be changed by the user.
override INTERNALCFLAGS :=   \
	$(INCLUDES)          \
	-std=c11             \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-stack-check     \
	-fno-pic             \
	-fno-pie             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD				 

# Internal linker flags that should not be changed by the user.
override INTERNALLDFLAGS :=    \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static

# Use find to glob all *.c files in the directory and extract the object names.
override CFILES := $(shell find ./ -type f -name '*.c')
override ASMFILES := $(shell find ./ -type f -name '*.s')
override OBJ := $(CFILES:.c=.o) $(ASMFILES:.s=.o)
#override OBJ2 := $(ASMFILES:.s=.o)
override HEADER_DEPS := $(CFILES:.c=.d)

# Default target.
.PHONY: all
all: $(KERNEL)

limine.h:
	curl https://raw.githubusercontent.com/limine-bootloader/limine/trunk/limine.h -o $@

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@

# Compilation rules for *.c files.
-include $(HEADER_DEPS)
%.o: %.c limine.h
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

# Compilation rules for *.s files.
%.o: %.s
	nasm -F dwarf -g -felf64 $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)

.PHONY: distclean
distclean: clean
	rm -f limine.h