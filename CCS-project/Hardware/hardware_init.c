#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <Hardware/I2cDriver/HAL_I2C.h>
#include <Hardware/HumTempDriver/HAL_DHT20.h>
#include <Hardware/LuxDriver/HAL_OPT3001.h>
#include <stdio.h>
#include "msp.h"

#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "Graphics/graphics_context.h"

#define TIMER_PERIOD0   0x7530   // 30 000 to hex

const Timer_A_UpModeConfig upConfigTimerA0 = {
        TIMER_A_CLOCKSOURCE_ACLK,                       // ACLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,                     // ACLK/64 = 500 Hz
        TIMER_PERIOD0,                                  // every minute
        TIMER_A_TAIE_INTERRUPT_ENABLE,                  // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,             // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                                // Clear value
};

const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source (3MHz)
        26,                                             // prescaler for timer
        0,                                              // first modulation stage select
        111,                                            // second modulation stage select
        EUSCI_A_UART_NO_PARITY,                         // No Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling enable
        EUSCI_A_UART_8_BIT_LEN                          // 8 bit data length
};

// stops watchdog timer and initializes some flash memory features (e.g. Voltage)
void basicInit () {
    // Stop watchdog timer
    WDT_A_holdTimer();

    /* Halting WDT and disabling master interrupts */
    Interrupt_disableMaster();

    Interrupt_enableSleepOnIsrExit();

    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
}


// initializes the two timers used for graphics updating and IR signals sending
void timerInit () {
    /* Initializes Clock System */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Configure the up mode timer
    Timer_A_configureUpMode(TIMER_A0_BASE,&upConfigTimerA0);

    //We enable the interrupt
    Interrupt_enableInterrupt(INT_TA0_N);

    // start timer
    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
}


void ledInit () {
    /* Configuring RG as output */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4); // green
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6); // red
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
}


void buttonsInit () {
    /* P1.4 as input for button C*/
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    /* P3.5 as input for button B*/
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);

    /* P4.1 as input for button J*/
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN1);

    /* P5.1 as input for button A*/
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);


    /* Enable interrupts on Port 1-3-4-5 (to catch P1.4,P3.5,P4.1,P5.1) */
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableInterrupt(INT_PORT3);
    Interrupt_enableInterrupt(INT_PORT4);
    Interrupt_enableInterrupt(INT_PORT5);
}


void joystickInit () {
    /* Configures Pin 6.0 and 4.4 as ADC input */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
     * with internal 2.5v reference */
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, ADC_DIFFERENTIAL_INPUTS);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

    /* Enabling the interrupt when a conversion on channel 1 (end of sequence)
     *  is complete and enabling conversions */
    ADC14_enableInterrupt(ADC_INT1);
}


void adcInit () {
    /* Initializing ADC (ADCOSC/64/8) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ACLK, ADC_PREDIVIDER_32, ADC_DIVIDER_4, 0);

    /* Enabling Interrupts */
    Interrupt_enableInterrupt(INT_ADC14);

    /* Setting up the sample timer to automatically step through the sequence convert. */
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Triggering the start of the sample */
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}


//initialize Liquid Crystal Display
void graphicsInit () {
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor (&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

void i2cInit () {
    /* Initialize I2C communication */
    Init_I2C_GPIO();
    I2C_init();
}

void lightSensorInit () {
    /* Initialize OPT3001 digital ambient light sensor */
    OPT3001_init();
}

void humiditytemperatureSensorInit () {
    /* Initialize DHT20 humidity and temperature sensor */
    dht20_init ();
}

void uartInit(){
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
}

void hwInit () {
    basicInit ();
    timerInit ();
    i2cInit ();
    uartInit ();
    lightSensorInit ();
    humiditytemperatureSensorInit ();
    buttonsInit ();
    ledInit ();
    graphicsInit ();
    joystickInit ();
    adcInit ();
}
