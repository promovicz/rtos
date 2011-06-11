#ifndef CORE_DEVICE_H
#define CORE_DEVICE_H

#include <core/common.h>

#include <core/file.h>

/* forward declaration */
struct device;

/* enum of device classes (adjust array in device.c!) */
typedef enum {
	DEVICE_CLASS_NONE,
	DEVICE_CLASS_CLOCK,
	DEVICE_CLASS_TIMER,
	DEVICE_CLASS_STREAM,
} device_class_t;

/* iteration callbacks */
typedef void (*device_cb_t) (struct device *dev, void *cookie);

/* callback to drivers for device status report */
typedef void (*device_report_cb_t) (struct device *dev);

/* device structure */
struct device {
	char *name;
	device_class_t class;

	device_report_cb_t report_cb;

	int uses;

	struct file_operations *fops;

	struct llist_head list;
};

/* use counting */
static void device_use(struct device *dev)
{
	dev->uses++;
}
static void device_unuse(struct device *dev)
{
	dev->uses--;
}

/* translate class enum to string */
const char *device_class_name(device_class_t class);

/* find device by its name */
struct device *device_by_name(const char *name);

/* call given callback for each class instance */
int device_foreach_of_class(device_class_t class, void *cookie, device_cb_t callback);

/* register given device */
void device_add(struct device *dev);

/* open given device */
int device_open(const char *name, int flags);

/* reporting functions */
void device_report(struct device *dev);
void device_report_all(void);

#endif /* !CORE_DEVICE_H */
