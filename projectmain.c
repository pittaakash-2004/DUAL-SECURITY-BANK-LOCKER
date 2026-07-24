#include<LPC21XX.h>
#include"defines.h"
#include"types.h"
#include"delay.h"
#include"lcd.h"
#include"lcd_defines.h"
#include"kpm.h"
#include "ext_int.h"
#include "i2c.h"
#include "i2c_eeprom.h"
#include "device.h"
#include "R305.h" 
#include "uart0.h"
#include<string.h>
#define SW_AL 5
unsigned char pass[5],curpass[5],newpass[5];
unsigned char *pas=pass,*curpas=curpass,*newpas=newpass;
unsigned char choose;
unsigned int pageid;
u8 userid[5],ruserid[5],pwd[5],rpwd[5];
u8*usrid,*rpd,*pd;
void menu(void);
void edit_pass(void);
void edit_fp(void);
extern unsigned char a;
extern int flag;

int mystrcmp(const char* a,const char* b)
{
	int k;
	for(k=0;a[k];k++)
	{
		if(a[k]!=b[k])
		{
			return a[k]-b[k];
		}
	}
	return 0;	
}

int main()
{
	InitUART0();                                     // Initialize UART0 for serial communication with fingerprint module

	init_i2c();                                      // Initialize I2C interface for EEPROM communication

	InitLCD();                                       // Initialize the 16x2 LCD

	Init_KPM();                                      // Initialize the keypad module

	Enable_EINT2();                                  // Enable External Interrupt 2 for menu access

	StrLCD("Welcome to Bank");                       // Display welcome message on LCD

	CmdLCD(GOTO_LINE2_POS0);                         // Move cursor to the second line, first position

	StrLCD("   Akash   ");                           // Display project/developer name

	delay_s(1);                                      // Wait for 1 second

	i2c_eeprom_pagewrite(0x50,0x0010,"1111",4);      // Store default User ID "1111" into EEPROM

	delay_ms(100);                                   // Wait until EEPROM write operation completes

	i2c_eeprom_seqread(0x50,0x0010,ruserid,4);       // Read stored User ID from EEPROM

	StrLCD(ruserid);                                 // Display the stored User ID (for testing)

	delay_ms(500);                                   // Wait for 500 milliseconds

	i2c_eeprom_pagewrite(0x50,0x0100,"1234",4);      // Store default password "1234" into EEPROM

	i2c_eeprom_seqread(0x50,0x0100,rpwd,4);          // Read the stored password from EEPROM

	while(1)                                         // Infinite loop for continuous authentication
	{
LABEL1:
		CmdLCD(CLEAR_LCD);                           // Clear the LCD display

		StrLCD("Enter 4 digit id:");                 // Prompt user to enter User ID

		delay_s(1);                                 // Wait for 1 second

		while(ColScan()==1)                         // Wait until any keypad key is pressed
		{
			if(a==1)                                // Check whether external interrupt occurred
			{
				menu();                             // Open administrator menu

				goto LABEL1;                        // Return to User ID entry screen
			}
		}

		CmdLCD(GOTO_LINE2_POS0);                    // Move cursor to second line

		usrid=Read(userid);                         // Read 4-digit User ID from keypad

//		StrLCD(usrid);                             // Display entered User ID (Debug)

//		delay_ms(2000);                            // Delay for debugging

//		delay_ms(1000);                            // Delay for debugging

//		CmdLCD(CLEAR_LCD);                         // Clear LCD (Debug)

		i2c_eeprom_seqread(0x50,0x0010,ruserid,4);  // Read stored User ID from EEPROM

		if((mystrcmp((const char*)usrid,(const char*)ruserid))==0) // Compare entered User ID with stored User ID
		{
			i2c_eeprom_seqread(0x50,0x0100,rpwd,4); // Read stored password from EEPROM

			CmdLCD(CLEAR_LCD);                      // Clear LCD

//			CmdLCD(GOTO_LINE1_POS0);               // Move cursor to first line (not required)

			StrLCD("Enter 4 digit pwd:");           // Ask user to enter password

			CmdLCD(GOTO_LINE2_POS0);                // Move cursor to second line

			pd=Read(pwd);                           // Read 4-digit password from keypad

			CmdLCD(GOTO_LINE2_POS0);                // Move cursor to second line

//			CmdLCD(CLEAR_LCD);                     // Clear LCD (Debug)

//			StrLCD(rpwd);                          // Display stored password (Debug)

//			delay_ms(1000);                        // Delay for debugging

//			i2c_eeprom_seqread(0x50,0x000F,ruserid,4); // Old EEPROM read statement

			CmdLCD(GOTO_LINE2_POS0);                // Move cursor to second line

			StrLCD(pd);                             // Display entered password (for testing)

			delay_ms(1000);                         // Wait for 1 second

			if((mystrcmp((const char*)pd,(const char*)rpwd))==0) // Compare entered password with stored password
	 		{
				CmdLCD(0x01);                       // Clear LCD display

	   			CmdLCD(0x80);                       // Move cursor to first line

          		if(search_fp())                    // Verify fingerprint using R305 sensor
				{
					CmdLCD(CLEAR_LCD);             // Clear LCD

					CmdLCD(GOTO_LINE1_POS0);       // Move cursor to first line

					StrLCD("BANK LOCK OPEN");      // Display successful authentication message

					do                            // Repeat until locker switch is pressed
					{
					 	if(flag==0)               // Check whether motor is free to rotate
							motor_open();         // Rotate motor to open bank locker
					}
					while(READBIT(IOPIN0,SW_AL)==1); // Continue until switch becomes LOW

					motor_close();                // Rotate motor to close the locker
				}

				delay_ms(1000);                  // Wait for 1 second before next operation
			}
			else                                 // Executed when password does not match
			{
				CmdLCD(CLEAR_LCD);               // Clear LCD

				CmdLCD(GOTO_LINE1_POS0);         // Move cursor to first line

				StrLCD("pwd incorrect");         // Display incorrect password message

				CmdLCD(GOTO_LINE2_POS0);         // Move cursor to second line

				StrLCD("Wait & Enter agn");      // Ask user to try again

				delay_s(1);                      // Wait for 1 second
			}
		}
		else                                     // Executed when User ID does not match
		{
			CmdLCD(CLEAR_LCD);                   // Clear LCD

			CmdLCD(GOTO_LINE1_POS0);             // Move cursor to first line

			StrLCD("Usr id incorrect");          // Display incorrect User ID message

			CmdLCD(GOTO_LINE2_POS0);             // Move cursor to second line

			StrLCD("Wait & Enter agn");          // Ask user to enter the User ID again

			delay_s(1);                          // Wait for 1 second
		}
	}
}

void menu()
{
	InitUART0();                          // Initialize UART0 for communication with the fingerprint module

	a=0;                                  // Clear the external interrupt flag

	CmdLCD(CLEAR_LCD);                    // Clear the LCD display

	CmdLCD(GOTO_LINE1_POS0);              // Move the cursor to the first row, first column

	StrLCD("MENU: ");                     // Display the menu heading

	StrLCD("1)Edit pass");                // Display option 1 to edit the password

	CmdLCD(GOTO_LINE2_POS0);              // Move the cursor to the second row, first column

	StrLCD("2)Edit FP 3)EXIT");           // Display option 2 to edit fingerprint and option 3 to exit

	if(KeyScan()=='1')                    // Check if the user pressed key '1'
	{
		edit_pass();                      // Call the password editing function

		return;                           // Return to the calling function after password update
	}

	if(KeyScan()=='2')                    // Check if the user pressed key '2'
	{
		edit_fp();                        // Call the fingerprint management function
	}

	return;                               // Return to the calling function if no valid option is selected
}
void edit_fp(void)
{
    InitLCD();                               // Initialize the LCD display

    InitUART0();                             // Initialize UART0 for communication with the R305 fingerprint sensor

    CmdLCD(0x01);                            // Clear the LCD display

    CmdLCD(0x80);                            // Move the cursor to the first row, first column

    StrLCD("FINGER PRINT ");                 // Display the title "FINGER PRINT"

    delay_ms(2000);                          // Wait for 2 seconds

    while (1)                                // Repeat until the user exits
    {
        CmdLCD(0x01);                        // Clear the LCD

        CmdLCD(0x80);                        // Move cursor to the first row

        StrLCD("1.ENROLL 3.DEL ");           // Display Enroll and Delete options

        CmdLCD(0xC0);                        // Move cursor to the second row

        StrLCD("2.SEARCH 4.D.ALL");          // Display Search and Delete All options

        choose = KeyScan();                  // Read the user's keypad selection

        switch(choose)                       // Execute the selected menu option
        {

        case '1':                            // Option 1: Enroll a new fingerprint

            CmdLCD(0x01);                    // Clear the LCD

            CmdLCD(0x80);                    // Move cursor to the first row

            StrLCD("Enter ID: ");            // Ask the user to enter a fingerprint ID

            pageid = ReadNum();              // Read the fingerprint ID from the keypad

            CmdLCD(0xC0);                    // Move cursor to the second row

            u32LCD(pageid);                  // Display the entered fingerprint ID

            delay_ms(100);                   // Small delay

            if(pageid > 0 && pageid < 10)    // Check whether the ID is valid (1 to 9)
            {
                enroll(pageid);              // Enroll the fingerprint with the entered ID
            }

            break;                           // Exit this case

        case '2':                            // Option 2: Search for a fingerprint

            CmdLCD(0x01);                    // Clear the LCD

            CmdLCD(0x80);                    // Move cursor to the first row

            StrLCD("PLACE FINGER");          // Ask the user to place a finger on the sensor

            CmdLCD(0xC0);                    // Move cursor to the second row

            u32LCD(search_fp());             // Search the fingerprint and display the matched ID

            delay_ms(1000);                  // Wait for 1 second

            break;                           // Exit this case

        case '3':                            // Option 3: Delete a fingerprint

            CmdLCD(0x01);                    // Clear the LCD

            CmdLCD(0x80);                    // Move cursor to the first row

            StrLCD("Enter ID: ");            // Ask the user to enter the fingerprint ID

            pageid = ReadNum();              // Read the fingerprint ID

            CmdLCD(0xC0);                    // Move cursor to the second row

            u32LCD(pageid);                  // Display the entered fingerprint ID

            delay_ms(100);                   // Small delay

            if(pageid > 0 && pageid < 10)    // Check whether the entered ID is valid
            {
                if(delete_fp(pageid) == 0x00) // Delete the fingerprint and check whether deletion is successful
                {
                    CmdLCD(0x01);            // Clear the LCD

                    CmdLCD(0x80);            // Move cursor to the first row

                    StrLCD("SELECTED ID DEL."); // Display successful deletion message

                    delay_ms(1000);          // Wait for 1 second
                }
                else                         // Executed if deletion fails
                {
                    CmdLCD(0x01);            // Clear the LCD

                    CmdLCD(0x80);            // Move cursor to the first row

                    StrLCD("Failed to del.."); // Display deletion failure message

                    delay_ms(1000);          // Wait for 1 second
                }
            }

            break;                           // Exit this case

        case '4':                            // Option 4: Delete all stored fingerprints

            if(deleteall_fp() == 0x00)       // Delete all fingerprints and check for success
            {
                CmdLCD(0x01);                // Clear the LCD

                CmdLCD(0x80);                // Move cursor to the first row

                StrLCD("DEL ALL SUCCUSS");   // Display successful delete-all message

                delay_ms(1000);              // Wait for 1 second
            }
            else                             // Executed if delete-all operation fails
            {
                CmdLCD(0x01);                // Clear the LCD

                CmdLCD(0x80);                // Move cursor to the first row

                StrLCD("Failed to del..");   // Display failure message

                delay_ms(1000);              // Wait for 1 second
            }

            break;                           // Exit this case

        default:                             // Executed for any invalid key
            return;                          // Exit the fingerprint menu and return to the caller
        }
    }
}
void edit_pass(void)
{
	i2c_eeprom_pagewrite(0x50,0x0100,"1111",4);   // Write the current/default password to EEPROM address 0x0100

	CmdLCD(CLEAR_LCD);                           // Clear the LCD display

	CmdLCD(GOTO_LINE1_POS0);                     // Move the cursor to the first row, first column

	StrLCD("Ent 4 dig cur Pass:");               // Prompt the user to enter the current password

	CmdLCD(GOTO_LINE2_POS0);                     // Move the cursor to the second row

	pas = Read(pas);                             // Read the current password entered from the keypad

	i2c_eeprom_seqread(0x50,0x0100,curpass,4);   // Read the stored password from EEPROM into curpass

	if(!(strcmp((const char*)pas,(const char*)curpas))) // Compare entered password with stored password
	{
		CmdLCD(CLEAR_LCD);                       // Clear the LCD display

		CmdLCD(GOTO_LINE1_POS0);                 // Move the cursor to the first row

		StrLCD("Ent 4 dig New pas:");            // Ask the user to enter a new password

		CmdLCD(GOTO_LINE2_POS0);                 // Move the cursor to the second row

		pas = Read(pas);                         // Read the new password from the keypad

		CmdLCD(CLEAR_LCD);                       // Clear the LCD display

		CmdLCD(GOTO_LINE1_POS0);                 // Move the cursor to the first row

		StrLCD("Ent 4 dig New pas agn:");        // Ask the user to re-enter the new password for confirmation

		CmdLCD(GOTO_LINE2_POS0);                 // Move the cursor to the second row

		newpas = Read(newpas);                   // Read the confirmation password

		if(!(strcmp((const char*)pas,(const char*)newpas))) // Compare the new password and confirmation password
		{
			i2c_eeprom_pagewrite(0x50,0x0100,newpas,4); // Store the new password into EEPROM

			CmdLCD(CLEAR_LCD);                   // Clear the LCD display

			StrLCD("pwd chg sucful");            // Display password changed successfully message

			delay_s(1);                          // Wait for 1 second

			return;                             // Exit the function after successful password update
		}
		else                                    // Executed if the confirmation password does not match
		{
			CmdLCD(CLEAR_LCD);                  // Clear the LCD display

			CmdLCD(GOTO_LINE1_POS0);            // Move the cursor to the first row

			StrLCD("Pwd chg fail");             // Display password change failed message

			delay_ms(300);                      // Wait for 300 milliseconds

			return;                             // Exit the function
		}
	}
	else                                        // Executed if the current password is incorrect
	{
		CmdLCD(CLEAR_LCD);                      // Clear the LCD display

		CmdLCD(GOTO_LINE1_POS0);                // Move the cursor to the first row

		StrLCD("Pwd chg fail");                 // Display password change failed message

		delay_ms(200);                          // Wait for 200 milliseconds

		return;                                 // Exit the function
	}
}
