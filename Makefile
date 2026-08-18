MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc

# Keep -O0 for accurate GDB variable stepping & debugging
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -g3 -O0 -Wall -Wextra

TARGET = scheduler
SRC = main.c scheduler.c timer.c

all: $(TARGET).elf

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@

# Run simulation using simavr with gdbserver listening on port 1234
sim: $(TARGET).elf
	simavr -m $(MCU) -f $(F_CPU) -g $(TARGET).elf

# Connect avr-gdb to the running simavr session
gdb: $(TARGET).elf
	avr-gdb -ex "target remote localhost:1234" -ex "load" $(TARGET).elf

clean:
	rm -f $(TARGET).elf

.PHONY: all sim gdb clean
