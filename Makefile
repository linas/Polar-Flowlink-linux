all: polar

CC       ?= gcc
CFLAGS   ?= -Wall -g

CXX      ?= g++
CXXFLAGS ?= -Wall -g

COBJS     = hidapi/linux/hid-libusb.o src/protocol.o src/parse_data.o
CPPOBJS   = 
OBJS      = $(COBJS) $(CPPOBJS)
LIBS      = `pkg-config libusb-1.0 libudev --libs` -lpq -lpthread
INCLUDES ?= `pkg-config libusb-1.0 --cflags` -Ihidapi/hidapi


polar-rs300x: $(OBJS) src/polar-rs300x.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o ./bin/polar-rs300x

polar-ft7: $(OBJS) src/polar-ft7.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o ./bin/polar-ft7

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(CPPOBJS): %.o: %.c
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJS) ./bin/polar-rs300x ./bin/polar-ft7

.PHONY: clean
