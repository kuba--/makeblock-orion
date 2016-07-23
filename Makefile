SHELL := /bin/bash

ifeq ($(project),)
	project = $(shell basename $(PWD))
endif

BUILD_DIR = /tmp/build-$(project)
SRC = $(shell find $(project) -type f -name "*.ino" -print0 | xargs -0 -I {} echo "{}")

ifeq ($(ARDUINO_LIBS),)
	ARDUINO_LIBS := .
endif

#
# require $ARDUINO_DIR
#
ARDUINO_FLAGS := -logger=machine -hardware $(ARDUINO_DIR)/hardware -tools $(ARDUINO_DIR)/tools-builder -tools $(ARDUINO_DIR)/hardware/tools/avr -built-in-libraries $(ARDUINO_DIR)/libraries -libraries $(ARDUINO_LIBS) -fqbn=arduino:avr:uno -ide-version=10609 -build-path $(BUILD_DIR) -warnings=default -prefs=build.warn_data_percentage=75 -verbose

ifneq ($(debug),)
	ARDUINO_FLAGS += -debug-level=$(debug) -prefs compiler.cpp.extra_flags=-DDEBUG
endif

ifeq ($(ARDUINO_CONF),)
	ARDUINO_CONF := $(ARDUINO_DIR)/hardware/tools/avr/etc/avrdude.conf
endif

ifeq ($(ARDUINO_PORT),)
	ARDUINO_PORT := /dev/cu.wchusbserial1410
endif

all:	compile	upload

compile:	test-env	$(BUILD_DIR)
	$(ARDUINO_DIR)/arduino-builder -compile $(ARDUINO_FLAGS) $(SRC)

upload:	test-env	test-build
	$(ARDUINO_DIR)/hardware/tools/avr/bin/avrdude -C $(ARDUINO_CONF) -v -patmega328p -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$(BUILD_DIR)/$(SRC).hex:i

upload-hex:	test-env	test-hex
	$(ARDUINO_DIR)/hardware/tools/avr/bin/avrdude -C $(ARDUINO_CONF) -v -patmega328p -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$(hex):i

dump-prefs:	test-env	$(BUILD_DIR)
	$(ARDUINO_DIR)/arduino-builder -dump-prefs $(ARDUINO_FLAGS) $(SRC)

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

