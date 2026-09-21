/*
 * rtc.c – RTC bare-metal driver for STM32G431
 *
 * Ported from STM32F4.  Every change from the original is marked with a
 * "G431:" comment so the delta is easy to audit.
 *
 * Key changes at a glance
 * ────────────────────────────────────────────────────────────────────────────
 *  1.  RCC->APB1ENR      → RCC->APB1ENR1           (PWR clock enable)
 *  2.  RCC->AHB1ENR      → RCC->AHB2ENR            (GPIO clock enable)
 *  3.  PWR->CR           → PWR->CR1                 (backup-domain access)
 *  4.  RTC->ISR          → RTC->ICSR                (INIT / INITF / RSF bits)
 *  5.  RTC->ISR &= ~flag → RTC->SCR = flag          (write-1-to-clear pattern)
 *  6.  RTC->TAFCR        → TAMP->CR1/CR2/IER        (separate TAMP peripheral)
 *  7.  EXTI->IMR / RTSR  → EXTI->IMR1 / RTSR1
 *  8.  EXTI line 21      → EXTI line 19             (tamper / timestamp)
 *  9.  TAMP_STAMP_IRQn   → RTC_TAMP_LSECSS_IRQn
 * ────────────────────────────────────────────────────────────────────────────
 */

#include "rtc.h"

/*
 * READ_BIT / MODIFY_REG are normally provided by the STM32 family header
 * (stm32g4xx.h).  When building with the device-level header only
 * (stm32g431xx.h), define them here so no HAL dependency is introduced.
 */
#ifndef READ_REG
#define READ_REG(REG)               ((REG))
#endif
#ifndef WRITE_REG
#define WRITE_REG(REG, VAL)         ((REG) = (VAL))
#endif
#ifndef READ_BIT
#define READ_BIT(REG, BIT)          ((REG) & (BIT))
#endif
#ifndef MODIFY_REG
#define MODIFY_REG(REG, CLR, SET)   WRITE_REG((REG), ((READ_REG(REG) & ~(CLR)) | (SET)))
#endif

/* ── Private defines ─────────────────────────────────────────────────────── */

/*
 * G431: PWREN is still bit 28, but now lives in APB1ENR1, not APB1ENR.
 *       (The original tamper-init mistakenly enabled it via AHB1ENR – fixed.)
 */
#define GPIOCEN         (1U << 2)   /* GPIOC clock – bit 2 of AHB2ENR (G431) */
#define PWREN           (1U << 28)  /* PWR clock   – bit 28 of APB1ENR1      */
#define CR_DBP          (1U << 8)   /* Backup-domain write protection disable  */

/* LSI control – RCC->CSR bits (unchanged) */
#define CSR_LSION       (1U << 0)
#define CSR_LSIRDY      (1U << 1)

/* Backup-domain reset / RTC enable – RCC->BDCR bits (unchanged) */
#define BDCR_BDRST      (1U << 16)
#define BDCR_RTCEN      (1U << 15)

/* Write-protection keys */
#define RTC_WRITE_PROTECTION_KEY_1  ((uint8_t)0xCAU)
#define RTC_WRITE_PROTECTION_KEY_2  ((uint8_t)0x53U)

/*
 * G431: RTC->ICSR replaces RTC->ISR for init / sync status.
 *       Bit positions of INITF and RSF are identical to the F4 ISR.
 */
#define ISR_INITF       (1U << 6)   /* INITF – bit 6 of RTC->ICSR (same as F4) */
#define ISR_RSF         (1U << 5)   /* RSF   – bit 5 of RTC->ICSR (same as F4) */

/* RTC->CR bits (unchanged) */
#define TIME_FORMAT_AM  0x0000U
#define TIME_FORMAT_PM  (1U << 22)
#define CR_FMT          (1U << 6)
#define CR_ALRAE        (1U << 8)
#define CR_TSEDGE       (1U << 3)

/* Calendar defaults */
#define WEEKDAY_FRIDAY  ((uint8_t)0x05U)
#define MONTH_DECEMBER  ((uint8_t)0x12U)

/*
 * Prescalers for LSI @ ~32 kHz:
 *   f_ck_apre = 32000 / (ASYNCH + 1) = 32000 / 128 = 250 Hz
 *   f_ck_spre = 250   / (SYNCH  + 1) = 250   / 250 = 1 Hz  ✓
 *
 * Note: STM32G431 LSI is nominally 32 kHz but has a wide tolerance (±15 %).
 * For accurate timekeeping consider using the LSE (32.768 kHz crystal) with
 * ASYNCH = 127, SYNCH = 255.
 */
#define RTC_ASYNCH_PREDIV   ((uint32_t)0x7F)    /* 127 */
#define RTC_SYNCH_PREDIV    ((uint32_t)0x00F9)  /* 249 */

/*
 * G431 EXTI lines for RTC events
 *   Line 17 – RTC Alarm A / Alarm B  (same as STM32F4)
 *   Line 19 – RTC Tamper / Timestamp (was line 21 on STM32F4)
 */
#define EXTI_LINE_RTC_ALARM     (1U << 17)
#define EXTI_LINE_RTC_TAMP_TS   (1U << 19)  /* G431: line 19 */

/* ── Private function prototypes ─────────────────────────────────────────── */

static uint8_t rtc_init_seq(void);
static void    rtc_date_config(uint32_t WeekDay, uint32_t Day,
                               uint32_t Month,   uint32_t Year);
static void    rtc_time_config(uint32_t Format12_24, uint32_t Hours,
                               uint32_t Minutes,     uint32_t Seconds);
static void    rtc_set_asynch_prescaler(uint32_t AsynchPrescaler);
static void    rtc_set_synch_prescaler(uint32_t SynchPrescaler);
static uint8_t exit_init_seq(void);
static void    rtc_alma_config_time(uint32_t Format12_24, uint32_t Hours,
                                    uint32_t Minutes,     uint32_t Seconds);
static void    rtc_alma_set_mask(uint32_t Mask);
static uint8_t wait_for_synchro(void);


/* ══════════════════════════════════════════════════════════════════════════ */
/*  Public functions                                                          */
/* ══════════════════════════════════════════════════════════════════════════ */

void rtc_init(void)
{
    /* G431: PWREN is in APB1ENR1 (not APB1ENR as on STM32F4) */
    RCC->APB1ENR1 |= PWREN;

    /* G431: DBP bit is in PWR->CR1 (not PWR->CR as on STM32F4) */
    PWR->CR1 |= CR_DBP;

    /* Enable LSI and wait for it to stabilise (register unchanged) */
    RCC->CSR |= CSR_LSION;
    while ((RCC->CSR & CSR_LSIRDY) != CSR_LSIRDY) {}

    /* Reset then release the backup domain */
    RCC->BDCR |= BDCR_BDRST;
    RCC->BDCR &= ~BDCR_BDRST;

    /* Select LSI as RTC clock source (RTCSEL = 10b → bits [9:8] = 10) */
    RCC->BDCR &= ~(1U << 8);
    RCC->BDCR |=  (1U << 9);

    /* Enable the RTC */
    RCC->BDCR |= BDCR_RTCEN;

    /* Disable write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY_1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY_2;

    /* Enter init mode */
    if (rtc_init_seq() != 1)
    {
        /* Handle error */
    }

    /* Configure calendar: Friday, 29 December 2016 */
    rtc_date_config(WEEKDAY_FRIDAY, 0x29, MONTH_DECEMBER, 0x16);

    /* Configure time: 11:59:55 PM */
    rtc_time_config(TIME_FORMAT_PM, 0x11, 0x59, 0x55);

    /* 12-hour format */
    RTC->CR |= CR_FMT;

    /* Configure prescalers */
    rtc_set_asynch_prescaler(RTC_ASYNCH_PREDIV);
    rtc_set_synch_prescaler(RTC_SYNCH_PREDIV);

    /* Exit init mode */
    exit_init_seq();

    /* Re-enable write protection */
    RTC->WPR = 0xFF;
}


void rtc_alarm_init(void)
{
    /* Disable write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY_1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY_2;

    /* Enter init mode */
    if (rtc_init_seq() != 1)
    {
        /* Handle error */
    }

    /* Calendar: Friday, 29 December 2016 */
    rtc_date_config(WEEKDAY_FRIDAY, 0x29, MONTH_DECEMBER, 0x16);

    /* Time: 11:59:55 PM */
    rtc_time_config(TIME_FORMAT_PM, 0x11, 0x59, 0x55);

    /* Alarm A: 12:00:02 AM */
    rtc_alma_config_time(TIME_FORMAT_AM, 0x12, 0x00, 0x02);

    /* Ignore weekday comparison */
    rtc_alma_set_mask(RTC_ALRMAR_MSK4);

    /* Enable Alarm A */
    RTC->CR |= CR_ALRAE;

    /*
     * G431: Status flags live in RTC->SR (read-only).
     *       Clear them by writing 1 to the matching bit in RTC->SCR.
     *       Old F4 code: RTC->ISR &= ~ISR_ALRAF
     */
    RTC->SCR = SCR_CALRAF;

    /* Enable Alarm A interrupt */
    RTC->CR |= CR_ALRAIE;

    /*
     * G431: EXTI uses IMR1 / RTSR1 (split registers).
     *       Alarm line is still 17 (same as STM32F4).
     */
    EXTI->IMR1  |= EXTI_LINE_RTC_ALARM;
    EXTI->RTSR1 |= EXTI_LINE_RTC_ALARM;

    /* Enable RTC Alarm IRQ in NVIC (name unchanged) */
    NVIC_EnableIRQ(RTC_Alarm_IRQn);

    /* Exit init mode */
    exit_init_seq();

    /* Re-enable write protection */
    RTC->WPR = 0xFF;
}


void rtc_timestamp_init(void)
{
    /* Disable write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY_1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY_2;

    /* Timestamp on rising edge */
    RTC->CR &= ~CR_TSEDGE;

    /* PC13 is RTC_AF1; enable timestamp */
    RTC->CR |= CR_TSE;

    /* Enable timestamp interrupt */
    RTC->CR |= CR_TSIE;

    /*
     * G431: Timestamp / Tamper EXTI is line 19 (was line 21 on STM32F4).
     *       Registers are IMR1 / RTSR1.
     */
    EXTI->IMR1  |= EXTI_LINE_RTC_TAMP_TS;
    EXTI->RTSR1 |= EXTI_LINE_RTC_TAMP_TS;

    /*
     * G431: IRQ for Tamper / Timestamp is now named
     *       RTC_TAMP_LSECSS_IRQn (was TAMP_STAMP_IRQn on F4).
     */
    NVIC_EnableIRQ(RTC_TAMP_LSECSS_IRQn);

    /* Enter init mode */
    if (rtc_init_seq() != 1)
    {
        /* Handle error */
    }

    /* Calendar: Friday, 29 December 2016 */
    rtc_date_config(WEEKDAY_FRIDAY, 0x29, MONTH_DECEMBER, 0x16);

    /* Time: 11:59:55 PM */
    rtc_time_config(TIME_FORMAT_PM, 0x11, 0x59, 0x55);

    /* Exit init mode */
    exit_init_seq();

    /* Re-enable write protection */
    RTC->WPR = 0xFF;

    /*
     * Configure PC13 as input (timestamp pin, RTC_AF1).
     * G431: GPIO clock is in AHB2ENR (was AHB1ENR on STM32F4).
     */
    RCC->AHB2ENR |= GPIOCEN;

    /* PC13: MODER[27:26] = 00 (input) */
    GPIOC->MODER &= ~(1U << 26);
    GPIOC->MODER &= ~(1U << 27);
}


void rtc_tamper_detect_init(void)
{
    /*
     * G431: PWREN is in APB1ENR1.
     *       Note: the original STM32F4 code used AHB1ENR here by mistake;
     *       the correct register on F4 was APB1ENR.  Fixed for G431.
     */
    RCC->APB1ENR1 |= PWREN;

    /* G431: DBP in PWR->CR1 (original code already used CR1 here) */
    PWR->CR1 |= CR_DBP;

    /* Enable the RTC */
    RCC->BDCR |= BDCR_RTCEN;

    /*
     * G431: TAFCR no longer exists.  Tamper configuration has moved to the
     *       dedicated TAMP peripheral (base 0x40002400).
     *
     *   RTC_CR_TAMPTS    – "Timestamp on tamper" – stays in RTC->CR.
     *   TAMP0E / TAMPIE  – moved to TAMP->CR1 / TAMP->IER.
     *
     *   The original code wrote RTC_CR_TAMPTS to TAFCR (copy-paste error);
     *   on G431 it belongs to RTC->CR.
     */

    /* Enable timestamp on tamper event */
    RTC->CR |= RTC_CR_TAMPTS;

    /* Enable TAMP1 input (replaces RTC_TAFCR_TAMP0E) */
    TAMP->CR1 |= TAMP_CR1_TAMP1E;

    /*
     * Set TAMP1 trigger polarity to falling edge.
     * TAMP->CR2: TAMP1TRG = 1 → falling edge
     * (On F4 this was RTC_TAFCR_TAMP1ETRG.)
     */
    TAMP->CR2 |= TAMP_CR2_TAMP1TRG;

    /* Enable TAMP1 interrupt (replaces RTC_TAFCR_TAMPIE) */
    TAMP->IER |= TAMP_IER_TAMP1IE;

    /*
     * G431: IRQ name changed.
     *       Old: TAMP_STAMP_IRQn
     *       New: RTC_TAMP_LSECSS_IRQn
     */
    NVIC_EnableIRQ(RTC_TAMP_LSECSS_IRQn);

    /*
     * G431: Tamper/Timestamp EXTI line is 19 (was 21 on STM32F4).
     *       Registers are IMR1 / RTSR1.
     */
    EXTI->IMR1  |= EXTI_LINE_RTC_TAMP_TS;
    EXTI->RTSR1 |= EXTI_LINE_RTC_TAMP_TS;

    /*
     * G431: Tamper flags are now in the TAMP peripheral.
     *       Clear TAMP1 flag by writing 1 to TAMP->SCR bit CTAMP1F.
     *       Old: RTC->ISR &= ~RTC_ISR_TAMP1F
     */
    TAMP->SCR = TAMP_SCR_CTAMP1F;
}


/* ══════════════════════════════════════════════════════════════════════════ */
/*  Utility functions (BCD / calendar reads)                                  */
/* ══════════════════════════════════════════════════════════════════════════ */

uint8_t rtc_convert_bin2bcd(uint8_t value)
{
    return (uint8_t)((((value) / 10U) << 4U) | ((value) % 10U));
}

uint8_t rtc_convert_bcd2bin(uint8_t value)
{
    return (uint8_t)(((uint8_t)((value) & (uint8_t)0xF0U) >> (uint8_t)0x4U) * 10U
                     + ((value) & (uint8_t)0x0FU));
}

uint32_t rtc_date_get_day(void)
{
    return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_DT | RTC_DR_DU))) >> RTC_DR_DU_Pos);
}

uint32_t rtc_date_get_year(void)
{
    return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_YT | RTC_DR_YU))) >> RTC_DR_YU_Pos);
}

uint32_t rtc_date_get_month(void)
{
    return (uint32_t)((READ_BIT(RTC->DR, (RTC_DR_MT | RTC_DR_MU))) >> RTC_DR_MU_Pos);
}

uint32_t rtc_time_get_second(void)
{
    return (uint32_t)(READ_BIT(RTC->TR, (RTC_TR_ST | RTC_TR_SU)) >> RTC_TR_SU_Pos);
}

uint32_t rtc_time_get_minute(void)
{
    return (uint32_t)((READ_BIT(RTC->TR, (RTC_TR_MNT | RTC_TR_MNU))) >> RTC_TR_MNU_Pos);
}

uint32_t rtc_time_get_hour(void)
{
    return (uint32_t)((READ_BIT(RTC->TR, (RTC_TR_HT | RTC_TR_HU))) >> RTC_TR_HU_Pos);
}


/* ══════════════════════════════════════════════════════════════════════════ */
/*  Static / private helpers                                                  */
/* ══════════════════════════════════════════════════════════════════════════ */

static void rtc_set_asynch_prescaler(uint32_t AsynchPrescaler)
{
    MODIFY_REG(RTC->PRER, RTC_PRER_PREDIV_A,
               AsynchPrescaler << RTC_PRER_PREDIV_A_Pos);
}

static void rtc_set_synch_prescaler(uint32_t SynchPrescaler)
{
    MODIFY_REG(RTC->PRER, RTC_PRER_PREDIV_S, SynchPrescaler);
}

static void rtc_alma_set_mask(uint32_t Mask)
{
    MODIFY_REG(RTC->ALRMAR,
               RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 |
               RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1,
               Mask);
}

static void rtc_alma_config_time(uint32_t Format12_24, uint32_t Hours,
                                  uint32_t Minutes,     uint32_t Seconds)
{
    register uint32_t temp = 0U;

    temp = Format12_24
         | (((Hours   & 0xF0U) << (RTC_ALRMAR_HT_Pos  - 4U)) | ((Hours   & 0x0FU) << RTC_ALRMAR_HU_Pos))
         | (((Minutes & 0xF0U) << (RTC_ALRMAR_MNT_Pos - 4U)) | ((Minutes & 0x0FU) << RTC_ALRMAR_MNU_Pos))
         | (((Seconds & 0xF0U) << (RTC_ALRMAR_ST_Pos  - 4U)) | ((Seconds & 0x0FU) << RTC_ALRMAR_SU_Pos));

    MODIFY_REG(RTC->ALRMAR,
               RTC_ALRMAR_PM  | RTC_ALRMAR_HT  | RTC_ALRMAR_HU  |
               RTC_ALRMAR_MNT | RTC_ALRMAR_MNU |
               RTC_ALRMAR_ST  | RTC_ALRMAR_SU,
               temp);
}

static void rtc_date_config(uint32_t WeekDay, uint32_t Day,
                             uint32_t Month,   uint32_t Year)
{
    register uint32_t temp = 0U;

    temp = (WeekDay << RTC_DR_WDU_Pos)
         | (((Year  & 0xF0U) << (RTC_DR_YT_Pos - 4U)) | ((Year  & 0x0FU) << RTC_DR_YU_Pos))
         | (((Month & 0xF0U) << (RTC_DR_MT_Pos - 4U)) | ((Month & 0x0FU) << RTC_DR_MU_Pos))
         | (((Day   & 0xF0U) << (RTC_DR_DT_Pos - 4U)) | ((Day   & 0x0FU) << RTC_DR_DU_Pos));

    MODIFY_REG(RTC->DR,
               RTC_DR_WDU | RTC_DR_MT | RTC_DR_MU |
               RTC_DR_DT  | RTC_DR_DU |
               RTC_DR_YT  | RTC_DR_YU,
               temp);
}

static void rtc_time_config(uint32_t Format12_24, uint32_t Hours,
                             uint32_t Minutes,     uint32_t Seconds)
{
    register uint32_t temp = 0U;

    temp = Format12_24
         | (((Hours   & 0xF0U) << (RTC_TR_HT_Pos  - 4U)) | ((Hours   & 0x0FU) << RTC_TR_HU_Pos))
         | (((Minutes & 0xF0U) << (RTC_TR_MNT_Pos - 4U)) | ((Minutes & 0x0FU) << RTC_TR_MNU_Pos))
         | (((Seconds & 0xF0U) << (RTC_TR_ST_Pos  - 4U)) | ((Seconds & 0x0FU) << RTC_TR_SU_Pos));

    MODIFY_REG(RTC->TR,
               RTC_TR_PM  | RTC_TR_HT  | RTC_TR_HU  |
               RTC_TR_MNT | RTC_TR_MNU |
               RTC_TR_ST  | RTC_TR_SU,
               temp);
}

/* ── Init-mode helpers ───────────────────────────────────────────────────── */

/*
 * G431: Init mode is controlled via RTC->ICSR (not RTC->ISR).
 *       Write the INIT bit (bit 7) to enter / exit.
 *       Old F4 pattern:  RTC->ISR = 0xFFFF  / RTC->ISR = ~0xFFFF
 *       New G431 pattern: RTC->ICSR |= INIT / RTC->ICSR &= ~INIT
 */
void _rtc_enable_init_mode(void)
{
    RTC->ICSR |= RTC_ICSR_INIT;     /* G431: RTC->ICSR instead of RTC->ISR */
}

void _rtc_disable_init_mode(void)
{
    RTC->ICSR &= ~RTC_ICSR_INIT;    /* G431: RTC->ICSR instead of RTC->ISR */
}

/*
 * G431: INITF and RSF reside in RTC->ICSR.
 *       Bit positions (6 and 5) are identical to STM32F4 ISR.
 */
uint8_t _rtc_isActiveflag_init(void)
{
    return ((RTC->ICSR & ISR_INITF) == ISR_INITF);  /* G431: ICSR */
}

uint8_t _rtc_isActiveflag_rs(void)
{
    return ((RTC->ICSR & ISR_RSF) == ISR_RSF);       /* G431: ICSR */
}

static uint8_t rtc_init_seq(void)
{
    _rtc_enable_init_mode();
    while (_rtc_isActiveflag_init() != 1) {}
    return 1;
}

static uint8_t wait_for_synchro(void)
{
    /*
     * G431: RSF (bit 5) is in RTC->ICSR; cleared by writing 0 (same as F4).
     */
    RTC->ICSR &= ~ISR_RSF;                   /* G431: ICSR */
    while (_rtc_isActiveflag_rs() != 1) {}
    return 1;
}

static uint8_t exit_init_seq(void)
{
    _rtc_disable_init_mode();
    return wait_for_synchro();
}

/**
 * @brief Get the current timestamp from the RTC.
 * @return Current timestamp as a 32-bit integer.
 */
uint32_t get_current_timestamp(void)
{
    uint32_t hour = rtc_time_get_hour();
    uint32_t minute = rtc_time_get_minute();
    uint32_t second = rtc_time_get_second();

    return (hour * 3600) + (minute * 60) + (second);
}