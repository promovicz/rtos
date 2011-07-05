
#include <core/device.h>

#include <fat/sd_raw.h>

#include <stdio.h>

struct msd {
	struct device dev;
	bool_t avail;
	struct sd_raw_info nfo;
};

#define msd_from_dev(d) container_of(d, struct msd, dev);

static void msd_device_report(struct device *dev)
{
	struct msd *m = msd_from_dev(dev);

	if(!m->avail) {
		printf(" card not present\n");
		return;
	}

	printf(" card mfr %d oem %s product %s\n",
			m->nfo.manufacturer, &m->nfo.oem[0], &m->nfo.product[0]);

	printf("      rev %u.%u mfg %u/%u\n",
			m->nfo.revision >> 4, m->nfo.revision & 0xF,
			m->nfo.manufacturing_year + 2000, m->nfo.manufacturing_month); // XXX month offset?

	printf("      serial %u\n", m->nfo.serial);

	printf("      capacity %u\n", m->nfo.capacity);

	printf("      content is %s\n", m->nfo.flag_copy ? "copy" : "original");

	if(m->nfo.flag_write_protect) {
		printf("      permanently write protected\n");
	}

	if(m->nfo.flag_write_protect_temp) {
		printf("      temporarily write protected\n");
	}

	printf("      format is ");
	switch(m->nfo.format) {
	case SD_RAW_FORMAT_HARDDISK:
		printf("harddisk\n");
		break;
	case SD_RAW_FORMAT_SUPERFLOPPY:
		printf("superfloppy\n");
		break;
	case SD_RAW_FORMAT_UNIVERSAL:
		printf("universal\n");
		break;
	case SD_RAW_FORMAT_UNKNOWN:
	default:
		printf("unknown\n");
		break;
	}
}

struct msd msd = {
	.dev = {
			.name = "msd",
			.class = DEVICE_CLASS_BLOCK,
			.fops = NULL,
			.report_cb = &msd_device_report,
	}
};

void msd_poll(void)
{
	int res;
	bool_t avail = sd_raw_available();
	struct sd_raw_info newnfo;

	if(!avail) {
		goto unavailable;
	}

	res = sd_raw_get_info(&newnfo);

	if(!res) {
		goto unavailable;
	}

	res = memcmp(&msd.nfo, &newnfo, sizeof(struct sd_raw_info));

	if(res != 0) {
		// XXX card has changed
		memcpy(&msd.nfo, &newnfo, sizeof(struct sd_raw_info));
	}

	msd.avail = BOOL_TRUE;

	return;

unavailable:
	if(msd.avail) {
		// XXX card removed
	}

	msd.avail = BOOL_FALSE;
}

void msd_init(void)
{
	device_add(&msd.dev);

	msd_poll();
}
