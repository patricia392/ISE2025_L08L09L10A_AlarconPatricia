#ifndef __LCD_H
#define __LCD_H

#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"

void LCD_reset(void);
void delay(uint32_t n_microsegundos);
void LCD_Init(void);
void LCD_update(void);
void startLCD(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void symbolToLocalBuffer(uint8_t line, uint8_t symbol);
void symbolToLocalBuffer_L1(uint8_t symbol);
void symbolToLocalBuffer_L2(uint8_t symbol);
void LCD_update_L1(void);
void LCD_update_L2(void);
void write_lcd(char cadena[], int linea, int longitudCadena);
void LCD_write_line(char cadena[], int linea);
void lcd_borrar(void);

extern unsigned char buffer[512];

#endif /* __LCD_H */
