.PHONY: build clean
.DEFAULT_GOAL := build


# Configuration variables.
CROSS_COMPILE ?=
EXTRA_LIBS ?=
GPP = $(CROSS_COMPILE)g++

COMPILE_FLAGS = -c -std=c++14 -Wall
DEBUG_FLAGS ?=
LINK_FLAGS =

INCLUDES  = -Iinclude/
INCLUDES += -Idependencies/json/src/
INCLUDES += -Idependencies/promclient-cpp/include/
INCLUDES += -Idependencies/promclient-cpp/features/onion/src

LIBS = -lusb-1.0 -lpthread $(EXTRA_LIBS)

OBJECTS = build/config.o \
					build/device-repo.o \
					build/log.o \
					build/main.o \
					build/models/pcsensor/temper1f.o \
					build/output.o \
					build/outputs/file.o \
					build/outputs/prometheus.o \
					build/sensor.o \
					build/usb-sensor.o


# Build dependencies.
ONION = dependencies/promclient-cpp/out/libonion_static.a
PROMCLIENT = dependencies/promclient-cpp/out/libpromclient.a
$(PROMCLIENT):
	$(MAKE) -C dependencies/promclient-cpp build FEAT_HTTP=1

$(ONION): $(PROMCLIENT)
	@echo 'Both files are build by promclient'


# Source to objects targets.
build/%.o: src/%.cpp
	mkdir -p build/models/pcsensor
	mkdir -p build/outputs
	$(GPP) $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(INCLUDES) $< -o $@


# Link binaries.
out/thermal: $(OBJECTS) $(PROMCLIENT) $(ONION)
	mkdir -p out/
	$(GPP) $(LINK_FLAGS) $(LIBS) -o $@ $^


# Main targets.
build: out/thermal

clean:
	rm -rf build/ out/

debug:
	$(MAKE) build DEBUG_FLAGS="-DDEBUG -ggdb"
