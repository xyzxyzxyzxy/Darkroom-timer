/*
 * Template class implementing the singleton pattern
 *
 * Maintains the state of the timer and implements its functions.
 *
 * */

#ifndef TIMER_H_
#define TIMER_H_
#include "main.h"
#include "Keypad.h"
#include "7seg.h"

#define DEBOUNCE_INTERVAL 8 //75ms of debouncing time
#define INACTIVITY_TIME 15
#define GPIO_RELAY_PORT GPIOB
#define GPIO_RELAY_PIN GPIO_PIN_9
#define GPIO_BUZZER_PORT GPIOA
#define GPIO_BUZZER_PIN GPIO_PIN_10

/*
 * Constructor is private, static functions call the private implementations over the single object instance
 * template arguments can be defined when static functions are called,
 * for simplicity a typedef can be used to state what template arguments one wants to use.
 *
 * Template arguments have to be two timer handle typedef, first is the "countdown" timer, second the "inactivity" timer.
 * First timer generates UE every tenth of a second, inactivity timer can be set to generate UE every one second or less.
 *
 * */
template <TIM_HandleTypeDef * htim, TIM_HandleTypeDef * htim_sleep>
class Timer {

	float time = 0;
	float prev_time = 0;
	float inactivity_t = 0;
	char key = '\0';
	bool pmode = false;
	bool isTimerActive = false;
	bool asleep = false;

	Timer() {}
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;

	/*
	 * Resets timer to its default state when it was created
	 * */
	void resetTimer() {
		if (isTimerActive) {
			HAL_TIM_Base_Stop_IT(htim);
			HAL_TIM_Base_Start_IT(htim_sleep);

			displayFlush();

			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			HAL_GPIO_TogglePin(GPIO_RELAY_PORT, GPIO_RELAY_PIN);
			HAL_Delay(1000);

			HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		}
		if (pmode) {
			displayFlush();

			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			HAL_GPIO_TogglePin(GPIO_RELAY_PORT, GPIO_RELAY_PIN);
			HAL_Delay(1000);

			HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		}

		time = 0;
		key = '\0';

		pmode = false;
		isTimerActive = false;
	}
	/*Returns a reference to the object instance*/
	static Timer& getTimer() {
		static Timer timer_instance;
		return timer_instance;
	}
	float getTimeImpl() {
		return time;
	}
	char getKeyImpl() {
		return key;
	}
	bool isPmodeActiveImpl() {
		return pmode;
	}
	bool IisSleeping() {
			return asleep;
		}
	void IsetSleepState(bool state) {
		asleep = state;
	}
	/*
	 * Updates time of inactivity, if
	 * INACTIVITY_TIME threshold is reached then the MCU is put in stopMode
	 * */
	bool IinactivityTimeUpdate() {
		inactivity_t++;
		if (inactivity_t == INACTIVITY_TIME) {
			inactivity_t = 0;
			return true;
		}
		return false;
	}
	/*
	 * Updates current time based on previously inserted time and current digit
	 *
	 * Current time is then written to the display
	 *
	 * */
	void IdisplayTime() {

		int n = (int)(time * 10);

		if (n >= 1) {
			int ind = 0;
			int n_digits = log10(n) + 1;

			int d[4] = {0, 0, 0, 0};
			uint8_t k = 0;

			d[0] = n % 10;
			n /= 10;
			k++;
			while (n > 0) {
				d[k] = n % 10;
				n = n / 10;
				k++;
			}

			//write leading digits as off
			for (ind = 0; ind < 4-n_digits; ++ind) {
				HAL_Delay(2);
				(*writeDigit[ind])();
				print_OFF();
			}

			//write number digits
			for (int i = ind; i < 4; ++i) {
				HAL_Delay(2);
				(*writeDigit[i])();
				if (i == 2) {
					print_decimal();
					HAL_Delay(2);
				}
				switch (d[3-i]) {
				case 0:
					print_0();
					break;
				case 1:
					print_1();
					break;
				case 2:
					print_2();
					break;
				case 3:
					print_3();
					break;
				case 4:
					print_4();
					break;
				case 5:
					print_5();
					break;
				case 6:
					print_6();
					break;
				case 7:
					print_7();
					break;
				case 8:
					print_8();
					break;
				case 9:
					print_9();
					break;
				default:
					break;
				}
			}
		} else { //Number is < 1 log10 does not work.
			for (int i = 0; i < 3; ++i) {
				HAL_Delay(2);
				(*writeDigit[i])();
				print_OFF();
			}
		}
	}
	/*
	 * Writes a character to the display, depending on what function key was pressed
	 *
	 * */
	void IdisplayFunction() {
		if ((int)key >= 65 && (int)key <= 68) {
			for (int i = 0; i < 2; ++i) {
				HAL_Delay(2);
				(*writeDigit[i])();
				print_OFF();
			}
			HAL_Delay(2);
			write_D4();
			switch(key) {
				case 'A':
					print_A();
					break;
				case 'B':
					print_b();
					break;
				case 'C':
					print_C();
					break;
				case 'D':
					print_d();
					break;
				default:
					break;
			}
		}
	}
	/*
	 * Writes dashes on the display
	 * */
	void IdisplayWait() {
		for (int i = 0; i < 4; ++i) {
			HAL_Delay(2);
			(*writeDigit[i])();
			print_wait();
		}
	}
	/*
	 * Called when serving a keypad interrupt, decodes input by the user.
	 * The pressed button column is first determined depending on which GPIO triggered the interrupt,
	 * the corrisponding row is determined looping over the rows of the specific column.
	 *
	 * */
	void IgetIn(uint16_t GPIO_Pin) {

		//reset inactivity timer
		inactivity_t = 0;

		bool valid_press = false;
		int rn = 1;
		int cn;

		//The display is cleared to avoid glitch effects when pressing buttons
		displayFlush();

		//Toggle buzzer pin on to produce a sound when a key is presseds
		HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);

		/*
		 * If the MCU is in stopMode the current input is ignored, and the MCU returns to the default RUN mode
		 *
		 * */
		if (asleep) goto exit;

		if (GPIO_Pin == GPIO_PIN_11) {
			cn = 1; //fixed DETERMINED BY GPIO_PIN
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			//Determine which row the key is in
			while(!HAL_GPIO_ReadPin(C1_PORT, C1_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}

			/*
			 * Debounce function is called to determine if the key press is a valid press or just a bounce
			 * input is accepted only if current press is a valid press, otherwise the input is ignored
			 *
			 * */
			for (int i = 0; i < 15; ++i) {
				if (debounce(HAL_GPIO_ReadPin(C1_PORT, C1_PIN))) {
					valid_press = true;
				}
				HAL_Delay(DEBOUNCE_INTERVAL);
			}

			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		else if (GPIO_Pin == GPIO_RELAY_PIN) {
			cn = 2;
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			while(!HAL_GPIO_ReadPin(C2_PORT, C2_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}

			for (int i = 0; i < 15; ++i) {
				if (debounce(HAL_GPIO_ReadPin(C2_PORT, C2_PIN))) {
					valid_press = true;
				}
				HAL_Delay(DEBOUNCE_INTERVAL);
			}

			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		else if (GPIO_Pin == GPIO_PIN_15) {
			cn = 3;
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			while(!HAL_GPIO_ReadPin(C3_PORT, C3_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}

			for (int i = 0; i < 15; ++i) {
				if (debounce(HAL_GPIO_ReadPin(C3_PORT, C3_PIN))) {
					valid_press = true;
				}
				HAL_Delay(DEBOUNCE_INTERVAL);
			}

			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		else if (GPIO_Pin == GPIO_PIN_13) {
			cn = 4;
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			while(!HAL_GPIO_ReadPin(C4_PORT, C4_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}

			for (int i = 0; i < 15; ++i) {
				if (debounce(HAL_GPIO_ReadPin(C4_PORT, C4_PIN))) {
					valid_press = true;
				}
				HAL_Delay(DEBOUNCE_INTERVAL);
			}

			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		if (valid_press) {
			/*
			 * The key corresponding to the row and column found is accessed through a matrix containing the symbols
			 * */
			key = keypad[rn][cn];
		}
		exit://Toggle buzzer pin off
		HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
	}
	/*
	 * Updates current time subtracting a tenth of a second to the time
	 * if time variable is less than or equal zero timer stops and relay opens circuit.
	 *
	 * */
	void IupdateTime() {
		time -= 0.1;
		if (time <= 0) {

			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

			HAL_GPIO_TogglePin(GPIO_RELAY_PORT, GPIO_RELAY_PIN);
			HAL_TIM_Base_Stop_IT(htim);
			isTimerActive = false;

			HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
			HAL_Delay(80);
			HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
			HAL_Delay(80);
			HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
			HAL_Delay(80);
			HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
			HAL_Delay(80);
			HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);
			HAL_Delay(80);
			HAL_GPIO_TogglePin(GPIO_BUZZER_PORT, GPIO_BUZZER_PIN);


			//End is displayed for about 1s
			for (int i = 0; i < 128; ++i) {
				write_D1();
				print_OFF();
				HAL_Delay(2);
				write_D2();
				print_E();
				HAL_Delay(2);
				write_D3();
				print_n();
				HAL_Delay(2);
				write_D4();
				print_d();
				HAL_Delay(2);
			}

			displayFlush();

			HAL_Delay(1000);

			HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
			HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

			HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);


			HAL_TIM_Base_Start_IT(htim_sleep); //Restart sleep timer
			time = 0;
		}
	}
	/*
	 * Check what to do depending on the current input by the user
	 *
	 * */
	void executeImpl() {
		//IF ZERO NOTHING TO DO
		if (key != '\0') {

			if (key == 'B') {
				resetTimer();
				return;
			}

			if(isTimerActive) return; //DO NOTHING IF TIMER IS CURRENTLY ACTIVE

			//a DIGIT
			if ((int)key <= 58 && (int)key >= 48) {
				  if (pmode == true) {
					  pmode = false;
					  displayFlush();
					  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
					  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
					  HAL_GPIO_TogglePin(GPIO_RELAY_PORT, GPIO_RELAY_PIN);
					  HAL_Delay(1000);

					  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
					  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
					  HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
					  HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
				  }
				  time = time * 10 + (int)key - 48;
				  if (time > 999) {
					  time = (int)key-48;
				  }
			//a FUNCTION KEY
			} else {
				  if (key == '*') {
					  //POSITIONING MODE ON
					  time = 0;
					  pmode = !pmode;
					  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
					  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
					  HAL_GPIO_TogglePin(GPIO_RELAY_PORT, GPIO_RELAY_PIN);
					  displayFlush();
					  HAL_Delay(1000);
					  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
					  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
				  } else {
					  switch(key) {
						  case 'C':
							  if (time == 0) {
								  time = prev_time;
							  } else {
								  startTimer();
							  }
							  break;
						  case 'D':
							  deleteDigit();
							  break;
						  case '#':
							  incrementTime();
							  break;
					  }
				  }
			}
		//reset key variable
		key = '\0';
		}
	}
	/*
	 * main execution loop
	 * 1 - check what to do depending on current input
	 * 2 - if current time is greater than zero display the time
	 * 	   else display dashes.
	 * */
	void runImpl() {
		 executeImpl();
		 if (getTimeImpl() > 0) {
			 IdisplayTime();
		 } else {
			 IdisplayWait();
		 }
	}
	/*Debounce algorithm implementation*/
	bool debounce(uint16_t key_pressed) {
		static uint16_t state = 0; // Current debounce status
		state=(state<<1) | key_pressed | 0xe000;
		if(state==0xffff)return true;
		return false;
	}
	/*Stops the inactivity timer and resets inactivity time counter*/
	void stopInactivityTimer() {
		HAL_TIM_Base_Stop_IT(htim_sleep);
		inactivity_t = 0;
	}
	/*
	 * Starts the timer (countdown)
	 * relay closes the circuit for the amount of time specified by the variable time
	 *
	 * */
	void startTimer() {

		//stop sleep timer
		stopInactivityTimer();

		displayFlush();

		//Start is displayed for about 500ms
		for (int i = 0; i < 62; ++i) {
			write_D1();
			print_5();
			HAL_Delay(2);
			write_D2();
			print_t();
			HAL_Delay(2);
			write_D3();
			print_r();
			HAL_Delay(2);
			write_D4();
			print_t();
			HAL_Delay(2);
		}

		displayFlush();

		key = '\0';
		prev_time = time;
		HAL_TIM_Base_Start_IT(htim);

		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

		//toggle relay pin, close switch
		HAL_GPIO_TogglePin(GPIO_RELAY_PORT, GPIO_RELAY_PIN);
		isTimerActive = true;

		HAL_Delay(1000);

		HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	}
	/*
	 * Delete the last inserted digit
	 * if only one digit was inserted the time is reset back to zero
	 *
	 * */
	void deleteDigit() {
		  if (time < 10) {
			  time = 0;
		  }
		  time /= 10;
		  time = floor(time);
	}
	/*
	 * Increments time by half a second
	 *
	 * */
	void incrementTime() {
		if (time > 0) time += 0.5;
	}
	/*
	 * Turns off the display
	 *
	 * */
	void displayFlush() {
		write_D1();
		print_OFF();
		write_D2();
		print_OFF();
		write_D3();
		print_OFF();
		write_D4();
		print_OFF();
	}
public:
	/*
	 * Static implementation of the private member functions
	 *
	 * */
	static float getTime() {
		return getTimer().getTimeImpl();
	}
	static char getKey() {
		return getTimer().getKeyImpl();
	}
	bool isPmodeActive() {
		return getTimer().isPmodeActiveImpl();
	}
	static bool inactivityTimeUpdate() {
		return getTimer().IinactivityTimeUpdate();
	}
	static bool isSleeping() {
		return getTimer().IisSleeping();
	}
	static void displayTime() {
		getTimer().IdisplayTime();
	}
	static void displayFunction() {
		getTimer().IdisplayFunction();
	}
	static void displayWait() {
		getTimer().IdisplayWait();
	}
	static void getIn(uint16_t GPIO_Pin) {
		getTimer().IgetIn(GPIO_Pin);
	}
	static void updateTime() {
		getTimer().IupdateTime();
	}
	static void execute() {
		getTimer().executeImpl();
	}
	static void setSleepState(bool state) {
		getTimer().IsetSleepState(state);
	}
	static void run() {
		getTimer().runImpl();
	}
};

#endif /* TIMER_H_ */
