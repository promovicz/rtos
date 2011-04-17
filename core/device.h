#ifndef CORE_DEVICE_H
#define CORE_DEVICE_H

#include <core/common.h>

#include <core/list.h>

struct device;

typedef enum {
	DEVICE_CLASS_NONE,
	DEVICE_CLASS_CLOCK,
	DEVICE_CLASS_STREAM,
} device_class_t;

typedef void (*device_cb_t) (struct device *dev, void *cookie);

typedef void (*device_report_cb_t) (struct device *dev);

struct device {
	char *name;
	device_class_t class;

	device_report_cb_t report_cb;

	int uses;
	struct llist_head list;
};

const char *device_class_name(device_class_t class);

struct device *device_by_name(const char *name);

int device_foreach_of_class(device_class_t class, void *cookie, device_cb_t callback);

void device_add(struct device *dev);

void device_report(struct device *dev);

void device_report_all(void);

#endif /* !CORE_DEVICE_H */
