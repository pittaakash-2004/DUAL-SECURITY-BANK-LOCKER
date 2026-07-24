//lcd.c
#include "defines.h"
#include "delay.h"
#include "types.h"
#include "lcd_defines.h"
#include "lcd.h"
#include <LPC21xx.h>

// Function to write one byte (command or data) to the LCD
void writeLCD(u8 Data)
{
    // Place the 8-bit data on the LCD data pins
    WRITEBYTE(IOPIN0, LCD_DATA, Data);

    // Make Enable (EN) pin HIGH to latch the data
    SSETBIT(IOSET0, LCD_EN);

    // Small delay for Enable pulse width
    delay_us(1);

    // Make Enable (EN) pin LOW
    // LCD reads the data on the falling edge of EN
    SCLRBIT(IOCLR0, LCD_EN);

    // Wait for the LCD to execute the command/data
    delay_ms(2);
}

// Function to send a command to the LCD
void CmdLCD(u8 cmd)
{
    // RS = 0 selects the command register
    SCLRBIT(IOCLR0, LCD_RS);

    // Send the command byte to the LCD
    writeLCD(cmd);
}

// Function to display a single ASCII character
void CharLCD(u8 asciival)
{
    // RS = 1 selects the data register
    SSETBIT(IOSET0, LCD_RS);

    // Send the ASCII character to the LCD
    writeLCD(asciival);
}

// Function to initialize the LCD
void InitLCD(void)
{
    // Configure LCD data pins and control pins as outputs
    IODIR0 |= ((0xFF << LCD_DATA) |
               (1 << LCD_RW) |
               (1 << LCD_RS) |
               (1 << LCD_EN));

    // Wait for LCD power stabilization
    delay_ms(15);

    // Wake-up sequence (as per HD44780 datasheet)
    CmdLCD(0x30);

    // Wait 5 ms
    delay_ms(5);

    // Send wake-up command again
    CmdLCD(0x30);

    // Wait 100 ms
    delay_ms(100);

    // Send wake-up command third time
    CmdLCD(0x30);

    // Configure LCD for 8-bit interface and 2-line display
    CmdLCD(MODE_8BIT_2LINE);

    // Turn ON display and turn OFF cursor
    CmdLCD(DSP_ON_CUR_OFF);

    // Clear the LCD screen
    CmdLCD(CLEAR_LCD);

    // Move cursor to the right after each character
    CmdLCD(SHIFT_CUR_RIGHT);
}

// Function to display a string on the LCD
void StrLCD(u8 *p)
{
    // Continue until the null character is reached
    while(*p)

        // Display one character and move to the next
        CharLCD(*p++);
}

// Function to display an unsigned 32-bit integer
void u32LCD(u32 data)
{
    s32 i = 0;          // Index variable
    u8 a[10];           // Temporary buffer to store digits

    // Special case when number is zero
    if(data == 0)
        CharLCD('0');
    else
    {
        // Extract digits from least significant to most significant
        while(data != 0)
        {
            a[i++] = (data % 10) + 48;   // Convert digit to ASCII
            data /= 10;                  // Remove last digit
        }

        // Display digits in reverse order
        for(--i; i >= 0; i--)
        {
            CharLCD(a[i]);
        }
    }
}

// Function to display a signed 32-bit integer
void s32LCD(s32 n)
{
    // Check if the number is negative
    if(n < 0)
    {
        // Display minus sign
        CharLCD('-');

        // Convert number to positive
        n = -n;
    }

    // Display the absolute value
    u32LCD(n);
}

// Function to display a floating-point number
// nDP = Number of digits after decimal point
void f32LCD(f32 n, u32 nDP)
{
    u32 data, i;

    // Check for negative number
    if(n < 0)
    {
        // Display minus sign
        CharLCD('-');

        // Convert to positive
        n = -n;
    }

    // Get integer part
    data = n;

    // Display integer part
    u32LCD(data);

    // Display decimal point
    CharLCD('.');

    // Display fractional part
    for(i = 0; i < nDP; i++)
    {
        // Shift next fractional digit to integer part
        n = (n - data) * 10;

        // Extract integer digit
        data = n;

        // Display digit
        CharLCD(data + 48);
    }
}

// Function to display a hexadecimal number
void HexLCD(u32 n)
{
    s32 i = 0;      // Index variable
    u8 a[8], t;     // Buffer for hexadecimal digits

    // Special case for zero
    if(n == 0)
    {
        CharLCD('0');
    }
    else
    {
        // Convert number into hexadecimal digits
        while(n != 0)
        {
            // Get remainder (0–15)
            t = (n % 16);

            // Convert to ASCII
            t = ((t > 9) ? ((t - 10) + 'A') : (t + 48));

            // Store digit
            a[i++] = t;

            // Divide by 16
            n /= 16;
        }

        // Display digits in correct order
        for(--i; i >= 0; i--)
        {
            CharLCD(a[i]);
        }
    }
}

// Function to build custom LCD characters in CGRAM
// p = Pointer to character pattern
// nBytes = Total bytes to write
void BuildCGRAM(u8 *p, u32 nBytes)
{
    s32 i;

    // Set CGRAM address to the beginning
    CmdLCD(GOTO_CGRAM_START);

    // Write custom character data into CGRAM
    for(i = 0; i < nBytes; i++)
    {
        CharLCD(p[i]);
    }

    // Return cursor to first line, first position
    CmdLCD(GOTO_LINE1_POS0);
}
