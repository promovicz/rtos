
#include "rtc.h"

#include <stdio.h>

struct rtc_regs {
	uint8_t ILR;
	uint8_t _pad0[3];
	uint16_t CTC;
	uint8_t _pad1[2];
	uint8_t CCR;
	uint8_t _pad2[3];
	uint8_t CIIR;
	uint8_t _pad3[3];
	uint8_t AMR;
	uint8_t _pad33[3];
	uint32_t CTIME0;
	uint32_t CTIME1;
	uint32_t CTIME2;

	uint8_t SEC;
	uint8_t _pad4[3];
	uint8_t MIN;
	uint8_t _pad5[3];
	uint8_t HOUR;
	uint8_t _pad6[3];
	uint8_t DOM;
	uint8_t _pad7[3];
	uint8_t DOW;
	uint8_t _pad8[3];
	uint16_t DOY;
	uint8_t _pad9[2];
	uint8_t MONTH;
	uint8_t _pad10[3];
	uint16_t YEAR;
	uint8_t _pad11[2];

	uint8_t _xpad0[32];

	uint8_t ALSEC;
	uint8_t _pad12[3];
	uint8_t ALMIN;
	uint8_t _pad13[3];
	uint8_t ALHOUR;
	uint8_t _pad14[3];
	uint8_t ALDOM;
	uint8_t _pad15[3];
	uint8_t ALDOW;
	uint8_t _pad16[3];
	uint16_t ALDOY;
	uint8_t _pad17[2];
	uint8_t ALMONTH;
	uint8_t _pad18[3];
	uint16_t ALYEAR;
	uint8_t _pad19[2];

	uint16_t PREINT;
	uint8_t _pad20[2];
	uint16_t PREFRAC;
	uint8_t _pad21[2];
};

enum ilr_bits {
	ILR_RTCCIF = (1<<0),
	ILR_RTCALF = (1<<1),
};

enum ccr_bits {
	CCR_CLKEN  = (1<<0),
	CCR_CTCRST = (1<<1),
	CCR_CLKSRC = (1<<4),
};

enum ciir_bits {
	CIIR_IMSEC  = (1<<0),
	CIIR_IMMIN  = (1<<1),
	CIIR_IMHOUR = (1<<2),
	CIIR_IMDOM  = (1<<3),
	CIIR_IMDOW  = (1<<4),
	CIIR_IMDOY  = (1<<5),
	CIIR_IMMON  = (1<<6),
	CIIR_IMYEAR = (1<<7),
};

enum amr_bits {
	AMR_AMRSEC  = (1<<0),
	AMR_AMRMIN  = (1<<1),
	AMR_AMRHOUR = (1<<2),
	AMR_AMRDOM  = (1<<3),
	AMR_AMRDOW  = (1<<4),
	AMR_AMRDOY  = (1<<5),
	AMR_AMRMON  = (1<<6),
	AMR_AMRYEAR = (1<<7),
};

const char const *dayname[] = {
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday",
};

const char const *monthname[] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

const char const *sourcename[] = {
	"invalid",
	"rtc",
	"user",
	"gps",
};


#define RTC_BASE (0xE0024000)
#define RTC ((volatile struct rtc_regs *)RTC_BASE)

struct rtc_time_t {
	rtc_hour_t hour;
	rtc_minute_t minute;
	rtc_second_t second;
};

struct rtc_date_t {
	rtc_year_t year;
	rtc_month_t month;
	rtc_dom_t dom;
};

bool_t rtc_running_at_init;

bool_t rtc_time_valid;
rtc_source_t rtc_time_source;

bool_t rtc_date_valid;
rtc_source_t rtc_date_source;

void rtc_init()
{
	if(RTC->CCR != (CCR_CLKEN|CCR_CLKSRC)) {
		RTC->CCR = CCR_CLKEN|CCR_CLKSRC;

		rtc_time_valid = BOOL_FALSE;
		rtc_time_source = RTC_SOURCE_RTC;
		rtc_date_valid = BOOL_FALSE;
		rtc_date_source = RTC_SOURCE_RTC;

		rtc_indicate_time(RTC_SOURCE_INVALID, 12, 0, 0);
		rtc_indicate_date(RTC_SOURCE_INVALID, 0, 1, 1);

		rtc_running_at_init = BOOL_FALSE;
	} else {
		bool_t date_seems_valid = (RTC->YEAR > 2000);

		rtc_time_valid = BOOL_TRUE;
		rtc_time_source = RTC_SOURCE_RTC;
		rtc_date_valid = date_seems_valid?BOOL_TRUE:BOOL_FALSE;
		rtc_date_source = date_seems_valid?RTC_SOURCE_RTC:RTC_SOURCE_INVALID;

		if(!date_seems_valid) {
			rtc_indicate_date(RTC_SOURCE_INVALID, 0, 1, 1);
		}

		rtc_running_at_init = BOOL_TRUE;
	}
}

void rtc_indicate_time(rtc_source_t source, rtc_hour_t h, rtc_minute_t m, rtc_second_t s)
{
	if(source >= rtc_time_source) {
		RTC->SEC = s;
		RTC->MIN = m;
		RTC->HOUR = h;
		if(source != RTC_SOURCE_INVALID) {
			rtc_time_valid = BOOL_TRUE;
		}
		rtc_time_source = source;
	}
}

rtc_dow_t rtc_compute_dow(rtc_year_t year, rtc_month_t month, rtc_dom_t dom)
{
	return 0;
}

rtc_doy_t rtc_compute_doy(rtc_year_t year, rtc_month_t month, rtc_dom_t dom)
{
	return 0;
}

void rtc_indicate_date(rtc_source_t source, rtc_year_t year, rtc_month_t month, rtc_dom_t dom)
{
	if(source >= rtc_date_source) {
		RTC->DOW = rtc_compute_dow(year, month, dom);
		RTC->DOY = rtc_compute_doy(year, month, dom);
		RTC->DOM = dom;
		RTC->MONTH = month;
		RTC->YEAR = year;
		if(source != RTC_SOURCE_INVALID) {
			rtc_date_valid = BOOL_TRUE;
		}
		rtc_date_source = source;
	}
}

void rtc_report(void)
{
	if(rtc_time_valid) {
		printf("wall time %02d:%02d:%02d source %s\n",
			   RTC->HOUR, RTC->MIN, RTC->SEC,
			   sourcename[rtc_time_source]);
		if(rtc_date_valid) {
			printf("wall date XXX source %s\n", sourcename[rtc_date_source]);
		} else {
			printf("wall date unknown\n");
		}
	} else {
		printf("wall time unknown\n");
	}

	printf("rtc was %s at init\n", rtc_running_at_init?"running":"started");
}
