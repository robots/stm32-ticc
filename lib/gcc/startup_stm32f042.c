/**
 ******************************************************************************
 * @file      startup_stm32f10x_ld.c
 * @author    MCD Application Team, modif. Martin Thomas, Michal Demin
 * @version   V3.0.0-mthomas
 * @date      06/25/2015
 * @brief     STM32F042 Entry level Arm Cortex-M0 vector table for GNU toolchain.
 *            This module performs:
 *                - Set the initial SP
 *                - Set the initial PC == Reset_Handler,
 *                - Set the vector table entries with the exceptions ISR address,
 *                - Branches to main in the C library (which eventually
 *                  calls main()).
 *            After Reset the Cortex-Mx processor is in Thread mode,
 *            priority is Privileged, and the Stack is set to Main.
 *******************************************************************************
 * @copy
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
 */

/* Modified by Martin Thomas
   - to take VECT_TAB_RAM setting into account, also see the linker-script
   - to avoid warning "ISO C forbids initialization between function pointer and 'void *'".
   - added optional startup-delay to avoid unwanted operations while connecting with
     debugger/programmer
   - tested with the GNU arm-eabi toolchain as in CS G++ lite Q1/2009-161

  Michal Demin:
   - Changed interrupt vector for stm32f0x devices
*/

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef void( *const intfunc )( void );

/* Private define ------------------------------------------------------------*/
#define WEAK __attribute__ ((weak))

/* Private macro -------------------------------------------------------------*/
extern unsigned long _etext;
/* start address for the initialization values of the .data section.
defined in linker script */
extern unsigned long _sidata;

/* start address for the .data section. defined in linker script */
extern unsigned long _sdata;

/* end address for the .data section. defined in linker script */
extern unsigned long _edata;

/* start address for the .bss section. defined in linker script */
extern unsigned long _sbss;

/* end address for the .bss section. defined in linker script */
extern unsigned long _ebss;

/* init value for the stack pointer. defined in linker script */
extern unsigned long _estack;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void Reset_Handler(void) __attribute__((__interrupt__, noreturn));
extern int main(void);
void __Init_Data(void);
void Default_Handler(void);

/*******************************************************************************
*
*            Forward declaration of the default fault handlers.
*
*******************************************************************************/
//mthomas void WEAK Reset_Handler(void);
void WEAK NMI_Handler(void);
void WEAK HardFault_Handler(void);
void WEAK SVC_Handler(void);
void WEAK PendSV_Handler(void);
void WEAK SysTick_Handler(void);

/* External Interrupts */
void WEAK WWDG_IRQHandler(void);
void WEAK PVD_VDDIO2_IRQHandler(void);
void WEAK RTC_IRQHandler(void);
void WEAK FLASH_IRQHandler(void);
void WEAK RCC_CRS_IRQHandler(void);
void WEAK EXTI0_1_IRQHandler(void);
void WEAK EXTI2_3_IRQHandler(void);
void WEAK EXTI4_15_IRQHandler(void);
void WEAK TSC_IRQHandler(void);
void WEAK DMA1_Channel1_IRQHandler(void);
void WEAK DMA1_Channel2_3_IRQHandler(void);
void WEAK DMA1_Channel4_5_IRQHandler(void);
void WEAK ADC1_IRQHandler(void);
void WEAK TIM1_BRK_UP_TRG_COM_IRQHandler(void);
void WEAK TIM1_CC_IRQHandler(void);
void WEAK TIM2_IRQHandler(void);
void WEAK TIM3_IRQHandler(void);
void WEAK TIM14_IRQHandler(void);
void WEAK TIM16_IRQHandler(void);
void WEAK TIM17_IRQHandler(void);
void WEAK I2C1_IRQHandler(void);
void WEAK SPI1_IRQHandler(void);
void WEAK SPI2_IRQHandler(void);
void WEAK USART1_IRQHandler(void);
void WEAK USART2_IRQHandler(void);
void WEAK CEC_CAN_IRQHandler(void);
void WEAK USB_IRQHandler(void);

/* Private functions ---------------------------------------------------------*/
/******************************************************************************
*
* mthomas: If been built with VECT_TAB_RAM this creates two tables:
* (1) a minimal table (stack-pointer, reset-vector) used during startup
*     before relocation of the vector table using SCB_VTOR
* (2) a full table which is copied to RAM and used after vector relocation
*     (NVIC_SetVectorTable)
* If been built without VECT_TAB_RAM the following comment from STM is valid:
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/

#ifdef VECT_TAB_RAM
__attribute__ ((section(".isr_vectorsflash")))
void (* const g_pfnVectorsStartup[])(void) =
{
    (intfunc)((unsigned long)&_estack), /* The initial stack pointer during startup */
    Reset_Handler,             /* The reset handler during startup */
};
__attribute__ ((section(".isr_vectorsram")))
void (* g_pfnVectors[])(void) =
#else /* VECT_TAB_RAM */
__attribute__ ((section(".isr_vectorsflash")))
void (* const g_pfnVectors[])(void) =
#endif /* VECT_TAB_RAM */
{
    (intfunc)((unsigned long)&_estack), /* The stack pointer after relocation */
    Reset_Handler,              /* Reset Handler */
    NMI_Handler,                /* NMI Handler */
    HardFault_Handler,          /* Hard Fault Handler */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    SVC_Handler,                /* SVCall Handler - RTOS HOOK */
    0,                          /* Reserved */
    0,                          /* Reserved */
    PendSV_Handler,             /* PendSV Handler - RTOS HOOK */
    SysTick_Handler,            /* SysTick Handler - RTOS HOOK */

    /* External Interrupts */
    WWDG_IRQHandler,            /* Window Watchdog */
    PVD_VDDIO2_IRQHandler,      /* PVD and VDDIO2 through EXTI Line detect */
    RTC_IRQHandler,             /* RTC */
    FLASH_IRQHandler,           /* Flash */
    RCC_CRS_IRQHandler,         /* RCC and CRS */
    EXTI0_1_IRQHandler,         /* EXTI Line 0 and 1 */
    EXTI2_3_IRQHandler,         /* EXTI Line 2 and 3 */
    EXTI4_15_IRQHandler,        /* EXTI Line 4 to 15 */
    TSC_IRQHandler,             /* TS */
    DMA1_Channel1_IRQHandler,   /* DMA1 Channel 1 */
    DMA1_Channel2_3_IRQHandler, /* DMA1 Channel 2 and 3 */
    DMA1_Channel4_5_IRQHandler, /* DMA1 Channel 4 and 5 */
    ADC1_IRQHandler,          /* ADC1 & ADC2 */
    TIM1_BRK_UP_TRG_COM_IRQHandler, /* TIM1 Break, Update, Trigger and Commutation */
    TIM1_CC_IRQHandler,         /* TIM1 Capture Compare */
    TIM2_IRQHandler,            /* TIM2 */
    TIM3_IRQHandler,            /* TIM3 */
		0,
		0,
		TIM14_IRQHandler,           /* TIM14 */
		0,
		TIM16_IRQHandler,           /* TIM16 */
		TIM17_IRQHandler,           /* TIM17 */
		I2C1_IRQHandler,            /* I2C1 */
		0,
		SPI1_IRQHandler,            /* SPI1 */
		SPI2_IRQHandler,            /* SPI2 */
		USART1_IRQHandler,          /* USART1 */
		USART2_IRQHandler,          /* USART2 */
		0,
		CEC_CAN_IRQHandler,         /* CEC and CAN */
		USB_IRQHandler,             /* USB */
};

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

void Reset_Handler(void)
{

#ifdef STARTUP_DELAY
  volatile unsigned long i;
  for (i=0;i<500000;i++) { ; }
#endif

  /* Initialize data and bss */
  __Init_Data();

  /* Call the application's entry point.*/
  main();

  while(1) { ; }
}

/**
 * @brief  initializes data and bss sections
 * @param  None
 * @retval : None
*/

void __Init_Data(void)
{
  unsigned long *pulSrc, *pulDest;

  /* Copy the data segment initializers from flash to SRAM */
  pulSrc = &_sidata;

  for(pulDest = &_sdata; pulDest < &_edata; )
  {
    *(pulDest++) = *(pulSrc++);
  }
  /* Zero fill the bss segment. */
  for(pulDest = &_sbss; pulDest < &_ebss; )
  {
    *(pulDest++) = 0;
  }
}

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/
#pragma weak MMI_Handler = Default_Handler
#pragma weak SVC_Handler = Default_Handler
#pragma weak PendSV_Handler = Default_Handler
#pragma weak SysTick_Handler = Default_Handler

#pragma weak WWDG_IRQHandler = Default_Handler
#pragma weak PVD_VDDIO2_IRQHandler = Default_Handler
#pragma weak RTC_IRQHandler = Default_Handler
#pragma weak FLASH_IRQHandler = Default_Handler
#pragma weak RCC_CRS_IRQHandler = Default_Handler
#pragma weak EXTI0_1_IRQHandler = Default_Handler
#pragma weak EXTI2_3_IRQHandler = Default_Handler
#pragma weak EXTI4_15_IRQHandler = Default_Handler
#pragma weak TSC_IRQHandler = Default_Handler
#pragma weak DMA1_Channel1_IRQHandler = Default_Handler
#pragma weak DMA1_Channel2_3_IRQHandler = Default_Handler
#pragma weak DMA1_Channel4_5_IRQHandler = Default_Handler
#pragma weak ADC1_IRQHandler = Default_Handler
#pragma weak TIM1_BRK_UP_TRG_COM_IRQHandler = Default_Handler
#pragma weak TIM1_CC_IRQHandler = Default_Handler
#pragma weak TIM2_IRQHandler = Default_Handler
#pragma weak TIM3_IRQHandler = Default_Handler
#pragma weak TIM14_IRQHandler = Default_Handler
#pragma weak TIM16_IRQHandler = Default_Handler
#pragma weak TIM17_IRQHandler = Default_Handler
#pragma weak I2C1_IRQHandler = Default_Handler
#pragma weak SPI1_IRQHandler = Default_Handler
#pragma weak SPI2_IRQHandler = Default_Handler
#pragma weak USART1_IRQHandler = Default_Handler
#pragma weak USART2_IRQHandler = Default_Handler
#pragma weak CEC_CAN_IRQHandler = Default_Handler
#pragma weak USB_IRQHandler = Default_Handler
/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/

void Default_Handler(void)
{
  /* Go into an infinite loop. */
  while (1)
  {
  }
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
