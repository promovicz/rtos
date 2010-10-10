
#include <core/types.h>

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

char *dayname[] = {
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday",
};

char *monthname[] = {
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


#define RTC_BASE (0xE0024000)
#define RTC ((struct rtc_regs *)RTC_BASE)

typedef uint8_t  rtc_second_t;
typedef uint8_t  rtc_minute_t;
typedef uint8_t  rtc_hour_t;
typedef uint8_t  rtc_dom_t;
typedef uint8_t  rtc_dow_t;
typedef uint16_t rtc_doy_t;
typedef uint8_t  rtc_month_t;
typedef uint16_t rtc_year_t;

void rtc_init()
{
}
