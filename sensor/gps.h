#ifndef SENSOR_GPS_H
#define SENSOR_GPS_H

#include <core/types.h>

#define GPS_MAX_VISIBLE_SATS (12)

enum {
	GPS_FIX_NONE = 0,
	GPS_FIX_GPS_SPS,
	GPS_FIX_GPS_DGPS,
	GPS_FIX_GPS_PPS,
	GPS_FIX_RTK_FIXED,
	GPS_FIX_RTK_FLOAT,
	GPS_FIX_ESTIMATED,
	GPS_FIX_MANUAL,
	GPS_FIX_SIMULATOR,
	_GPS_FIX_COUNT,
};

struct gps_sat {
	int satid;
	int satelev;
	int satazim;
	int satsnr;
};

struct gps {
	bool_t fixvalid;
	int fixtype;
	int fixhour;
	int fixminute;
	int fixsecond;
	int fixmilisecond;

	bool_t satsvalid;
	int numvissats;
	struct gps_sat vissats[GPS_MAX_VISIBLE_SATS];
};

extern struct gps gps;

#endif /* !SENSOR_GPS_H */
