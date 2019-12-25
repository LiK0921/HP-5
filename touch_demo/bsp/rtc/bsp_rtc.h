#ifndef __BSP_RTC_H
#define __BSP_RTC_H

#include "imx6ul.h"

#define SECONDS_IN_A_DAY    (86400) /* 一天 86400 秒 */
#define SECONDS_IN_A_HOUR   (3600) /* 一个小时 3600 秒 */
#define SECONDS_IN_A_MINUTE (60) /* 一分钟 60 秒 */
#define DAYS_IN_A_YEAR      (365) /* 一年 365 天 */
#define YEAR_RANGE_START    (1970) /* 开始年份 1970 年 */
#define YEAR_RANGE_END      (2099) /* 结束年份 2099 年 */

typedef struct rtc_struct
{
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
}RTC_Struct;

void rtc_init(void);
void rtc_enable(void);
void rtc_disable(void);

u32 rtc_coverdate_to_seconds(RTC_Struct *datetime);
void rtc_convertseconds_to_datetime(u32 seconds, RTC_Struct *datetime);

u32 rtc_getseconds(void);

void rtc_setdatetime(RTC_Struct *datetime);
void rtc_getdatetime(RTC_Struct *datetime);

u8 rtc_isleapyear(u16 year);

#endif // !__BSP_RTC_H
