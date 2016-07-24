SHELL := /bin/bash

ifeq ($(ARDUINO_LIBS),)
	ARDUINO_LIBS := .
endif

ifeq ($(project),)
	PROJECT_DIR = $(PWD)
else
	PROJECT_DIR = $(shell cd $(project); pwd)
endif

SRC = $(shell find $(PROJECT_DIR) -type f -name "*.ino" -print0 | xargs -0 -I {} echo "{}")
PROJECT_NAME = $(shell basename $(PROJECT_DIR))
BUILD_DIR = /tmp/build-$(PROJECT_NAME)

ifeq ($(hex),)
	hex := $(BUILD_DIR)/$(PROJECT_NAME).hex
endif

#
# require $ARDUINO_DIR
#
ARDUINO_FLAGS := -logger=machine -hardware $(ARDUINO_DIR)/hardware -tools $(ARDUINO_DIR)/tools-builder -tools $(ARDUINO_DIR)/hardware/tools/avr -built-in-libraries $(ARDUINO_DIR)/libraries -libraries $(ARDUINO_LIBS) -fqbn=arduino:avr:uno -ide-version=10609 -build-path $(BUILD_DIR) -warnings=default -prefs=build.warn_data_percentage=75 -verbose -prefs build.project_name=$(PROJECT_NAME)

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

upload:	test-env	test-hex
	$(ARDUINO_DIR)/hardware/tools/avr/bin/avrdude -C $(ARDUINO_CONF) -v -patmega328p -carduino -P$(ARDUINO_PORT) -b115200 -D -Uflash:w:$(hex):i

dump-prefs:	test-env	$(BUILD_DIR)
	$(ARDUINO_DIR)/arduino-builder -dump-prefs $(ARDUINO_FLAGS) $(SRC)

test-env:
	@echo ARDUINO_DIR: $(ARDUINO_DIR)
	@echo ARDUINO_LIBS: $(ARDUINO_LIBS)
	@echo ARDUINO_CONF: $(ARDUINO_CONF)
	@echo ARDUINO_PORT: $(ARDUINO_PORT)
	@echo project: $(project)
	@echo PROJECT_DIR: $(PROJECT_DIR)
	@echo PROJECT_NAME: $(PROJECT_NAME)
	@echo SRC: $(SRC)
	@echo BUILD_DIR: $(BUILD_DIR)

	@if [ -z "$(ARDUINO_DIR)" ]; then \
		echo "Please set ARDUINO_DIR" && \
		exit 1; \
	fi

test-hex:
	@if [ ! -f "$(hex)" ]; then \
		echo "Hex '$(hex)' does not exist" && \
		exit 1; \
	fi

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -fr $(BUILD_DIR)

