MCU = atmega328p
CC = avr-gcc

CFLAGS = -mmcu=$(MCU) -g -O0 -Wall -Wextra

TARGET = scheduler
SRC = main.c scheduler.c timer.c

all: $(TARGET).elf

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@

clean:
	rm -f $(TARGET).elf
