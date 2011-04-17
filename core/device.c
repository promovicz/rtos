
#include "device.h"

#include <string.h>
#include <stdio.h>

LLIST_HEAD(all_devices);

static char *device_class_names[] = {
	"none",
	"clock",
	"stream"
};

const char *device_class_name(device_class_t class)
{
	return device_class_names[class];
}

struct device *device_by_name(const char *name)
{
	struct device *dev;

	llist_for_each_entry(dev, &all_devices, list) {
		if(!strcmp(name, dev->name)) {
			return dev;
		}
	}

	return NULL;
}

int device_foreach_of_class(device_class_t class, void *cookie, device_cb_t callback)
{
	int i = 0;
	struct device *dev;

	llist_for_each_entry(dev, &all_devices, list) {
		if(dev->class == class) {
			callback(dev, cookie);
			i++;
		}
	}

	return i;
}

void device_add(struct device *dev)
{
	llist_add_tail(&dev->list, &all_devices);
}

void device_report(struct device *dev)
{
	printf("dev %s class %s\n", dev->name, device_class_name(dev->class));
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
