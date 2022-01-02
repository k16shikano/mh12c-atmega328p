# WinAVR cross-compiler toolchain is used here
CC = avr-gcc
OBJCOPY = avr-objcopy

CFLAGS = -Wall -Os -mmcu=atmega328p
OBJFLAGS = -j .text -j .data -O ihex

SRC=main

all: $(SRC).hex 

# From .elf file to .hex
%.hex: %.elf
	$(OBJCOPY) $(OBJFLAGS) $< $@

# Main.elf requires additional objects to the firmware, not just main.o
$(SRC).elf: $(SRC).o
	$(CC) $(CFLAGS) $(SRC).o -o $@

# From C source to .o object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# From assembler source to .o object file
%.o: %.S
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

# for uncompatino
flush:
	sudo ~/src/avrdude-5.8/avrdude -c diecimila -C ~/src/avrdude-5.8/avrdude.conf -b 19200 -p atmega328p -e -U flash:w:$(SRC).hex -P ft0 -v -v -F
