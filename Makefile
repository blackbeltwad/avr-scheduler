MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

PORT = /dev/cu.usbmodem101
BAUD = 115200

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -g3 -O0 -Wall -Wextra

TARGET = scheduler
SRC = main.c scheduler.c timer.c context.S

all: $(TARGET).elf

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Upload to Arduino Uno
upload: $(TARGET).hex
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -b $(BAUD) -U flash:w:$(TARGET).hex

# Run simulation using simavr with gdbserver listening on port 1234
sim: $(TARGET).elf
	simavr -m $(MCU) -f $(F_CPU) -g $(TARGET).elf

# Connect avr-gdb to the running simavr session
gdb: $(TARGET).elf
	avr-gdb -ex "target remote localhost:1234" -ex "load" $(TARGET).elf

clean:
	rm -f $(TARGET).elf
