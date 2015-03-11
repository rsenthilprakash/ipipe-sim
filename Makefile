CC = gcc
CFLAGS = -Wall
PNG_CFLAGS = `pkg-config --cflags libpng`
PNG_LIBS = `pkg-config --libs libpng`

SOURCES = png_utils.c \
          demosaic_test.c \
          demosaic.c \
          rgb2rgb_test.c \
          rgb2rgb.c \

TARGETS = demosaic_test \
          rgb2rgb_test \

.PHONY: all
all: $(TARGETS)

demosaic_test: demosaic_test.o demosaic.o png_utils.o
	$(CC) $^ $(PNG_LIBS) $(LIBS) -o $@

rgb2rgb_test: rgb2rgb_test.o rgb2rgb.o png_utils.o
	$(CC) $^ $(PNG_LIBS) $(LIBS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $(PNG_CFLAGS) $< -o $@

depend: .depend

.depend: $(SOURCES)
	rm -f ./.depend
	$(CC) $(CFLAGS) $(PNG_CFLAGS) -MM $^ > ./.depend;

include .depend

.PHONY: clean
clean:
	rm -rf $(TARGETS)
	rm -rf *.o
	rm -rf ./.depend
