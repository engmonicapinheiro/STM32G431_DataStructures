#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>
#include "stm32g431xx.h"


/* ── RTC->CR control bits (bit positions unchanged from STM32F4) ─────────── */
#define CR_ALRAIE       (1U << 12)  /*!< Alarm A interrupt enable             */
#define CR_TSIE         (1U << 15)  /*!< Timestamp interrupt enable           */
#define CR_TSE          (1U << 11)  /*!< Timestamp enable                     */

/*
 * ── RTC->SR status flags (read-only, replaces ISR flags from STM32F4) ──────
 *
 *   STM32F4 ISR  →  STM32G431 SR
 *   bit 8  ALRAF →  bit 0  ALRAF  (RTC_SR_ALRAF)
 *   bit 11 TSF   →  bit 3  TSF    (RTC_SR_TSF)
 *
 *   Read with :  RTC->SR & SR_ALRAF
 */
#define SR_ALRAF        (1U << 0)   /*!< Alarm A flag  (F4: ISR bit 8)        */
#define SR_TSF          (1U << 3)   /*!< Timestamp flag (F4: ISR bit 11)      */

/*
 * ── RTC->SCR status-clear bits (write 1 to clear the corresponding SR flag) ─
 *
 *   Replaces the STM32F4 "RTC->ISR &= ~flag" pattern.
 *   Write with:  RTC->SCR = SCR_CALRAF;
 */
#define SCR_CALRAF      (1U << 0)   /*!< Clear Alarm A flag                   */
#define SCR_CTSF        (1U << 3)   /*!< Clear timestamp flag                 */

/*
 * ── RTC->ICSR bits (replaces ISR for init/sync on STM32G431) ────────────────
 *   Bit positions are identical to STM32F4 ISR, only the register name changed.
 */
#define ICSR_INIT       (1U << 7)   /*!< Init mode enable  (same bit as F4)   */

/* ── Public API ─────────────────────────────────────────────────────────── */
void rtc_init(void);
void rtc_alarm_init(void);
void rtc_timestamp_init(void);
void rtc_tamper_detect_init(void);

uint8_t  rtc_convert_bcd2bin(uint8_t value);
uint32_t rtc_date_get_day(void);
uint32_t rtc_date_get_year(void);
uint32_t rtc_date_get_month(void);
uint32_t rtc_time_get_second(void);
uint32_t rtc_time_get_minute(void);
uint32_t rtc_time_get_hour(void);
uint32_t get_current_timestamp(void);

#endif /* __RTC_H__ */

