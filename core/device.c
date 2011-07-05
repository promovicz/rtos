
#include "device.h"

#include <string.h>
#include <stdio.h>

LLIST_HEAD(all_devices);

static char *device_class_names[] = {
	"none",
	"clock",
	"timer",
	"stream",
	"block",
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
	dev->uses = 0;

	llist_add_tail(&dev->list, &all_devices);
}

int device_open(const char *name, int flags)
{
	struct device *d;
	struct file *f;
	int res;

	d = device_by_name(name);
	if(!d) {
		return -ENOENT;
	}

	if(!d->fops) {
		return -EINVAL;
	}

	f = file_alloc();
	if(!f) {
		return -ENOMEM;
	}

	f->f_name = d->name;
	f->f_flags = flags;
	f->f_ops = d->fops;

	f->f_device = d;

	res = f->f_ops->fop_open(f);
	if(res < 0) {
		return res;
	}

	return fd_alloc(f);
}

void device_report(struct device *dev)
{
	printf("dev %s class %s uses %d\n", dev->name, device_class_name(dev->class), dev->uses);
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
