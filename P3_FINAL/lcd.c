#include "lcd.h"
#include "Arial12x12.h"
#include <string.h>

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

static TIM_HandleTypeDef htim7;
unsigned char buffer[512];

static  uint16_t positionL1 = 0;//Pagina 0 
static  uint16_t positionL2 = 256;//lo pongo desde donde empieza, Pagina 2
	
void write_lcd(char cadena[], int linea, int longitudCadena){ //esta funcion no te la dans
  int i=0;
  
  //longitudCadena = strlen(cadena) -> te dice cuanto mide
  for (i=0;i<longitudCadena; i++){
    symbolToLocalBuffer(linea,cadena[i]);
  }
  LCD_update();
}

void lcd_borrar(void){
	int i = 0;
  for (i=0;i<512; i++){
    buffer[i]=0x00;
  }
}

void LCD_write_line(char cadena[], int linea){ //esta funcion no te la dans
  int i=0;
  int longitudCadena = 0;
		
	positionL1 = 0;//Pagina 0 
  positionL2 = 256;//lo pongo desde donde empieza, Pagina 2
  
  longitudCadena = strlen(cadena);
  for (i=0;i<longitudCadena; i++){
    symbolToLocalBuffer(linea,cadena[i]);
  }
	LCD_update();
}

void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
  uint8_t i, value1, value2;
  uint16_t offset=0;
//  static  uint16_t positionL1 = 0;//Pagina 0 
//  static  uint16_t positionL2 = 256;//lo pongo desde donde empieza, Pagina 2
  uint16_t position = 0;
  
  position = (line == 1) ? positionL1 : positionL2;//ternaria -> igual que un if
  
/*  if (line == 1){
    position = positionL1;
  }else{
    position = positionL2;
  }*/
  
  offset=25*(symbol - ' ');
  
  for (i = 0; i<12; i++){
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
    
    buffer[i+position]=value1;
    buffer[i+128+position]=value2;
  }
  
  if (line == 1){
    positionL1=positionL1+Arial12x12[offset];
  }else{
    positionL2=positionL2+Arial12x12[offset];
  }
}
void symbolToLocalBuffer_L1(uint8_t symbol){//caracter assci
  uint8_t i, value1, value2;
  uint16_t offset=0;
  //static  uint16_t positionL1 = 0;//importante static
  
  offset=25*(symbol - ' ');/*Lo multiplica por 25 porque hay 25 columnas en el Arial
  se empieza desde el espacio osea desde el 32 pero restando el espacio tiene el valor 32
  entonces hace que se empiece en el byte 0*/
  /*Te dice en que fila estás de tu array Arial12x12*/
  
  for (i = 0; i<12; i++){//El offset te da la posición, ocupa como máximo 12 columnas
    value1=Arial12x12[offset+i*2+1];/*Se suma 1 porque tiene que empezar a pintar desde la 1ª columna
    y lo multiplica por dos porque cada dos columnas del Arial muestra lo que pinta en la pagina 0*/
    value2=Arial12x12[offset+i*2+2];/*Pagina 1, que valor tiene el byte (de columna)
    define el valor de tu array Arial12x12*/
    
    buffer[i+positionL1]=value1;//Almacena en un buffer donde va a pintar en la pagina 0 y que byte de la columna
    buffer[i+128+positionL1]=value2;//Pagina 1, define la posición de la pantalla
  }
  positionL1=positionL1+Arial12x12[offset];//Lo que ha consumido yo
  //offset= lo que ocupa la letra (espacio)(1ª Columna del Arial)Ej = 0x02 ocupa 2 columnas de la página
}

void symbolToLocalBuffer_L2(uint8_t symbol){//caracter assci
  uint8_t i, value1, value2;
  uint16_t offset=0;
  //static  uint16_t positionL2 = 0;//importante static
  
  offset=25*(symbol - ' ');/*Lo multiplica por 25 porque hay 25 columnas en el Arial
  se empieza desde el espacio osea desde el 32 pero restando el espacio tiene el valor 32
  entonces hace que se empiece en el byte 0*/
  /*Te dice en que fila estás de tu array Arial12x12*/
  
  for (i = 0; i<12; i++){//El offset te da la posición, ocupa como máximo 12 columnas
    value1=Arial12x12[offset+i*2+1];/*Se suma 1 porque tiene que empezar a pintar desde la 1ª columna
    y lo multiplica por dos porque cada dos columnas del Arial muestra lo que pinta en la pagina 0*/
    value2=Arial12x12[offset+i*2+2];/*Pagina 1, que valor tiene el byte (de columna)
    define el valor de tu array Arial12x12*/
    
    buffer[i+256+positionL2]=value1;//Almacena en un buffer donde va a pintar en la pagina 2 y que byte de la columna
    buffer[i+384+positionL2]=value2;//Pagina 3, define la posición de la pantalla
  }
  positionL2=positionL2+Arial12x12[offset];//Lo que ha consumido yo
  //offset= lo que ocupa la letra (espacio)(1ª Columna del Arial)Ej = 0x02 ocupa 2 columnas de la página
}

void startLCD(void){
  
  LCD_reset();
  LCD_Init();
  
}

void LCD_Init(void){
  LCD_wr_cmd(0xAE); //Display off (1010(A) 1110(E))
  LCD_wr_cmd(0xA2); //Fija el valor de la relación de la tensión de polarización del LCD a 1/9
  LCD_wr_cmd(0xA0); //El direccionamiento de la RAM de datos del display es la normal
  LCD_wr_cmd(0xC8); //El scan en las salidas COM es el normal
  LCD_wr_cmd(0x22); //Fija la relación de resistencias interna a 2
  LCD_wr_cmd(0x2F); //Power on
  
  LCD_wr_cmd(0x40); //Display empieza en la línea 0
  LCD_wr_cmd(0xAF); //Display ON
  LCD_wr_cmd(0x81); //Contraste
  LCD_wr_cmd(0x15); //Valor Contraste o 0x17
  LCD_wr_cmd(0xA4); //Display all points normal -- Pantalla reversa 0xA7
  LCD_wr_cmd(0xA6); //LCD Display normal
}

void LCD_update_L1(void){ //actualiza todas las páginas
  int i;
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB0); // Página 0

  for(i=0;i<128;i++){
  LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB1); // Página 1

  for(i=128;i<256;i++){
  LCD_wr_data(buffer[i]);
  }
}

void LCD_update_L2(void){ //actualiza todas las páginas
  int i;
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB0); // Página 0

  for(i=0;i<128;i++){
  LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB1); // Página 1

  for(i=128;i<256;i++){
  LCD_wr_data(buffer[i]);
  }
}

void LCD_update(void){ //actualiza todas las páginas
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
 
 for(i=256;i<384;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3

 for(i=384;i<512;i++){
 LCD_wr_data(buffer[i]);
 }
}

void LCD_reset(void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  //INICIALIZACIÓN Y CONFIGURACIÓN DRIVER SPI PARA LCD
  SPIdrv->Initialize(NULL);
  SPIdrv->PowerControl(ARM_POWER_FULL); //Le pasas un estado, activa perifericos, interrupciones, se puede llamar muchas veces
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8) , 20000000);

  //INICIALIZAR LOS PINES NECESARIOS (EL MOSI Y SCK YA ESTAN EN RTE_Device)
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  
  //GPIO_InitStruct.Pin = GPIO_PIN_5;//LCD_SCK
  //HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  //GPIO_InitStruct.Pin = GPIO_PIN_7;//LCD_MOSI
  //HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_6;//LCD_RESET
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;//LCD_A0
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_14;//LCD_CS_N
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
  delay(1);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
  delay(1000);
}

void delay(uint32_t n_microsegundos){
  //TIENE RELOJ 42MHz
  __HAL_RCC_TIM7_CLK_ENABLE();
  htim7.Instance = TIM7; //APB1
  htim7.Init.Prescaler = 41;//1 us por cuenta
  htim7.Init.Period = (n_microsegundos - 1);
  
  HAL_TIM_Base_Init(&htim7); //Configurar timer
  HAL_TIM_Base_Start(&htim7); //Start timer basico

  while(!__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)){ //HASTA QUE NO SEA TRUE NO SALE DEL WHILE
  }//Cuando este el flag update sale del while
  
  __HAL_TIM_CLEAR_FLAG(&htim7,TIM_FLAG_UPDATE);
  
  HAL_TIM_Base_Stop(&htim7);
  HAL_TIM_Base_DeInit(&htim7);
}

void LCD_wr_data(unsigned char data){ // Función que escribe un dato en el LCD.
  
 // Seleccionar CS = 0;//Elige que esclavo va usar o que comunicacion SPI usa
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
 // Seleccionar A0 = 1;//Para escribir dato
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);

 // Escribir un dato (data) usando la función SPIDrv->Send(…);
  SPIdrv->Send(&data, sizeof(data));
  
 // Esperar a que se libere el bus SPI;
  while (SPIdrv->GetStatus().busy == 1){
  }
  
 // Seleccionar CS = 1;
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);

}

void LCD_wr_cmd(unsigned char cmd){ // Función que escribe un comando en el LCD.

 // Seleccionar CS = 0;
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
 // Seleccionar A0 = 0;
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET);

 // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
 SPIdrv->Send(&cmd, sizeof(cmd));
  
 // Esperar a que se libere el bus SPI;
  while (SPIdrv->GetStatus().busy == 1){
  }
 // Seleccionar CS = 1;
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
}
