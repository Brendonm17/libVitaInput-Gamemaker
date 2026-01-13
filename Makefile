TARGET = vita_input
OBJS   = main.o

CC      = arm-vita-eabi-gcc
STRIP   = arm-vita-eabi-strip

# CFLAGS: Essential for Vita hardware
CFLAGS  = -O2 -Wall -fPIC -marm -march=armv7-a -mfloat-abi=hard -fno-stack-protector -fno-builtin

# LDFLAGS: 
# 1. -Wl,-q: Keeps the relocation table
# 2. -nostartfiles: Prevents the "main" and "crt0" errors
# 3. -nodefaultlibs: Prevents pulling in the broken static libc.a
LDFLAGS = -Wl,-q -nostartfiles -nodefaultlibs

# Library list
LIBS    = -lSceCtrl_stub -lSceTouch_stub -lSceMotion_stub -lScePower_stub -lSceLibc_stub -lSceLibKernel_stub -lgcc

all: $(TARGET).suprx

$(TARGET).suprx: $(TARGET).velf
	vita-make-fself -s $< $@

$(TARGET).velf: $(TARGET).elf
	vita-elf-create $< $@ exports.yml

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET).suprx $(TARGET).velf $(TARGET).elf $(OBJS)