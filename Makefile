SHELL := /bin/bash

ifeq ($(project),)
	project = $(shell basename $(PWD))
endif

BUILD_DIR := /tmp/build-$(project)
SRC = $(shell find $(project) -type f -name "*.ino")

ifeq ($(ARDUINO_LIBS),)
	ARDUINO_LIBS := .
endif

ARDUINO_FLAGS := -logger=machine -hardware $(ARDUINO_DIR)/hardware -tools $(ARDUINO_DIR)/tools-builder -tools $(ARDUINO_DIR)/hardware/tools/avr -built-in-libraries $(ARDUINO_DIR)/libraries -libraries $(ARDUINO_LIBS) -fqbn=arduino:avr:uno -ide-version=10609 -build-path $(BUILD_DIR) -warnings=default -prefs=build.warn_data_percentage=75 -verbose

ifeq ($(AVRDUDE_CONF),)
	AVRDUDE_CONF := $(ARDUINO_DIR)/hardware/tools/avr/etc/avrdude.conf
endif

ifeq ($(AVRDUDE_PORT),)
	AVRDUDE_PORT := /dev/cu.wchusbserial1410
endif

all:	dump-prefs	compile

dump-prefs:	test-env	$(BUILD_DIR)
	$(ARDUINO_DIR)/arduino-builder -dump-prefs $(ARDUINO_FLAGS) $(SRC)

compile:	test-env	$(BUILD_DIR)
	$(ARDUINO_DIR)/arduino-builder -compile $(ARDUINO_FLAGS) $(SRC)

upload:	test-env	test-build
	$(ARDUINO_DIR)/hardware/tools/avr/bin/avrdude -C $(AVRDUDE_CONF) -v -patmega328p -carduino -P$(AVRDUDE_PORT) -b115200 -D -Uflash:w:$(BUILD_DIR)/$(SRC).hex:i

upload-hex:	test-env	test-hex
	$(ARDUINO_DIR)/hardware/tools/avr/bin/avrdude -C $(AVRDUDE_CONF) -v -patmega328p -carduino -P$(AVRDUDE_PORT) -b115200 -D -Uflash:w:$(hex):i

test-env:
	@if [ -z "$(ARDUINO_DIR)" ]; then \
		echo "Please set ARDUINO_DIR" && \
		exit 1; \
	fi

test-build:
	@if [ ! -f "$(BUILD_DIR)/$(SRC).hex" ]; then \
		echo "$(BUILD_DIR)/$(SRC).hex does not exist" && \
		exit 1; \
	fi

test-hex:
	@if [ ! -f "$(hex)" ]; then \
		echo "$(hex) does not exist" && \
		exit 1; \
	fi

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -fr $(BUILD_DIR)

