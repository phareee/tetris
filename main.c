/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int score = 0;
int gameover = 0;
int blocktype,blockrotate,block_x,block_y;//x=row y=column
int state = 0; //1==pause
uint8_t top[84] = 			"________________________________ ######  ######  ######  #####    ######   ###### \r\n";
uint8_t newgame[23][84] = {	"|                              |   ##    ##        ##    ##  ##     ##    ##      \r\n",
							"|                              |   ##    ######    ##    #####	    ##     #####  \r\n",
							"|                              |   ##    ##        ##    ##  ##     ##         ## \r\n",
							"|                              |   ##    ######    ##    ##   ##  ######  ######  \r\n",
							"|                              |.....PRESS >> a << to move block to the left......\r\n",
							"|                              |.PRESS >> s << to move block to bottom (quicker!).\r\n",
							"|                              |.....PRESS >> d << to move block to the right.....\r\n",
							"|                              |......PRESS >> space bar << to rotate block.......\r\n",
							"|                              |.....PRESS >>RESET BUTTON<< to start new game.....\r\n",
							"|                              |.....PUSH USER BUTTON to pause/resume playing.....\r\n",
							"|                              |..You will get 100 points from clearing one line..\r\n",
							"|                              |..If you can clear more than one line at a time...\r\n",
							"|                              |..........You will get the bonus points!..........\r\n",
							"|                              |--------------------------------------------------\r\n",
							"|                              |.....C U R R E N T S C O R E : 0 0 0 0 0 0 0......\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n",
							"|                              |..................................................\r\n"};
uint8_t bottom[82] = 		"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~..................................................";
uint8_t block[10][2][4] = {{"  X ","    "},
							{"XXXX","    "},
							{"   X","XXXX"},
							{" XX ","XX  "},
							{"XXX "," X  "},
							{"X   ","XXXX"},
							{"XX  ","    "},
							{" XX "," XX "},
							{"XXX ","    "},
							{"XX  "," XX "}};
uint8_t game_over[9][50]={" ####   ###   ## ##  #####                        ",
		"##     ## ## ## # ## ##                           ",
		"## ### ##### ## # ## #####                        ",
		"##  ## ## ## ##   ## ##                           ",
		" ####  ## ## ##   ## ##### ###  ## ## ##### ## ###",
		"                          ## ## ## ## ##    ###   ",
		"                          ## ## ## ## ##### ##    ",
		"                          ## ## ## ## ##    ##    ",
		"                           ###   ###  ##### ##    "};
void clear(){
	HAL_UART_Transmit(&huart2, "\033[2J", 7, 10);
	HAL_UART_Transmit(&huart2, "\033[H", 6, 10);
}

void print_game(uint8_t game[23][84]){
	int i =0;
	HAL_UART_Transmit(&huart2, top, sizeof(top), 10);
	for (i = 0; i < 23; i++) HAL_UART_Transmit(&huart2, game[i], sizeof(game[i]), 10);
	HAL_UART_Transmit(&huart2, bottom, sizeof(bottom), 10);
}

int can_call_new_block(){
	int r ;
	r = rand()%10;
	for(int j = 0;j<4;j++){
			if(newgame[0][14+j]=='#'&&block[r][0][j] == 'X') return -1;
			if(newgame[1][14+j]=='#'&&block[r][1][j] == 'X') return -1;
	}
	return r;
}
void call_new_block(){
	int b = can_call_new_block();
	if(b == -1) {
		gameover = 1;
	}
	else{
		for(int j = 0;j<4;j++){
			if(block[b][0][j] == 'X')newgame[0][14+j]='X';
			if(block[b][1][j] == 'X')newgame[1][14+j]='X';
		}
		blocktype = b;
		blockrotate = 4;
		block_x = 0;
		block_y = 14;
	}
}
void fall(int h){
	int i,j;
	for (i=h;i>0;i--){
		for(j = 1;j<31;j++) newgame[i][j] = newgame[i-1][j];
	}
	for(j = 1;j<31;j++) newgame[0][j] = ' ';
}
int check_line(int i){
	int l = 1;
		for(int j = 1;j<31;j++) {
			if (newgame[i][j] == ' ') l = 0;
		}
	return l;
}

void update_score(){
	int s = score;
	newgame[14][75] = '0' + (s%10); s/=10;
	newgame[14][73] = '0' + (s%10); s/=10;
	newgame[14][71] = '0' + (s%10); s/=10;
	newgame[14][69] = '0' + (s%10); s/=10;
	newgame[14][67] = '0' + (s%10); s/=10;
	newgame[14][65] = '0' + (s%10); s/=10;
	newgame[14][63] = '0' + (s%10);
}

void check(){
	int p = 100;
	int c =0;
	for(int m = 0;m<23;m++){
		if(check_line(m)) {
			fall(m);
			block_x++;
			score += p;
			p*=2;
			clear();
			update_score();
			print_game(newgame);
			HAL_Delay(250);
			c++;
		}
	}
	if(c) call_new_block();
}

int can_move_down(){
	for(int i = 22;i>=0;i--){
		for(int j = 1;j<31;j++){
			if (newgame[i][j]=='X' && (i+1)>22) return 0;
			else if (newgame[i][j]=='X' && newgame[i+1][j] == '#') return 0;
		}
	}
	return 1;
}

void move_down(){
	if (can_move_down()){
		for(int i = 22;i>=0;i--){
				for(int j = 1;j<31;j++){
					if (newgame[i][j] == 'X') {
						newgame[i+1][j] = 'X';
						newgame[i][j] = ' ';
					}
				}
		}
		block_x++;
	}
	else {
		for(int i = 22;i>=0;i--){
						for(int j = 1;j<31;j++){
							if (newgame[i][j] == 'X') {
								newgame[i][j] = '#';
							}
						}
				}

		call_new_block();
	}
}
int can_move_left(){
	for(int i = 22;i>=0;i--){
			for(int j = 1;j<31;j++){
				if (newgame[i][j]=='X' && (j-1)<1) return 0;
				else if (newgame[i][j]=='X' && newgame[i][j-1] == '#') return 0;
			}
		}
	return 1;
}
int can_move_right(){
	for(int i = 22;i>=0;i--){
			for(int j = 30;j>=0;j--){
				if (newgame[i][j]=='X' && (j+1)>30) return 0;
				else if (newgame[i][j]=='X' && newgame[i][j+1] == '#') return 0;
			}
		}
	return 1;
}

void move_left(){
	if(can_move_left()){
		for(int i = 22;i>=0;i--){
				for(int j = 1;j<31;j++){
					if (newgame[i][j] == 'X') {
						newgame[i][j-1] = 'X';
						newgame[i][j] = ' ';
					}
				}
		}
		block_y--;
	}
}

void move_right(){
	if (can_move_right()){
		for(int i = 22;i>=0;i--){
			for(int j = 30;j>=0;j--){
				if (newgame[i][j] == 'X') {
					newgame[i][j+1] = 'X';
					newgame[i][j] = ' ';
				}
			}
		}
		block_y++;
	}
}
int can_rotate(){
	if (blocktype == 0||blocktype == 7) return -1;
	else{
		if(blockrotate == 4){ //now4columns
			if(block_x+3>22) return-1;
			if(newgame[block_x][block_y] == 'X' && newgame[block_x+3][block_y]=='#') return -1;
			if(newgame[block_x+1][block_y] == 'X' && newgame[block_x+3][block_y+1]=='#') return -1;
			if(newgame[block_x][block_y+1] == 'X' && newgame[block_x+2][block_y]=='#') return -1;
			if(newgame[block_x+1][block_y+1] == 'X' && newgame[block_x+2][block_y+1]=='#') return -1;
			if(newgame[block_x][block_y+2] == 'X' && newgame[block_x+1][block_y]=='#') return -1;
			if(newgame[block_x+1][block_y+2] == 'X' && newgame[block_x+1][block_y+1]=='#') return -1;
			if(newgame[block_x][block_y+3] == 'X' && newgame[block_x][block_y]=='#') return -1;
			if(newgame[block_x+1][block_y+3] == 'X' && newgame[block_x][block_y+1]=='#') return -1;
		}
		else if (blockrotate == 2){ //now2columns
			if(block_y+3>30) return -1;
			if(newgame[block_x+3][block_y+1] == 'X' && newgame[block_x][block_y+3]=='#') return -1;
			if(newgame[block_x+3][block_y] == 'X' && newgame[block_x+1][block_y+3]=='#') return -1;
			if(newgame[block_x+2][block_y+1] == 'X' && newgame[block_x][block_y+2]=='#') return -1;
			if(newgame[block_x+2][block_y] == 'X' && newgame[block_x+1][block_y+2]=='#') return -1;
			if(newgame[block_x][block_y+1] == 'X' && newgame[block_x][block_y]=='#') return -1;
			if(newgame[block_x+1][block_y+1] == 'X' && newgame[block_x][block_y+1]=='#') return -1;
			if(newgame[block_x+1][block_y] == 'X' && newgame[block_x+1][block_y+1]=='#') return -1;
			if(newgame[block_x][block_y] == 'X' && newgame[block_x+1][block_y]=='#') return -1;
		}
	}
	return 1;
}
void rotate(){
	int c = can_rotate();
	if(c == 1){
		if(blockrotate == 4){
			if(newgame[block_x][block_y] == 'X'){
				newgame[block_x+3][block_y]='X';
				newgame[block_x][block_y] = ' ';
			}
			if(newgame[block_x+1][block_y] == 'X'){
				newgame[block_x+3][block_y+1]='X';
				newgame[block_x+1][block_y] = ' ';
			}
			if(newgame[block_x][block_y+1] == 'X'){
				newgame[block_x+2][block_y]='X';
				newgame[block_x][block_y+1] = ' ';
			}
			if(newgame[block_x+1][block_y+1] == 'X'){
				newgame[block_x+2][block_y+1]='X';
				newgame[block_x+1][block_y+1] = ' ';
			}
			if(newgame[block_x][block_y+2] == 'X'){
				newgame[block_x+1][block_y]='X';
				newgame[block_x][block_y+2] = ' ';
			}
			if(newgame[block_x+1][block_y+2] == 'X'){
				newgame[block_x+1][block_y+1] ='X';
				newgame[block_x+1][block_y+2] = ' ';
			}
			if(newgame[block_x][block_y+3] == 'X'){
				newgame[block_x][block_y]='X';
				newgame[block_x][block_y+3] = ' ';
			}
			if(newgame[block_x+1][block_y+3] == 'X'){
				newgame[block_x][block_y+1]='X';
				newgame[block_x+1][block_y+3] = ' ';
			}
			blockrotate = 2;
		}
		else if(blockrotate == 2){
			if(newgame[block_x+3][block_y+1] == 'X'){
				newgame[block_x][block_y+3] ='X';
				newgame[block_x+3][block_y+1] = ' ';
			}
			if(newgame[block_x+3][block_y] == 'X'){
				newgame[block_x+1][block_y+3]='X';
				newgame[block_x+3][block_y] = ' ';
			}
			if(newgame[block_x+2][block_y+1] == 'X'){
				newgame[block_x][block_y+2]='X';
				newgame[block_x+2][block_y+1] = ' ';
			}
			if(newgame[block_x+2][block_y] == 'X'){
				newgame[block_x+1][block_y+2]='X';
				newgame[block_x+2][block_y] = ' ';
			}
			uint8_t temp;
			if (newgame[block_x][block_y] == 'X'){
				temp = newgame[block_x][block_y];
				newgame[block_x][block_y] = ' ';
			}
			if(newgame[block_x][block_y+1] == 'X'){
				newgame[block_x][block_y]='X';
				newgame[block_x][block_y+1] = ' ';
			}
			if(newgame[block_x+1][block_y+1] == 'X'){
				newgame[block_x][block_y+1] = 'X';
				newgame[block_x+1][block_y+1] = ' ';
			}
			if(newgame[block_x+1][block_y] == 'X'){
				newgame[block_x+1][block_y+1] = 'X';
				newgame[block_x+1][block_y] = ' ';
			}
			if(temp == 'X'){
				newgame[block_x+1][block_y]= temp;
			}
			blockrotate = 4;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */


int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/


	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
/* USER CODE END WHILE */
  call_new_block();
  print_game(newgame);
  while (1){
	  uint8_t t[1];
	  if(state == 1)continue;

	 if(!gameover){
		 while(HAL_UART_Receive(&huart2, t, 1, 5)==HAL_OK){
		  	  switch ((char)t[0]) {
		  	  		  	case 's':
		  	  			move_down();
		  	  			break;
		  	  		  	case 'a':
		  	  		 	move_left();
		  	  		 	break;
		  	  		  	case 'd' :
		  	  		  	move_right();
		  	  		  	break;
		  	  		  	case ' ':
		  	  		  	rotate();
		  	  		  	break;
		  	  		  	default:
		  	  		  	break;
		  	  		  	}
		 }
	  		  check();
	  		  move_down();
	  		  clear();
	  		  print_game(newgame);
	  		  HAL_Delay(350);
	  		if(gameover){
	  			for(int a =0;a<50;a++){
	  				newgame[15][32+a]=game_over[0][a];
	  				newgame[16][32+a]=game_over[1][a];
	  				newgame[17][32+a]=game_over[2][a];
	  				newgame[18][32+a]=game_over[3][a];
	  				newgame[19][32+a]=game_over[4][a];
	  				newgame[20][32+a]=game_over[5][a];
	  				newgame[21][32+a]=game_over[6][a];
	  				newgame[22][32+a]=game_over[7][a];
	  				bottom[32+a]=game_over[8][a];
	  			}
	  			clear();
	  			print_game(newgame);
	  		}
	  	  }
  }

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (state == 0) state = 1;
	else state = 0;

}
static void MX_USART2_UART_Init(void){

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
