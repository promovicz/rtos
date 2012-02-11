#ifndef LPC_RTC_H
#define LPC_RTC_H

#include <core/types.h>

typedef uint8_t  rtc_second_t;
typedef uint8_t  rtc_minute_t;
typedef uint8_t  rtc_hour_t;
typedef uint8_t  rtc_dom_t;
typedef uint8_t  rtc_dow_t;
typedef uint16_t rtc_doy_t;
typedef uint8_t  rtc_month_t;
typedef uint16_t rtc_year_t;

typedef enum {
	RTC_SOURCE_INVALID,
	RTC_SOURCE_RTC,
	RTC_SOURCE_USER,
	RTC_SOURCE_GPS,
} rtc_source_t;

void rtc_init(void);

void rtc_indicate_time(rtc_source_t source, rtc_hour_t h, rtc_minute_t m, rtc_second_t s);

void rtc_indicate_date(rtc_source_t source, rtc_year_t year, rtc_month_t month, rtc_dom_t dom);

void rtc_report(void);

#endif /* !LPC_RTC_H */
