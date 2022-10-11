PROJECT := usb_device_viewer
OBJS := main.cpp

SDL_INCLUDE_DIR := include/SDL2
SDL_LIB_DIR := lib/x64/SDL2

LIBUSB_INCLUDE_DIR := include/libusb
LIBUSB_LIB_DIR := lib/x64/libusb

IMGUI_DIR := include/imgui 

OBJS += include/imgui/imgui.cpp include/imgui/imgui_demo.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp
OBJS += include/imgui/backends/imgui_impl_sdl.cpp include/imgui/backends/imgui_impl_sdlrenderer.cpp
OBJS += usbdevice.c

INCLUDE_DIR := -I$(SDL_INCLUDE_DIR) -I$(LIBUSB_INCLUDE_DIR)
LIB_DIR := -L$(SDL_LIB_DIR) -L$(LIBUSB_LIB_DIR)

CC = g++

COMPILER_FLAGS := -w -std=c99 -std=c++14 -pedantic

LINKER_FLAGS := -lSDL2main -lSDL2 -lSDL2_image -l"libusb-1.0" -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32

XLINKER_FLAGS := -Xlinker /subsystem:console -Xlinker /NODEFAULTLIB:MSVCRT

OBJ_NAME := main.exe

$(PROJECT): $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(INCLUDE_DIR) $(LIB_DIR) $(LINKER_FLAGS) $(XLINKER_FLAGS) -o $(OBJ_NAME)

all: $(OBJS)

clean:
	rm -f $(OBJS) $(OBJ_NAME)

.PHONY: all clean