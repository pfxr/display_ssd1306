#Toolchain
CC=avr-gcc
CPP=avr-g++
CP=avr-objcopy

#Source files
SRC_FOLDER=src/
SRC_FILES=$(wildcard $(SRC_FOLDER)*.c)

#Outputs
BIN=bin
OBJECTS=$(foreach c_file, $(SRC_FILES), $(c_file:.c=.o))

MMCU=attiny85
FCPU=8000000UL
COM=/dev/ttyUSB0

default: image.h compile upload

clean:
	rm -rf src/*.o
	rm -rf bin

image.h: memory.py
	python3 memory.py

%.o : %.c
	$(CC) -Wall -mmcu=$(MMCU) -DF_CPU=$(FCPU) -g -c $< -o $@

compile: $(OBJECTS)
	mkdir -p bin
	${CPP} -L/usr/lib -o $(BIN)/display.elf $(OBJECTS) -mmcu=$(MMCU) -Wl,-Map=$(BIN)/display.map,--cref	
	${CP} -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(BIN)/display.elf $(BIN)/display.hex
	${CP} --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex $(BIN)/display.elf $(BIN)/display.eep
	${CP} --no-change-warnings -j .lock --change-section-lma .lock=0 -O ihex $(BIN)/display.elf $(BIN)/display.lock
	${CP} --no-change-warnings -j .signature --change-section-lma .signature=0 -O ihex $(BIN)/display.elf $(BIN)/display.sig
	${CP} --no-change-warnings -j .fuse --change-section-lma .fuse=0 -O ihex $(BIN)/display.elf $(BIN)/display.fuse

upload:
	avrdude -b 19200 -c avrisp -P $(COM) -p t85 -U flash:w:$(BIN)/display.hex

fuses:
	avrdude -b 19200 -c avrisp -P $(COM) -p t85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 
