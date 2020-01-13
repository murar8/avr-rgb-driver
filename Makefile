DEVICE     = attiny85
CLOCK      = 16000000
PROGRAMMER = avrisp
PORT	   = /dev/ttyUSB0

CC    		= avr-gcc 
CFLAGS 		= -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -Wno-main
HEX 		= avr-objcopy -j .text -j .data -O ihex
UPLOAD	    = avrdude -P $(PORT) -c $(PROGRAMMER) -p $(DEVICE) -b 19200 -U flash:w:

TARGET		= main
SRCDIR    	= src
OBJDIR		= obj
BINDIR		= bin

sources = $(wildcard src/*.c)
objects = $(patsubst src/%.c, obj/%.o, $(sources))

default: upload

obj/%.o: src/%.c
	$(CC) -c -o $@ $^ $(CFLAGS)

bin/$(TARGET).elf: $(objects)
	$(CC) -o $@ $^ $(CFLAGS)

bin/$(TARGET).hex: bin/$(TARGET).elf
	$(HEX) $^ $@

upload: bin/$(TARGET).hex
	$(UPLOAD)$^

clean:
	rm -f bin/**
	rm -f obj/**