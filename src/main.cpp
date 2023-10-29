/* Arduino libraries */
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <tic.h>
#include <wk2132.h>

/* C/C++ libraries */
#include <inttypes.h>

/* Peripherals */
static wk2132 m_wk2132;
static tic m_tic_parser;
static LiquidCrystal m_lcd(8, 9, 9, 4, 5, 6, 7);

/**
 *
 */
void setup(void) {

    /* Setup UART used for debug */
    Serial.begin(115200);
    Serial.println("Hello world");

    /* Setup LCD */
    m_lcd.begin(16, 2);
    m_lcd.clear();
    m_lcd.print("Hello world");

    /* Setup I2C */
    Wire.begin();

    /* Setup UART exapander
     * The first parameter is the frequency (in Hz) of the crystal attached to the WK2132 IC
     * The second parameter is a reference to the default I2C library used to talk with the WK2132 IC
     * The other paramters define the I2C address */
    m_wk2132.setup(11059200, Wire, 0, 0);

    /* Setup UART port that listens to data coming from the electricity meter
     * The parameter is the baudrate: 1200 for HISTORIC (most meters) or 9600 for STANDARD */
    m_wk2132.uarts[0].begin(1200);

    /* Setup tic parser
     * The parameter is a reference the first UART of the WK2132 IC */
    m_tic_parser.setup(m_wk2132.uarts[0]);
}

/**
 *
 */
void loop(void) {

    /* Listen for incoming TIC messages */
    struct tic_message msg;
    int res = m_tic_parser.process(msg);
    if (res < 0) {
        Serial.printf("Failed to process incoming data! Maybe change baudrate?\r\n");
    } else if (res > 0) {

        /* Log received message */
        Serial.printf("Received message: %s = %s\r\n", msg.name, msg.data);

        /* If we have received the BASE consumption index, print it on the LDC display */
        if (strcmp_P(msg.name, PSTR("BASE")) == 0) {
            uint32_t base = strtoul(msg.data, NULL, 10);
            m_lcd.clear();
            m_lcd.setCursor(0, 0);
            m_lcd.print("Index BASE =");
            m_lcd.setCursor(0, 1);
            m_lcd.printf("%" PRIu32 ".%03" PRIu32 " kWh", base / 1000U, base % 1000U);
        }
    }
}
