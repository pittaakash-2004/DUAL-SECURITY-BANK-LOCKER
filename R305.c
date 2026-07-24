#include "uart0.h"     
#include "lcd.h"   
#include "lcd_defines.h"
#include "delay.h"  

// Variable declarations
unsigned char ret, j, buf[25];   // ret: stores return value, j: loop counter, buf: transmit buffer

// External variables declared in another file
extern unsigned char i, rec_buff[25];  // i: receive counter, rec_buff: UART receive buffer

// Function to send the "Collect Fingerprint Image" command
unsigned char collect(void)
{
    // Clear the first 12 bytes of the transmit buffer
    for(j = 0; j < 12; j++)
        buf[j] = 0x00;

    // ---------------- Packet Header ----------------
    buf[0] = 0xEF;      // Header byte 1 (MSB)
    buf[1] = 0x01;      // Header byte 2 (LSB)

    // ---------------- Device Address ----------------
    buf[2] = 0xFF;      // Address byte 1
    buf[3] = 0xFF;      // Address byte 2
    buf[4] = 0xFF;      // Address byte 3
    buf[5] = 0xFF;      // Address byte 4 (Default module address = 0xFFFFFFFF)

    // ---------------- Packet Identifier ----------------
    buf[6] = 0x01;      // Command packet identifier

    // ---------------- Packet Length ----------------
    buf[7] = 0x00;      // Length MSB
    buf[8] = 0x03;      // Length LSB (Instruction + Checksum = 3 bytes)

    // ---------------- Instruction Code ----------------
    buf[9] = 0x01;      // Command: Collect fingerprint image (GenImg)

    // ---------------- Checksum ----------------
    buf[10] = 0x00;     // Checksum MSB
    buf[11] = 0x05;     // Checksum LSB
                        // Checksum = 0x01 + 0x0003 + 0x01 = 0x0005

    // Reset receive counter before receiving response
    i = 0;

    // Send the 12-byte command packet through UART
    UART0_Str1(12);

    // Wait until 12 bytes of response are received
    while(i < 12);

    // Check the confirmation code in the received packet
    if(rec_buff[9] == 0x00)      // 0x00 = Command executed successfully
        return 0;                // Success
    else
        return 1;                // Failure
}

// Function to store the captured fingerprint image into CharBuffer1 or CharBuffer2
unsigned char store(unsigned char id)
{
    // Clear the first 13 bytes of the transmit buffer
    for(j = 0; j < 13; j++)
        buf[j] = 0x00;

    // ---------------- Packet Header ----------------
    buf[0] = 0xEF;      // Header byte 1 (MSB)
    buf[1] = 0x01;      // Header byte 2 (LSB)

    // ---------------- Device Address ----------------
    buf[2] = 0xFF;      // Address byte 1
    buf[3] = 0xFF;      // Address byte 2
    buf[4] = 0xFF;      // Address byte 3
    buf[5] = 0xFF;      // Address byte 4 (Default address = 0xFFFFFFFF)

    // ---------------- Packet Identifier ----------------
    buf[6] = 0x01;      // Command packet identifier

    // ---------------- Packet Length ----------------
    buf[7] = 0x00;      // Length MSB
    buf[8] = 0x04;      // Length LSB
                        // Length = Instruction + Parameter + Checksum = 4 bytes

    // ---------------- Instruction Code ----------------
    buf[9] = 0x02;      // Img2Tz command
                        // Converts the captured fingerprint image into a character file

    // ---------------- Parameter ----------------
    buf[10] = id;       // Buffer ID
                        // 0x01 = Store in CharBuffer1
                        // 0x02 = Store in CharBuffer2

    // ---------------- Checksum ----------------
    buf[11] = 0x00;     // Checksum MSB
    buf[12] = 0x07 + id;// Checksum LSB
                        // Checksum = Packet ID + Length + Instruction + Buffer ID
                        // = 0x01 + 0x0004 + 0x02 + id
                        // = 0x07 + id

    // Reset receive counter before receiving response
    i = 0;

    // Send the 13-byte command packet to the fingerprint sensor
    UART0_Str1(13);

    // Wait until 12 bytes of response are received
    while(i < 12);

    // Check the confirmation code returned by the sensor
    if(rec_buff[9] == 0x00)      // 0x00 = Character file generated successfully
        return 0;                // Success
    else
        return 1;                // Failure
}


// Function to store the generated fingerprint template into the sensor database
unsigned char gen_template(unsigned int pageid)
{
    // Clear the first 15 bytes of the transmit buffer
    for(j = 0; j < 15; j++)
        buf[j] = 0x00;

    // ---------------- Packet Header ----------------
    buf[0] = 0xEF;      // Header byte 1 (MSB)
    buf[1] = 0x01;      // Header byte 2 (LSB)

    // ---------------- Device Address ----------------
    buf[2] = 0xFF;      // Address byte 1
    buf[3] = 0xFF;      // Address byte 2
    buf[4] = 0xFF;      // Address byte 3
    buf[5] = 0xFF;      // Address byte 4 (Default address = 0xFFFFFFFF)

    // ---------------- Packet Identifier ----------------
    buf[6] = 0x01;      // Command packet identifier

    // ---------------- Packet Length ----------------
    buf[7] = 0x00;      // Length MSB
    buf[8] = 0x06;      // Length LSB
                        // Length = Instruction + Parameters + Checksum = 6 bytes

    // ---------------- Instruction Code ----------------
    buf[9] = 0x06;      // Store command
                        // Stores the fingerprint template from CharBuffer into Flash memory

    // ---------------- Parameter 1 ----------------
    buf[10] = 0x01;     // Buffer ID
                        // 0x01 = CharBuffer1
                        // Template from CharBuffer1 will be stored

    // ---------------- Parameter 2 ----------------
    buf[11] = pageid >> 8;   // Page ID MSB (High byte)

    // ---------------- Parameter 3 ----------------
    buf[12] = pageid;        // Page ID LSB (Low byte)
                             // Specifies the location where the template is stored

    // ---------------- Checksum ----------------
    j = 0x0E + pageid;       // Calculate checksum
                             // 0x01(Packet ID) + 0x0006(Length) +
                             // 0x06(Store) + 0x01(Buffer ID) + Page ID
                             // = 0x000E + Page ID

    buf[13] = j >> 8;        // Checksum MSB
    buf[14] = j;             // Checksum LSB

    // Reset receive counter before receiving response
    i = 0;

    // Send the 15-byte command packet through UART
    UART0_Str1(15);

    // Wait until 12-byte response is received
    while(i < 12);

    // Check the confirmation code from the sensor
    // NOTE: Your original code checks buf[9], which is incorrect.
    // It should check rec_buff[9].

    if(rec_buff[9] == 0x00)      // 0x00 = Template stored successfully
        return 0;                // Success
    else
        return 1;                // Failure
}

// Function to enroll a new fingerprint into the sensor memory
// pageid: Location in the fingerprint database where the template is stored
unsigned char enroll(unsigned int pageid)
{
    // Clear the LCD display
    CmdLCD(0x01);

    // Move the cursor to the first line, first position
    CmdLCD(0x80);

    // Ask the user to place their finger
    StrLCD("PLACE FINGER  ");

    // Wait for 2 seconds so the user can place the finger
    delay_ms(2000);

    // Capture the fingerprint image
    ret = collect();

    // Wait for the sensor to complete image capture
    delay_ms(1000);

    // Move the cursor to the first line
    CmdLCD(0x80);

    // Ask the user to remove the finger
    StrLCD("REMOVE FINGER ");

    // Wait for the user to remove the finger
    delay_ms(1500);

    // Clear the LCD
    CmdLCD(0x01);

    // Move the cursor to the first line
    CmdLCD(0x80);

    // Check whether the fingerprint image was captured successfully
    if(ret == 0)
    {
        // Convert the captured fingerprint image into a template
        // and store it in CharBuffer1
        ret = store(1);

        // Check whether the conversion was successful
        if(ret == 0)
        {
            // Ask the user to place the same finger again
            StrLCD("PLACE FINGER  ");

            // Wait for the user
            delay_ms(2000);

            // Capture the fingerprint image again
            ret = collect();

            // Wait for image capture
            delay_ms(1000);

            // Move cursor to first line
            CmdLCD(0x80);

            // Ask the user to remove the finger
            StrLCD("REMOVE FINGER ");

            // Wait for finger removal
            delay_ms(1500);

            // Move cursor to first line
            CmdLCD(0x80);

            // Check whether the second fingerprint image was captured
            if(ret == 0)
            {
                // Convert the second image into a template
                // and store it in CharBuffer2
                ret = store(2);

                // Merge CharBuffer1 and CharBuffer2
                // Generate the final fingerprint template
                // Store it into Flash memory at pageid
                ret = gen_template(pageid);

                // Check whether template generation and storage succeeded
                if(ret == 0)
                    return 0;      // Enrollment successful
                else
                    return 1;      // Template generation/storage failed
            }
            else
                return 1;          // Second fingerprint capture failed
        }
        else
            return 1;              // Failed to store first fingerprint template
    }
    else
        return 1;                  // First fingerprint capture failed
}

// Function to delete a fingerprint template from the sensor database
// pageid: ID of the fingerprint to be deleted
unsigned char delete_fp(unsigned int pageid)
{
    // Clear the first 16 bytes of the transmit buffer
    for(j = 0; j < 16; j++)
        buf[j] = 0x00;

    // ---------------- Packet Header ----------------
    buf[0] = 0xEF;      // Header byte 1 (MSB)
    buf[1] = 0x01;      // Header byte 2 (LSB)

    // ---------------- Device Address ----------------
    buf[2] = 0xFF;      // Address byte 1
    buf[3] = 0xFF;      // Address byte 2
    buf[4] = 0xFF;      // Address byte 3
    buf[5] = 0xFF;      // Address byte 4 (Default address = 0xFFFFFFFF)

    // ---------------- Packet Identifier ----------------
    buf[6] = 0x01;      // Command packet identifier

    // ---------------- Packet Length ----------------
    buf[7] = 0x00;      // Length MSB
    buf[8] = 0x07;      // Length LSB
                        // Length = Instruction + Parameters + Checksum = 7 bytes

    // ---------------- Instruction Code ----------------
    buf[9] = 0x0C;      // Delete Character File (Delete Template) command

    // ---------------- Parameter 1 ----------------
    buf[10] = pageid >> 8;   // Page ID MSB (High byte)

    // ---------------- Parameter 2 ----------------
    buf[11] = pageid;        // Page ID LSB (Low byte)
                             // Specifies the fingerprint ID to delete

    // ---------------- Parameter 3 ----------------
    buf[12] = 0x00;          // Number of templates MSB
                             // Delete count = 1 template

    // ---------------- Parameter 4 ----------------
    buf[13] = 0x01;          // Number of templates LSB
                             // Delete only one fingerprint template

    // ---------------- Checksum ----------------
    j = 0x15 + pageid;       // Calculate checksum
                             // = Packet ID + Length + Instruction
                             // + Page ID + Delete Count
                             // = 0x01 + 0x0007 + 0x0C + pageid + 0x0001
                             // = 0x0015 + pageid

    buf[14] = j >> 8;        // Checksum MSB
    buf[15] = j;             // Checksum LSB

    // Reset receive counter before receiving the response
    i = 0;

    // Send the 16-byte delete command packet through UART
    UART0_Str1(16);

    // Wait until the complete 12-byte response packet is received
    while(i < 12);

    // Check the confirmation code returned by the fingerprint sensor
    if(rec_buff[9] == 0x00)
        return 0;            // Fingerprint deleted successfully
    else
        return 1;            // Error while deleting the fingerprint
}
unsigned char deleteall_fp(void)
{
    for(j=0;j<12;j++)
        buf[j]=0x00;    
    
    buf[0]=0xEF; //MSB
    buf[1]=0x01;//LSB Header---2 BYTES
    
    buf[2]=0xff;
    buf[3]=0xff;
    buf[4]=0xff;
    buf[5]=0xff; //LSB DEVICE ADRESS----4 BYTES
    
     
    buf[6]=0x01;//Package identifier--1 BYTE
    
    
    buf[8]=0x03;//LSB Package length--2 BYTES
  
  
    buf[9]=0x0D; //Instruction code--1 BYTE  
    
       						
    buf[11]=0x11;//LSBC hecksum--2 BYTES  
    i=0;
    UART0_Str1(12);   
 
    while(i<12);
    if(rec_buff[9]==0x00)
        return 0;  //SUCCESSFULLY DELETED ALL
    else 
        return 1;  //ERROR IN DELETION

} 


unsigned int identify_fp(void)       
{    
    for(j=0;j<12;j++)
        buf[j]=0x00;    
    
    buf[0]=0xEF; //MSB
    buf[1]=0x01;//LSB Header---2 BYTES    
    buf[2]=0xff;
    buf[3]=0xff;
    buf[4]=0xff;
    buf[5]=0xff; //LSB DEVICE ADRESS----4 BYTES      
    buf[6]=0x01;//Package identifier--1 BYTE     
    buf[8]=0x08;//LSB Package length--2 BYTES  
    buf[9]=0x04; //Instruction code--1 BYTE     
    buf[10]=0x01;//BUFFER ID    
    buf[12]=0x00;//START PAGE ADDRESS--2 BYTES    
    buf[14]=0x0A;//PAGENUM---2 BYTES
    buf[16]=0x18;//LSBC hecksum--2 BYTES 
		i=0;
    UART0_Str1(17);   
 
    while(i<16);
    if(rec_buff[9]==0x00)
    {
        j=rec_buff[10];
        j=(j<<8)|rec_buff[11];
        return (j);  //SUCCESSFULLY IDENTIFIED FP
    }
    else 
        return 0;  //ERROR IN DELETION
}


// Function to search for a fingerprint in the sensor database
unsigned int search_fp()
{
    // Clear the LCD display
    CmdLCD(0x01);

    // Move cursor to the first line
    CmdLCD(0x80);

    // Ask the user to place a finger
    StrLCD("PLACE FINGER  ");

    // Wait for the user to place the finger
    delay_ms(2000);

    // Capture the fingerprint image
    ret = collect();

    // Wait for image capture to complete
    delay_ms(1000);

    // Move cursor to the first line
    CmdLCD(0x80);

    // Ask the user to remove the finger
    StrLCD("REMOVE FINGER");

    // Wait for finger removal
    delay_ms(1500);

    // Clear the LCD
    CmdLCD(0x01);

    // Move cursor to the first line
    CmdLCD(0x80);

    // Check whether fingerprint image was captured successfully
    if(ret == 0)
    {
        // Convert the captured image into a character file
        // Store it in CharBuffer1
        ret = store(1);

        // Check whether conversion was successful
        if(ret == 0)
        {
            // Compare the generated template with all templates
            // stored in the fingerprint sensor database
            ret = identify_fp();

            // Check whether a matching fingerprint was found
            if(ret < 1)
            {
                // Clear the LCD
                CmdLCD(CLEAR_LCD);

                // Move cursor to the first line
                CmdLCD(GOTO_LINE1_POS0);

                // Display fingerprint not available
                StrLCD("FP NOT AVL");

                // Display the message for 1 second
                delay_ms(1000);

                // Return 0 to indicate no matching fingerprint
                return 0;
            }
            else
            {
                // Matching fingerprint found
                // Return the matched Page ID
                return ret;
            }
        }
        else
        {
            // Failed to convert the fingerprint image into a template
            StrLCD("FAILED IN STORING");

            // Display the error message for 2 seconds
            delay_ms(2000);

            // Return failure
            return 0;
        }
    }
    else
    {
        // Failed to capture the fingerprint image
        StrLCD("FAILED IN IDENTIFTN");

        // Display the error message for 2 seconds
        delay_ms(2000);

        // Return failure
        return 0;
    }
}
