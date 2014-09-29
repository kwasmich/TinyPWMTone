MCU=attiny13a
MCU_DUDE=attiny13
OBJCOPY=/opt/local/bin/avr-objcopy
CC=/opt/local/bin/avr-gcc
AVRSIZE=/opt/local/bin/avr-size
CFLAGS=-Wall -Os -mmcu=$(MCU)
LDFLAGS=
SOURCES=main.c
OBJECTS=$(SOURCES:%.c=%.o)
EXECUTABLE=TinyPWMTone

all: $(SOURCES) $(EXECUTABLE).hex

$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(EXECUTABLE).hex: $(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@
	$(AVRSIZE) $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	$(CC) $(CFLAGS) -S -o main.s -c $<

flash: $(EXECUTABLE).hex
	avrdude -c avrispmkII -P usb -p $(MCU_DUDE) -U flash:w:$(EXECUTABLE).hex
#	avrdude -c avrispmkII -P usb -p attiny13 -U lfuse:w:0x6A:m

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE).elf
	rm -f $(EXECUTABLE).hex
	rm -f *.s
	rm -f a.out
