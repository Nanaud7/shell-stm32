# Shell for STM32 targets

The following tutorial has been made for Nucleo targets with STM32CubeIDE, but you can easily refer to it for other configurations.

- [Shell bare-metal](#Shell-bare-metal)
- [Shell FreeRTOS](#Shell-FreeRTOS)

## Shell bare-metal

1. Open the .ioc file of your project and configure it:
- In *Project Manager/Code Generator*, add *Generate peripheral initialization as a pair of '.c/.h' files per peripheral*.
- In *Connectivity*, clic on your UART/USART used with the Nucleo USB cable. In *NVIC Settings*, enable *global interrupt*.

2. Add *shell.c* and *shell.h* to your project. Include *shell.h* in *main.c*.

3. Before the infinite loop, add the shell initialization function with the address of the UART handler.
```c
shell_init(&huart2);
  ```

4. Add a new command with the function *shell_add()*.
```c
shell_add("button", (void*)toggle_button, "Shell button example");
  ```

5. Add the function *HAL_UART_RxCpltCallback()* and complete it with following lines:
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	if(huart->Instance == USART2){
		shell_char_received();
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&c, 1);
	}
}
  ```

## Shell FreeRTOS

1. Open the .ioc file of your project and configure it:
- In *Project Manager/Code Generator*, add *Generate peripheral initialization as a pair of '.c/.h' files per peripheral*.
- In *System Core/SYS*, add TIM6 as the *Timebase Source*.
- In *Middleware/FREERTOS*, enable FreeRTOS with the interface *CMSIS_V1*.
- In *Middleware/FREERTOS/Advanced settings*, enable *USE_NEWLIB_REENTRANT*.
- In *Connectivity*, clic on your UART/USART used with the Nucleo USB cable. In *NVIC Settings*, enable *global interrupt*.

2. Add *shell.c* and *shell.h* to your project. Include *shell.h* in *main.c*.

3. Create a function which will be the dedicated shell task. Call in it the shell initialization function and add your commands with *shell_add()*.
```c
void vTaskShell(void * p) {
	shell_init(&(UART_DEVICE));
	shell_add("button", (void*)toggle_button, "Shell button example");

	while(1){
		shell_char_received();
	}
}
  ```

4. Before the scheduler loop, call the FreeRTOS task creation function.
```c
xReturned = xTaskCreate(
	vTaskShell,      	/* Function that implements the task. */
	"Shell",         	/* Text name for the task. */
	STACK_SIZE,      	/* Stack size in words, not bytes. */
	(void *) NULL,    	/* Parameter passed into the task. */
	1,			/* Priority at which the task is created. */
	&xHandle);      	/* Used to pass out the created task's handle. */
if( xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ) {
	printf("Task Shell creation error: Could not allocate required memory\r\n");
}
  ```
  
 5. Add the function *HAL_UART_RxCpltCallback()* and complete it with following lines:
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){
	if(huart->Instance == USART2){
		xQueueSendFromISR(qShell, &c, NULL);
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&c, 1);
	}
}
  ```
