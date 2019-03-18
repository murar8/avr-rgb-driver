DEVICE     = attiny85           # See avr-help for all possible devices
CLOCK      = 16000000           # 16Mhz
OBJECTS    = main.o             # Add more objects for each .c file here

UPLOAD = avrdude -P /dev/ttyUSB0 -c avrisp -b 19200 -p t85 -U flash:w:
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

flash:	all
	$(UPLOAD)main.hex

clean:
	rm -f main.hex main.elf $(OBJECTS)

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf
