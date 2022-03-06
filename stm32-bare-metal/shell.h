/**
 ******************************************************************************
 * @file	shell.h
 * @author 	Arnaud C.
 * @brief	shell for bare-metal STM32
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC_SHELL_H_
#define INC_SHELL_H_

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "usart.h"
#include "gpio.h"

/* Exported types ------------------------------------------------------------*/
/* End of exported types -----------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/* End of exported macros ----------------------------------------------------*/

/* External variables --------------------------------------------------------*/
extern char c;
extern uint8_t it_uart_rx_ready;
/* End of external variables -------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
uint8_t uart_write(char * s, uint16_t size);
uint8_t shell_init(UART_HandleTypeDef* huart);
uint8_t shell_add(char * cmd, int (* pfunc)(int argc, char ** argv), char * description);
uint8_t shell_char_received();
uint8_t shell_exec(char * cmd);
/* End of exported functions -------------------------------------------------*/

#endif /* INC_SHELL_H_ */
