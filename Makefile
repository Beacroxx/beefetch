CC = clang
CFLAGS = -Wall -O3
STRIP = strip

all:
	$(CC) $(CFLAGS) -o beefetch main.c
	$(STRIP) beefetch

clean:
	rm -f beefetch

install:
	install -m 755 beefetch /usr/local/bin	

.PHONY: all clean