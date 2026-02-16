/*
 * File:   main.c
 * Author: Rayen
 *
 * Created on 3 juillet 2025, 19:55
 */
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 4000000

#include "LCD_I2C.h"
#include <xc.h>

unsigned char RS, i2c_add, BackLight_State = LCD_BACKLIGHT;

void main(void) {
 
  I2C_Init();
  LCD_Init(0x4E); // initialize LCD module with I2C address 0x4E
 
  LCD_Set_Cursor(1, 1);
  // write anything 
  LCD_Write_String(" Rayen Zahzeh");
 
  while(1)
  {
    LCD_shift_right();
    __delay_ms(150);
     LCD_shift_right();
    __delay_ms(150);
    LCD_shift_left();
    __delay_ms(150);
   LCD_shift_left();
    __delay_ms(150);
  } 
  return;
}
 
 // config
void I2C_Init()
{
  SSPCON = 0x28;
  SSPCON2 = 0x00;
  SSPSTAT = 0x00;
  SSPADD = ((_XTAL_FREQ/4)/I2C_BaudRate) - 1;
  SCL_D = 1;
  SDA_D = 1;
}
 
void I2C_Wait()
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}
 
void I2C_Start()
{
  I2C_Wait();
  SEN = 1;
}
 
void I2C_RepeatedStart()
{
  I2C_Wait();
  RSEN = 1;
}
 
void I2C_Stop()
{
  I2C_Wait();
  PEN = 1;
}
 
void I2C_ACK(void)
{
  ACKDT = 0; // ACK
  I2C_Wait();
  ACKEN = 1; // send ACK
}
 
void I2C_NACK(void)
{
  ACKDT = 1; // NACK
  I2C_Wait();
  ACKEN = 1; // send NACK
}
 
unsigned char I2C_Write(unsigned char data)
{
  I2C_Wait();
  SSPBUF = data;
  while(!SSPIF);
  SSPIF = 0;
  return ACKSTAT;
}
 
unsigned char I2C_Read_Byte(void)
{
 
  I2C_Wait();
  RCEN = 1;
  while(!SSPIF); 
  SSPIF = 0; 
  I2C_Wait();
  return SSPBUF; 
}

//LCD
 
void LCD_Init(unsigned char I2C_Add)
{
  i2c_add = I2C_Add;
  lcd_direct_write(0x00);
  __delay_ms(30);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(LCD_RETURN_HOME);
  __delay_ms(5);
  LCD_CMD(0x20 | (LCD_TYPE << 2));
  __delay_ms(50);
  LCD_CMD(LCD_TURN_ON);
  __delay_ms(50);
  LCD_CMD(LCD_CLEAR);
  __delay_ms(50);
  LCD_CMD(LCD_ENTRY_MODE_SET | LCD_RETURN_HOME);
  __delay_ms(50);
}
 
void lcd_direct_write(unsigned char Data)
{
  I2C_Start();
  I2C_Write(i2c_add);
  I2C_Write(Data );
  I2C_Stop();
}
 
void LCD_Write_4Bit(unsigned char nibble)
{
  // get the RS value to LSB OF Data
  nibble |= RS;
  lcd_direct_write(nibble | 0x04);
  lcd_direct_write(nibble & 0xFB);
  __delay_us(50);
}
 
void LCD_CMD(unsigned char CMD)
{
  RS = 0; // command register select
  LCD_Write_4Bit(CMD & 0xF0);
  LCD_Write_4Bit((CMD << 4) & 0xF0);
}
 
void LCD_Write_Char(char Data)
{
  RS = 1; // data Register select
  LCD_Write_4Bit(Data & 0xF0);
  LCD_Write_4Bit((Data << 4) & 0xF0);
}
 
void LCD_Write_String(char* Str)
{
  for(int i=0; Str[i]!='\0'; i++)
    LCD_Write_Char(Str[i]);
}
 
void LCD_Set_Cursor(unsigned char ROW, unsigned char COL)
{
  switch(ROW) 
  {
    case 2:
      LCD_CMD(0xC0 + COL-1);
      break;
    case 3:
      LCD_CMD(0x94 + COL-1);
      break;
    case 4:
      LCD_CMD(0xD4 + COL-1);
      break;
    // Case 1
    default:
      LCD_CMD(0x80 + COL-1);
  }
}
 
void Backlight()
{
  BackLight_State = LCD_BACKLIGHT;
  lcd_direct_write(0);
}
 
void noBacklight()
{
  BackLight_State = LCD_NOBACKLIGHT;
  lcd_direct_write(0);
}
 
void LCD_Shift_left()
{
  LCD_CMD(0x18);
  __delay_us(40);
}
 
void  LCD_Shift_right()
{
  LCD_CMD(0x1C);
  __delay_us(40);
}
 
void LCD_Clear()
{
  LCD_CMD(0x01);
  __delay_us(40);

}
