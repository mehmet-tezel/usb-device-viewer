#pragma once

#include "include/libusb/libusb.h"

struct Device {

	short queue;

	uint16_t vendor_id;
	uint16_t device_id;

	uint8_t address;
	uint8_t port_number;
	uint8_t bus_number;

	const char* speed;

	char serial[128];
	char manufacturer[128];
	char product[128];
};

struct Device CreateEmptyDevice();
struct Device* GetDevices(int rc, libusb_device** list);

void PrintDevice(struct Device device);