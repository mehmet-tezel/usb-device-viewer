#include "usbdevice.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct Device CreateEmptyDevice() {
	struct Device my_device;

	my_device.queue = -1;
	my_device.vendor_id = 0000;
	my_device.device_id = 0000;
	my_device.address = '\0';
	my_device.port_number = '\0';
	my_device.bus_number = '\0';
	my_device.speed = "Unknown";

	strcpy(my_device.serial, "Unknown");
	strcpy(my_device.manufacturer, "Unknown");
	strcpy(my_device.product, "Unknown");

	return my_device;
}

struct Device* GetDevices(int rc, libusb_device** list) {

	struct libusb_device_descriptor desc = { 0 };

	libusb_context* context = NULL;
	libusb_device* device = NULL;
	libusb_device_handle* handle = NULL;

	ssize_t count = 0, idx;
	uint8_t string[256], devicePath[8];

	const char* deviceSpeed;

	count = libusb_get_device_list(context, &list);

	struct Device* devices = (struct Device*)malloc(count * sizeof(struct Device));

	for (idx = 0; idx < count; ++idx) {

		devices[idx] = CreateEmptyDevice();

		device = list[idx];

		libusb_open(device, &handle);

		rc = libusb_get_device_descriptor(device, &desc);

		devices[idx].queue = idx;
		devices[idx].bus_number = libusb_get_bus_number(device);
		devices[idx].address = libusb_get_device_address(device);
		devices[idx].vendor_id = desc.idVendor;
		devices[idx].device_id = desc.idProduct;

		if (handle) {

			if (desc.iSerialNumber) {
				rc = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
				if (rc > 0)
					strcpy(devices[idx].serial, (const char*)string);
			}

			if (desc.iManufacturer) {
				rc = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
				if (rc > 0)
					strcpy(devices[idx].manufacturer, (const char*)string);
			}

			if (desc.iProduct) {
				rc = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
				if (rc > 0)
					strcpy(devices[idx].product, (const char*)string);
			}
		}

		switch (libusb_get_device_speed(device)) {
		case LIBUSB_SPEED_LOW:			deviceSpeed = "1.5M";	 break;
		case LIBUSB_SPEED_FULL:			deviceSpeed = "12M";	 break;
		case LIBUSB_SPEED_HIGH:			deviceSpeed = "480M";	 break;
		case LIBUSB_SPEED_SUPER:		deviceSpeed = "5G";		 break;
		case LIBUSB_SPEED_SUPER_PLUS:	deviceSpeed = "10G";	 break;
		default:						deviceSpeed = "Unknown"; break;
		}

		devices[idx].speed = deviceSpeed;

		rc = libusb_get_port_numbers(device, devicePath, sizeof(devicePath));
		if (rc > 0)
			devices[idx].port_number = devicePath[0];
	}

	context = NULL;
	device = NULL;
	handle = NULL;

	return devices;
}

void PrintDevice(struct Device device) {
	printf("-[%d] [Bus: %d \| Device: %d]", device.queue, device.bus_number, device.address);
	if (device.port_number != '\0')
		printf(" COM Port Number: COM%d\n", device.port_number);
	else
		printf(" COM Port Number: %c\n", device.port_number);
	printf("Vendor ID: %04X\nDevice ID: %04X\n", device.vendor_id, device.device_id);
	printf("Serial: %s\n", device.serial);
	printf("Manufacturer: %s\n", device.manufacturer);
	printf("Product: %s\n", device.product);
	printf("Device Speed: %s\n\n", device.speed);
}