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

void rtc_init();

void rtc_set_time(rtc_hour_t h, rtc_minute_t m, rtc_second_t s);

void rtc_set_date(rtc_year_t year, rtc_month_t month, rtc_dom_t dom);

#endif /* !LPC_RTC_H */
