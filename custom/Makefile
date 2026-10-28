CC         ?= gcc
CFLAGS     ?= -O2 -Wpedantic -Wextra -Wformat -Wall -march=native -mtune=native
BUILD_DIR  ?= builds

.DEFAULT_GOAL:= all
all: blink check_battery chime live-wallpaper


blink: blink.c
	$(CC) -o $(BUILD_DIR)/$@ $^ $(CFLAGS) -lglfw

check_battery: check_battery.c
	$(CC) -o $(BUILD_DIR)/$@ $^ $(CFLAGS) `pkg-config --cflags --libs libnotify` -DBEEP -lportaudio

chime: chime.c
	$(CC) -o $(BUILD_DIR)/$@ $^ $(CFLAGS) -lespeak-ng

live-wallpaper: live-wallpaper.c
	$(CC) -o $(BUILD_DIR)/$@ $^ $(CFLAGS) -lX11 -lGL -lm
  
