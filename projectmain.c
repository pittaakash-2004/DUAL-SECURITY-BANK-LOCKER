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
	InitUART0();
	init_i2c();
	InitLCD();
	Init_KPM();
	Enable_EINT2();
	StrLCD("Welcome to Bank");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("   Akash   ");
	delay_s(1);
	i2c_eeprom_pagewrite(0x50,0x0010,"1111",4);
	delay_ms(100);
	i2c_eeprom_seqread(0x50,0x0010,ruserid,4);
	StrLCD(ruserid);
	delay_ms(500);

	i2c_eeprom_pagewrite(0x50,0x0100,"1234",4);
	i2c_eeprom_seqread(0x50,0x0100,rpwd,4);
	while(1)
	{
		LABEL1:CmdLCD(CLEAR_LCD);
		StrLCD("Enter 4 digit id:");
		delay_s(1);
		while(ColScan()==1)
		{
			if(a==1)
			{
				menu();
				goto LABEL1;
			}
		}
		CmdLCD(GOTO_LINE2_POS0);
		usrid=Read(userid);
//		StrLCD(usrid);
//		delay_ms(2000);
	//	delay_ms(1000);
	//	CmdLCD(CLEAR_LCD);
		i2c_eeprom_seqread(0x50,0x0010,ruserid,4);
		if((mystrcmp((const char*)usrid,(const char*)ruserid))==0)
		{
			i2c_eeprom_seqread(0x50,0x0100,rpwd,4);
			CmdLCD(CLEAR_LCD);
//				CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Enter 4 digit pwd:");
			CmdLCD(GOTO_LINE2_POS0);
			pd=Read(pwd);
			CmdLCD(GOTO_LINE2_POS0);
//			CmdLCD(CLEAR_LCD);
//			StrLCD(rpwd);
//			delay_ms(1000);
		//i2c_eeprom_seqread(0x50,0x000F,ruserid,4);
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD(pd);
			delay_ms(1000);
			
			if((mystrcmp((const char*)pd,(const char*)rpwd))==0)
	 		{
				CmdLCD(0x01);
	   			CmdLCD(0x80);
          		if(search_fp())
				{
					CmdLCD(CLEAR_LCD);
					CmdLCD(GOTO_LINE1_POS0);
					StrLCD("BANK LOCK OPEN");
					do
					{
					 	if(flag==0)
							motor_open();
					}while(READBIT(IOPIN0,SW_AL)==1);
					motor_close();
				} 
				delay_ms(1000);
			}
			else
			{
				CmdLCD(CLEAR_LCD);
				CmdLCD(GOTO_LINE1_POS0);
				StrLCD("pwd incorrect");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("Wait & Enter agn");
				delay_s(1);
			}
		}
		else
		{
			CmdLCD(CLEAR_LCD);
			CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Usr id incorrect");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("Wait & Enter agn");
			delay_s(1);
		}
	}
}


void menu()
{
	InitUART0();
	a=0;
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("MENU: ");
	StrLCD("1)Edit pass");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("2)Edit FP 3)EXIT");
	if(KeyScan()=='1')
	{
		edit_pass();
		return;
	}	
	if(KeyScan()=='2')
	{
		edit_fp();
	}
	return;
}
void edit_fp(void)
{
     
   
    InitLCD();
    InitUART0();
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("FINGER PRINT ");

    delay_ms(2000);

    while (1)
    {
	   	CmdLCD(0x01);
	   	CmdLCD(0x80);
	   	StrLCD("1.ENROLL 3.DEL ");		
		CmdLCD(0xc0);
		StrLCD("2.SEARCH 4.D.ALL");
		choose = KeyScan();
		switch(choose)
		{
		
		case '1':
			CmdLCD(0x01);
	   		CmdLCD(0x80);
	   		StrLCD("Enter ID: ");
			pageid = ReadNum();
	   		CmdLCD(0xC0);
	   		u32LCD(pageid);					
    		delay_ms(100);
			if(pageid>0 && pageid <10)
			{
				enroll(pageid);
			}
			break;
		case '2':
	   		CmdLCD(0x01);
	   		CmdLCD(0x80);
	   		StrLCD("PLACE FINGER");
         	CmdLCD(0xC0);
          	u32LCD(search_fp()); 
			delay_ms(1000); 
			break;
		case '3':
			CmdLCD(0x01);
	   		CmdLCD(0x80);
	   		StrLCD("Enter ID: ");
			pageid = ReadNum();
	   		CmdLCD(0xC0);
	   		u32LCD(pageid);					
    		delay_ms(100);
			if(pageid>0 && pageid <10)
			{
				if(delete_fp(pageid) == 0x00)
				{
					CmdLCD(0x01);
			   		CmdLCD(0x80);
			   		StrLCD("SELECTED ID DEL.");					
					delay_ms(1000);
				}
				else
				{
					CmdLCD(0x01);
			   		CmdLCD(0x80);
			   		StrLCD("Failed to del..");					
					delay_ms(1000);
				}
			}
			break;
		case '4':
			if(deleteall_fp() == 0x00)
			{
                CmdLCD(0x01);
                CmdLCD(0x80);
                StrLCD("DEL ALL SUCCUSS");					
				delay_ms(1000);	
			}
			else
			{
				CmdLCD(0x01);
			   	CmdLCD(0x80);
			   	StrLCD("Failed to del..");					
				delay_ms(1000);				
			}
			break;
		default: return;
		}
    }
}
void edit_pass(void)
{
	i2c_eeprom_pagewrite(0x50,0x0100,"1111",4);
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("Ent 4 dig cur Pass:");
	CmdLCD(GOTO_LINE2_POS0);
	pas=Read(pas);
	i2c_eeprom_seqread(0x50,0x0100,curpass,4);
	if(!(strcmp((const char*)pas,(const char*)curpas)))
	{
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD("Ent 4 dig New pas:");
		CmdLCD(GOTO_LINE2_POS0);
		pas=Read(pas);
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD("Ent 4 dig New pas agn:");
		CmdLCD(GOTO_LINE2_POS0);
		newpas=Read(newpas);
		if(!(strcmp((const char*)pas,(const char*)newpas)))
		{
			i2c_eeprom_pagewrite(0x50,0x0100,newpas,4);
			CmdLCD(CLEAR_LCD);
			StrLCD("pwd chg sucful");
			delay_s(1);
			return;
		}
		else
		{
			CmdLCD(CLEAR_LCD);
			CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Pwd chg fail");
			delay_ms(300);
			return;
		}
	}
	else
	{
		CmdLCD(CLEAR_LCD);
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD("Pwd chg fail");
		delay_ms(200);
		return;
	}
}
