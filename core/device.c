
#include "device.h"

#include <string.h>
#include <stdio.h>

LLIST_HEAD(all_devices);

struct device *device_find(const char *name)
{
	struct device *dev;

	llist_for_each_entry(dev, &all_devices, list) {
		if(!strcmp(name, dev->name)) {
			return dev;
		}
	}

	return NULL;
}

void device_add(struct device *dev)
{
	llist_add_tail(&dev->list, &all_devices);
}

void device_report(struct device *dev)
{
	printf("dev %s\n", dev->name);
	if(dev->report_cb) {
		dev->report_cb(dev);
	}
}

void device_report_all(void)
{
	struct device *dev;

	llist_for_each_entry(dev, &all_devices, list) {
		device_report(dev);
	}
}
