#include <stdio.h>

#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_image.h"

#include "include/imgui/imgui.h"
#include "include/imgui/backends/imgui_impl_sdl.h"
#include "include/imgui/backends/imgui_impl_sdlrenderer.h"
#include "imgui_colors.h"

#include "usbdevice.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#define test 2

// Main code
int main(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("USB Device Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 435, 195, window_flags);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    libusb_context* context = NULL;
	libusb_device** list = NULL;

	int rc = 0;
	rc = libusb_init(&context);

	struct Device* devices = GetDevices(rc, list);
	int count = libusb_get_device_list(context, &list);

    char* c_devices[count];
    for (int i = 0; i < count; ++i)
        c_devices[i] = (char*)malloc(sizeof(char) * 9);
    int current_device = 0;

	for (int i = 0; i < count; ++i) {
        sprintf(c_devices[i], "[%d] %04X:%04X", devices[i].queue, devices[i].vendor_id, devices[i].device_id);
    }

    //0x2f3238
    ImVec4 clear_color = ImVec4(0.21f, 0.22f, 0.23f, 1.00f);
    bool im_mainWindow = true;

    imgui_set_color();

    SDL_Event event;
    bool quit = false;
    while (!quit)
    {   
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                quit = true;
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Main Window
        {
            ImGui::Begin("USB Device Viewer", &im_mainWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            /* i dont like this 
            if (!ImGui::Combo("Device List", &current_device, c_devices, IM_ARRAYSIZE(c_devices))) {
                ImGui::Text("Speed: %s", devices[current_device].speed);
            }
            */

           ImGui::Combo("Device List", &current_device, c_devices, IM_ARRAYSIZE(c_devices));

            for (int i = 0; i < count; ++i) {
                if (current_device == i) {
                    ImGui::TextColored(ImVec4(0, 255, 0, 255), "[Bus: %d | Device: %d]", devices[i].bus_number, devices[i].address);
                    ImGui::SameLine();
                   
                    if (devices[i].port_number != '\0') {
                        ImGui::TextColored(ImVec4(255, 255, 0, 255), "COM Port: %d", devices[i].port_number);
                        ImGui::NewLine();
                    }
                    else {
                        ImGui::TextColored(ImVec4(255, 255, 0, 255), "COM Port: %c", devices[i].port_number);
                        ImGui::NewLine();
                    }
                    
                    ImGui::Text("Vendor ID: %04X", devices[i].vendor_id);
                    ImGui::Text("Device ID: %04X", devices[i].device_id);
                    ImGui::Text("Device Speed: %s", devices[i].speed);
                    ImGui::NewLine();
                    ImGui::Text("Product: %s", devices[i].product);
                    ImGui::Text("Serial: %s", devices[i].serial);
                    ImGui::Text("Manufacturer: %s", devices[i].manufacturer);
                }
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(renderer);
    }

    // Cleanup
	libusb_free_device_list(list, 1);
    for (int i = 0; i < count; ++i)
        free(c_devices[i]);

	list = NULL;
	context = NULL;

	libusb_exit(NULL);

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
