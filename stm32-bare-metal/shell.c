/**
 ******************************************************************************
 * @file	shell.c
 * @author 	Arnaud C.
 * @brief	shell for bare-metal STM32
 ******************************************************************************
 */

#include "shell.h"

/* Types ---------------------------------------------------------------------*/

/**
 * shell_func_t : contains informations of a shell command
 */
typedef struct{
	char * cmd;
	int (* func)(int argc, char ** argv);
	char * description;
} shell_func_t;

/* End of types --------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/
#define SHELL_FUNC_LIST_MAX_SIZE 16
#define SHELL_CMD_MAX_SIZE 16
#define SHELL_ARGC_MAX 8
#define SHELL_BUFFER_SIZE 40
/* End of macros -------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
UART_HandleTypeDef* shell_huart = NULL;

char starting[] = "\r\n\r\n===== Shell =====\r\n";	// can be modified
char prompt[] = "@STM32 >> "; // can be modified

char c = 0; // character received
uint8_t pos = 0; // current buffer index
static char buf[SHELL_BUFFER_SIZE];
static char backspace[] = "\b \b";
uint8_t it_uart_rx_ready = 0;

static int shell_func_list_size = 0;
static shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE];

/* End of variables ----------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
 * __io_putchar : Required to printf() on uart
 * @param ch character to write on uart
 * @return 0 if HAL_OK
 */
uint8_t __io_putchar(int ch) {
	if(HAL_OK != HAL_UART_Transmit(shell_huart, (uint8_t *)&ch, 1, HAL_MAX_DELAY)){
		return 1;
	}
	return 0;
}

/**
 * uart_write : Write on uart
 * @param s string to write on uart
 * @param size length of the string
 * @return 0 if HAL_OK
 */
uint8_t uart_write(char *s, uint16_t size) {
	if(HAL_OK != HAL_UART_Transmit(shell_huart, (uint8_t*)s, size, 0xFFFF)){
		return 1;
	}
	return 0;
}

/**
 * sh_help : shell help menu
 * @param argc number of arguments of the command
 * @param argv arguments in strings array
 * @return 0
 */
uint8_t sh_help(int argc, char ** argv) {
	int i;
	for(i = 0 ; i < shell_func_list_size ; i++) {
		printf("%s : %s\r\n", shell_func_list[i].cmd, shell_func_list[i].description);
	}
	return 0;
}

/**
 * sh_example : shell example function
 * @param argc number of arguments of the command
 * @param argv arguments in strings array
 * @return 0
 */
uint8_t sh_example(int argc, char ** argv) {
	printf("argc = %d\r\n", argc);
	for (int i = 0 ; i < argc ; i++) {
		printf("arg nb %d = %s\r\n", i, argv[i]);
	}
	return 0;
}

/**
 *shell_init : shell initialization function
 * @param huart UART Handle to initialize
 * @return 0 if HAL_OK
 */
uint8_t shell_init(UART_HandleTypeDef* huart) {
	shell_huart = huart;

	uart_write(starting,sizeof(starting));
	uart_write(prompt,sizeof(prompt));

	shell_add("help", (void*)sh_help, (char *)"help");
	shell_add("f", (void*)sh_example, "Example command");

	if(HAL_OK != HAL_UART_Receive_IT(shell_huart, (uint8_t*)&c, 1)){
		return 1;
	}

	return 0;
}

/**
 * shell_add : add a command to the shell
 * @param cmd string related to the command
 * @param pfunc command function reference
 * @param description of the function displayed on shell help menu
 * @return 0 if number of commands is valid
 */
uint8_t shell_add(char * cmd, int (* pfunc)(int argc, char ** argv), char * description) {
	if (shell_func_list_size < SHELL_FUNC_LIST_MAX_SIZE) {
		shell_func_list[shell_func_list_size].cmd = cmd;
		shell_func_list[shell_func_list_size].func = pfunc;
		shell_func_list[shell_func_list_size].description = description;
		shell_func_list_size++;
		return 0;
	}

	return -1;
}

/**
 * shell_char_received : process the last character received
 * @return 0
 */
uint8_t shell_char_received() {

	switch (c) {
	case '\r':
		// ENTER key pressed
		printf("\r\n");
		buf[pos++] = 0;
		pos = 0;
		shell_exec(buf);
		uart_write(prompt,sizeof(prompt));
		break;

	case '\b':
		// DELETE key pressed
		if (pos > 0) {
			pos--;
			uart_write(backspace, 3);
		}
		break;

	default:
		if (pos < SHELL_BUFFER_SIZE) {
			uart_write(&c, 1);
			buf[pos++] = c;
		}
	}

	return 0;
}

/**
 * shell_exec : search and execute the command
 * @param cmd command to process
 * @return reference to the function
 */
uint8_t shell_exec(char * cmd) {
	int argc;
	char * argv[SHELL_ARGC_MAX];
	char *p;

	// Separating header and arguments
	char header[SHELL_CMD_MAX_SIZE] = "";
	int h = 0;

	while(cmd[h] != ' ' && h < SHELL_CMD_MAX_SIZE){
		header[h] = cmd[h];
		h++;
	}
	header[h] = '\0';

	// Searching for the command and parameters
	for(int i = 0 ; i < shell_func_list_size ; i++) {
		if (!strcmp(shell_func_list[i].cmd, header)) {
			argc = 1;
			argv[0] = cmd;

			for(p = cmd ; *p != '\0' && argc < SHELL_ARGC_MAX ; p++){
				if(*p == ' ') {
					*p = '\0';
					argv[argc++] = p+1;
				}
			}

			return shell_func_list[i].func(argc, argv);
		}
	}
	printf("%s: command not found\r\n", cmd);
	return -1;
}

/* End of functions ----------------------------------------------------------*/
