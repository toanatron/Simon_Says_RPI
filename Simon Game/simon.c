#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include "simon.h"
#include <time.h>
#include <softTone.h>

//define button pins
#define btnRed	  0
#define btnBlue   3
#define btnGreen  21
#define btnYellow 23 

//define LED pins
#define redLED	  1
#define blueLED   4
#define greenLED  5
#define yellowLED 26

//define buzzer
#define buzzerPin 28

//statuses of LEDs
int statusRed 	 = 1; //int value 0 in array
int statusBlue   = 1; //int value 1 in array
int statusGreen  = 1; //int value 2 in array
int statusYellow = 1; //int value 3 in array

//game stats
int level = 1;	//current level
int rounds = 10; //make sure this number is the same as sequence and player array size
int sequence[10]; //sequence generated
int playerArray[10]; //player temporary sequence
int game = 1;	//condition for game to keep playing

//set up LED ISR
void redISR(void){
	statusRed = !statusRed;
}

void blueISR(void){
	statusBlue = !statusBlue;
}

void greenISR(void){
	statusGreen = !statusGreen;
}

void yellowISR(void){
	statusYellow = !statusYellow;
}

//random number generator
int random_number(){
	int lower = 0;
	int upper = 3;
	return (rand() % (upper-lower+1));
}

//populate array
void fill(void){
	int counter = 0;
	while(counter < level){
		sequence[counter] = random_number();
		counter++;
	}
}

//display the sequence and make sound
void displaySequence(void){
	fill();
	int counter = 0;
	printf("New sequence: \n");
	while(counter < level){
		printf("%d ",sequence[counter]);
		if(sequence[counter] == 0){
			digitalWrite(redLED, LOW);
			softToneWrite(buzzerPin, 400);
			delay(500);
			digitalWrite(redLED, HIGH);
			softToneWrite(buzzerPin, 0);
			delay(500);

		}else if(sequence[counter] == 1){
			digitalWrite(blueLED, LOW);
			softToneWrite(buzzerPin, 600);
			delay(500);
			digitalWrite(blueLED, HIGH);
			softToneWrite(buzzerPin, 0);
			delay(500);

		}else if (sequence[counter] == 2){
			digitalWrite(greenLED, LOW);
			softToneWrite(buzzerPin, 800);
			delay(500);
			digitalWrite(greenLED, HIGH);
			softToneWrite(buzzerPin, 0);
			delay(500);
		}else if (sequence[counter] == 3){
			digitalWrite(yellowLED, LOW);
			softToneWrite(buzzerPin, 1000);
			delay(500);
			digitalWrite(yellowLED, HIGH);
			softToneWrite(buzzerPin, 0);
			delay(500);
		}
		counter++;
	}
	player();
}

//player's playing function
void player(void){
	int index = 0;
	while(index < level){
		int button = getButton();
		if(button > 3 || button <0){
			continue;
		}else{
			playerArray[index] = button;
			if(button == 0){
				digitalWrite(redLED, LOW);
				softToneWrite(buzzerPin, 400);
				delay(200);
				digitalWrite(redLED, HIGH);
				softToneWrite(buzzerPin, 0);
				delay(200);
			}else if(button == 1){
				digitalWrite(blueLED, LOW);
				softToneWrite(buzzerPin, 600);
				delay(200);
				digitalWrite(blueLED, HIGH);
				softToneWrite(buzzerPin, 0);
				delay(200);
			}else if (button == 2){
				digitalWrite(greenLED, LOW);
				softToneWrite(buzzerPin, 800);
				delay(200);
				digitalWrite(greenLED, HIGH);
				softToneWrite(buzzerPin, 0);
				delay(200);
			}else{
				digitalWrite(yellowLED, LOW);
				softToneWrite(buzzerPin, 1000);
				delay(200);
				digitalWrite(yellowLED, HIGH);
				softToneWrite(buzzerPin, 0);
				delay(200);
			}
			if(button != sequence[index]){
				wrongInput();
				break;
			}
			index++;
		}
	}
	level++;
}
//function used to wait for player to make sequence
int getButton(){
	int count; int returnVal;
	for(count = 0; count < 200; count++){
		if (digitalRead(btnRed) == 0){
			returnVal = 0;
			count = 300;
		}else if (digitalRead(btnBlue) == 0){
			returnVal = 1;
			count = 300;
		}else if (digitalRead(btnGreen) == 0){
			returnVal = 2;
			count = 300;
		}else if (digitalRead(btnYellow) == 0){
			returnVal = 3;
			count = 300;
		}else {
			returnVal = 4;
		}
	}
	return returnVal;
}

//display when player made the wrong sequence
void wrongInput(void){
	printf("\nYour sequence was wrong!\n");
	int i;
	printf("This is the correct sequence: \n");
	for(i = 0; i < level; i++){
		if(sequence[i] == 0){
			printf("Red ");
		}else if(sequence[i] == 1){
			printf("Blue ");
		}else if(sequence[i] == 2){
			printf("Green ");
		}else if(sequence[i] == 3){
			printf("Yellow ");
		}
	}
	printf("\nThis is your sequence\n");
	for(i = 0; i < level; i++){
		if(playerArray[i] == 0){
			printf("Red ");
		}else if(playerArray[i] == 1){
			printf("Blue ");
		}else if(playerArray[i] == 2){
			printf("Green ");
		}else if(playerArray[i] == 3){
			printf("Yellow ");
		}
	}
	game = 0;
}

//used to start the game
void play(void){
	int counter = 0;
	while(counter < rounds && game ==1){
		displaySequence();
		delay(2000);
		counter++;
	}
	if(game == 1){
		printf("\nYou have won! Would you like to play again?(Y/N) ");
	}else if (game == 0){
		printf("\nYou have lost! would you like to play again? (Y/N) ");
	}
	char c;
	scanf(" %c",&c);
	if(c == 'Y' || c == 'y'){
		game = 1;
		level = 1;
		play();
	}else{
		exit(0);
	}
}

//main function sets up all the GPIO
int main(void){
	
	srand(time(NULL));	
	if (wiringPiSetup() == -1){
		printf("setup wiringPi failed!\n");
		return -1;
	}
	if(softToneCreate(buzzerPin) < 0){
		printf("SoftTone setup failed!\n");
		return -1;
	}
	
	pinMode(redLED, OUTPUT);
	pinMode(btnRed, INPUT);
	pinMode(blueLED, OUTPUT);
	pinMode(btnBlue, INPUT);
	pinMode(greenLED, OUTPUT);
	pinMode(btnGreen, INPUT);
	pinMode(yellowLED, OUTPUT);
	pinMode(btnYellow, INPUT);

	pullUpDnControl(btnRed, PUD_UP);
	pullUpDnControl(btnBlue, PUD_UP);
	pullUpDnControl(btnGreen, PUD_UP);
	pullUpDnControl(btnYellow, PUD_UP);
	
	if(wiringPiISR(btnRed, INT_EDGE_FALLING, redISR)<0){
		printf("Red ISR setup error!\n");
		exit(0);
	}else if(wiringPiISR(btnBlue, INT_EDGE_FALLING, blueISR)<0){
		printf("Blue ISR setup error!\n");
		exit(0);
	}else if(wiringPiISR(btnGreen, INT_EDGE_FALLING, greenISR)<0){
		printf("Green ISR setup error!\n");
		exit(0);
	}else if (wiringPiISR(btnYellow, INT_EDGE_FALLING, yellowISR)<0){
		printf("Yellow ISR setup error!\n");
		exit(0);
	}
	digitalWrite(redLED, HIGH);
	digitalWrite(blueLED, HIGH);
	digitalWrite(greenLED, HIGH);
	digitalWrite(yellowLED, HIGH);
	play();
	
	return 0;
}
