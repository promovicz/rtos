#ifndef CORE_DEVICE_H
#define CORE_DEVICE_H

#include <core/common.h>

#include <core/list.h>

typedef enum {
	DEVICE_CLASS_NONE,
} device_class_t;

typedef void (*device_report_cb_t) (struct device *dev);

struct device {
	char *name;
	device_class_t class;

	device_report_cb_t report_cb;

	int uses;
	struct llist_head list;
};

struct device *device_find(const char *name);

void device_add(struct device *dev);

void device_report(struct device *dev);

void device_report_all(void);

#endif /* !CORE_DEVICE_H */
