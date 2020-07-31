/* Team Id:4289
 * Author List: Yashwant kumar.
 * filename: task5-main.c
 * Theme: Construct O-Bot.
 * Global Variables: current_pos,direction,pick_array,place_array,pick_right,place_high.
 */ 

/* left_wls = channel(1)
   center_wls = channel(2)
   right_wls = channel(3)
   sharp-sensor(left) = channel(4)
   sharp-sensor(right) = channel(5)*/

#define F_CPU 14745600

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define NORTH 4
#define EAST 3
#define SOUTH 2
#define WEST 1

//threshold values for white line sensor.
#define LEFT_THRESH 0xEA 
#define CENTER_THRESH 0xEA
#define RIGHT_THRESH 0xEA
#define WHITE_THRESH 0xDA

int current_pos = 0,direction = SOUTH;
int pick_array[] = {7,5,14,14,12,3,10,10};//array which contains node numbers of where to pick CM.
int place_array[] = {4,13,13,11,6,11,6,4};//array which contains node numbers of where to place CM.

bool place_high[] = {false,false,false,false,false,false,true,false,false,false,false,false,false,true,false};//this array helps the bot to identify low rise and high rise house.
bool pick_right[15] = {false,false,false,true,false,true,false,true,false,false,true,false,true,false,true};//this array will help the bot to know which side the CM is present.

//port initilization for motor, servo, white line sensor.
void port_init(){
	DDRL = 0x18;
	PORTL = 0x18;
	DDRC = 0x0F;
	DDRB = DDRB | 0xE0;
	PORTB = PORTB | 0xE0;
	DDRF = 0x00;
	PORTF = 0x00;
}

//initilization of ADC registers.
void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		
	ADMUX = 0x20;		
	ACSR = 0x80;
	ADCSRA = 0x86;		
}

//function for ADC conversion of sensors.
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	ADCSRA = ADCSRA | 0x40;		
	while((ADCSRA&0x10)==0);	
	a=ADCH;
	ADCSRA = ADCSRA|0x10; 
	ADCSRB = 0x00;
	return a;
}

//We will pass the required value for velocity of both motors.
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//for buzzer to beep for 2 seconds.
void buzzer_on(){
	DDRJ = DDRJ | 0x02;
	PORTJ = PORTJ & 0x00;
	PORTJ = 0x0F;
	_delay_ms(2000);
	PORTJ = 0x00;
	_delay_ms(10000);
}

//initilization of timer & counter registers.
void timer_init(){
	TCCR5A = 0xA9;
	TCCR5B = 0x0D;
	TCCR1A = 0x00; 
	TCCR1B = 0x00; 
	TCNT1H = 0xFC; 
	TCNT1L = 0x01;	
	OCR1AH = 0x00;	
	OCR1AL = 180;	
	OCR1BH = 0x00;
	OCR1BL = 90;		
	OCR1CH = 0x00;	
	OCR1CL = 0;	
	ICR1H  = 0x00;	
	ICR1L = 255;
	TCCR1A = 0xAB; 
	TCCR1C = 0x00;
	TCCR1B = 0x0C;	
}

//for bot to move forward.
void forward(void){
	PORTC = 0x05;
	velocity(200,255);
}

//for bot to move backward.
void backward(void){
	PORTC = 0x0A;
	velocity(50,67);	
}

//for bot to stop while moving forward.
void stop_forward(){
	forward();
	_delay_ms(130);
	PORTC = 0x0A;
	velocity(80,90);
	_delay_ms(70);
	PORTC = 0x00;
	velocity(0,0);
	_delay_ms(100);
}

//This function helps the bot to know its exact position or the node number with the help of its direction and previous node number.
int node_count(){
	if(current_pos == 0){
		if(direction == SOUTH){
			return ++ current_pos ;
		}
	}
	else if(current_pos == 1 ){
		if(direction == WEST){
			return ++ current_pos;
		}
		else if(direction == EAST){
			current_pos = 15;
			return current_pos;
		}
	}
	else if(current_pos == 2){
		if(direction == NORTH){
			return ++ current_pos;
		}
		else if(direction == EAST){
			return -- current_pos;
		}
	}
	else if(current_pos == 3){
		if(direction == NORTH){
			return ++ current_pos;
		}
		else if(direction == SOUTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 4){
		if(direction == NORTH){
			return ++ current_pos;
		}
		else if(direction == SOUTH){
			return -- current_pos;
		}
		else if(direction == EAST){
			current_pos = 13;
			return current_pos;
		}
	}
	else if(current_pos == 5){
		if(direction == NORTH){
			return ++ current_pos;
		}
		else if(direction == SOUTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 6){
		if(direction == NORTH){
			return ++ current_pos;
		}
		else if(direction == SOUTH){
			return -- current_pos;
		}
		else if(direction == EAST){
			current_pos = 11;
			return current_pos;
		}
	}
	else if(current_pos == 7){
		if(direction == NORTH){
			return ++ current_pos;
		}
		else if(direction == SOUTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 8){
		if(direction == SOUTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 9){
		if(direction == SOUTH){
			return ++ current_pos;
		}
	}
	else if(current_pos == 10){
		if(direction == SOUTH){
			return ++ current_pos;
		}
		else if(direction == NORTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 11){
		if(direction == SOUTH){
			return ++ current_pos;
		}
		else if(direction == NORTH){
			return -- current_pos;
		}
		else if(direction == WEST){
			current_pos = 6;
			return current_pos;
		}
	}
	else if(current_pos == 12){
		if(direction == SOUTH){
			return ++ current_pos;
		}
		else if(direction == NORTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 13){
		if(direction == SOUTH){
			return ++ current_pos;
		}
		else if(direction == NORTH){
			return -- current_pos;
		}
		else if(direction == WEST){
			current_pos= 4;
			return current_pos;
		}
	}
	else if(current_pos == 14){
		if(direction == SOUTH){
			return ++ current_pos;
		}
		else if(direction == NORTH){
			return -- current_pos;
		}
	}
	else if(current_pos == 15){
		if(direction == NORTH){
			return -- current_pos;
		}
		else if(direction == WEST){
			current_pos = 1;
			return current_pos;
		}
	}
}


//for bot to stop while turning left.
void stop_left(void){
	PORTC = 0x09;
	velocity(200,250);
	_delay_ms(100);
	PORTC = 0x00;
	velocity(0,0);
	_delay_ms(100);
}

//for bot to stop while turning right.
void stop_right(void){
	PORTC = 0x06;
	velocity(250,200);
	_delay_ms(105);
	PORTC = 0x00;
	velocity(0,0);
	_delay_ms(100);
}

//to turn left.
void left(void){
	unsigned char center_sensor;
	PORTC = 0x06;
	velocity(240,200);
	_delay_ms(250);
	do{
		center_sensor = ADC_Conversion(2);
		PORTC = 0x06;
		velocity(220,190);
		_delay_ms(1);
	}while(center_sensor <= CENTER_THRESH);
	stop_left();
}

//to turn right.
void right(void){
	unsigned char center_sensor;
	PORTC = 0x09;
	velocity(255,255);
	_delay_ms(200);
	do{
		center_sensor = ADC_Conversion(2);
		PORTC = 0x09;
		velocity(255,255);
		_delay_ms(1);
	}while(center_sensor <= CENTER_THRESH);
	stop_right();
}

void init(){
	cli();
	port_init();
	timer_init();
	adc_init();
	sei();
}

//moving servo to a specified angle.
void servo1(unsigned char degrees){
	float val = ((float)degrees * 0.512) + 34.56;
	OCR1AH = 0x00;
	OCR1AL = (uint16_t) val;
}

//moving servo to a specified angle.
void servo3(unsigned char degrees){
	float val = ((float)degrees * 0.512) + 34.56;
	OCR1BH = 0x00;
	OCR1BL = (uint16_t) val;
}

//moving servo to a specified angle.
void servo2(unsigned char degrees){
	float val = ((float)degrees * 0.512) + 34.56;
	OCR1CH = 0x00;
	OCR1CL = (uint16_t) val;
}

//This function helps the bot move on the arena detecting the black line with the help of white line sensor & also to move on BREAK PATH.
int forward_wls(int node){
	int node_num;
	unsigned char left_sensor,center_sensor,right_Sensor;
	while(node--){
		while(1){
			left_sensor = ADC_Conversion(1);
			center_sensor = ADC_Conversion(2);
			right_Sensor = ADC_Conversion(3);
			if(left_sensor <= LEFT_THRESH && center_sensor >= CENTER_THRESH && right_Sensor <= RIGHT_THRESH){
				forward();
				_delay_ms(50);
			}
			else if(left_sensor >= LEFT_THRESH && center_sensor <= CENTER_THRESH && right_Sensor <= RIGHT_THRESH){
				do{
					left_sensor = ADC_Conversion(1);
					center_sensor = ADC_Conversion(2);
					right_Sensor = ADC_Conversion(3);
					PORTC = 0x06;
					velocity(36,49);
					_delay_ms(1);
				}while((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor <= CENTER_THRESH);
			} 
			else if(left_sensor <= LEFT_THRESH && center_sensor <= CENTER_THRESH && right_Sensor >= RIGHT_THRESH){
				do{
					left_sensor = ADC_Conversion(1);
					center_sensor = ADC_Conversion(2);
					right_Sensor = ADC_Conversion(3);
					PORTC = 0x09;
					velocity(36,49);
					_delay_ms(1);
				}while((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor <= CENTER_THRESH);
			}
			else if(left_sensor >= LEFT_THRESH && right_Sensor >= RIGHT_THRESH && center_sensor >= CENTER_THRESH){
				forward();
				_delay_ms(115);
				stop_forward();
				node_num = node_count();
				break;
			}
			else if((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor >= CENTER_THRESH){
				forward();
				_delay_ms(115);
				stop_forward();
				node_num = node_count();
				break;
			}
		}
		return node_num;
	}
}

//this function helps the bot to pick CM's from respective nodes. 
void pick_block(){
	//This condition helps the bot to check whether it is picking CM's near the walls as there our gripper may hit the wall if it opens fully.
	if((current_pos == 3 && direction == EAST) || (current_pos == 5 && direction == EAST) || (current_pos == 12 && direction == WEST) || (current_pos == 14 && direction == WEST)){
		servo3(130);
		_delay_ms(500);
		servo1(106);
		_delay_ms(700);
		servo3(110);
		_delay_ms(500);
		servo2(0);
		_delay_ms(1000);
		servo3(190);
		_delay_ms(1000);
		servo1(200);
		_delay_ms(500);	
		servo2(80);
		_delay_ms(500);
	}
	else{
		servo3(0);
		_delay_ms(500);
		servo1(106);
		_delay_ms(500);
		servo2(0);
		_delay_ms(700);
		servo3(190);
		_delay_ms(700);
		servo1(200);
		_delay_ms(500);
		servo2(80);
		_delay_ms(500);
	}
}

// to place CM's at there respective HOUSES.
void place_block(){
	//this condition helps the bot to check whether it is placing in special house.
	if((current_pos == 9 && direction == WEST)||(current_pos == 8 && direction == EAST)){
		servo2(50);
		_delay_ms(200);
		servo1(150);
		_delay_ms(500);
		servo3(30);
		_delay_ms(500);
		servo1(180);
		_delay_ms(100);
		servo3(100);
		_delay_ms(100);
	}
	else{
		//this condition helps the bot to check whether it is placing in high rise house.
		if(place_high[current_pos]){
			servo1(170);
			_delay_ms(500);
			servo2(30);
			_delay_ms(500);
			servo3(30);
			_delay_ms(500);
			servo1(180);
			_delay_ms(100);
			servo3(100);
			_delay_ms(100);
		}
		else{
			servo1(140);
			_delay_ms(500);
			servo2(22);
			_delay_ms(500);
			servo3(30);
			_delay_ms(500);
			servo1(180);
			_delay_ms(100);
			servo3(100);
			_delay_ms(100);
		}	
	}
}

//this function helps the bot to take left turn while it is going to place in special house.
void left_white(){
	PORTC = 0x06;
	velocity(255,255);
	_delay_ms(658);
	stop_left();
}

//this function helps the bot to take right turn while it is going to place in special house.
void right_white(){
	PORTC = 0x09;
	velocity(255,255);
	_delay_ms(610);
	stop_right();
}

void white_path(){
	unsigned char right_sharp,left_sharp,left_sensor,center_sensor,right_Sensor;
	while(1){
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_Sensor = ADC_Conversion(3);
		if(left_sensor >= LEFT_THRESH && center_sensor <= CENTER_THRESH && right_Sensor >= RIGHT_THRESH){
			forward();
			_delay_ms(50);
		}
		else if(left_sensor <= LEFT_THRESH && center_sensor >= CENTER_THRESH && right_Sensor >= RIGHT_THRESH){
			do{
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_Sensor = ADC_Conversion(3);
				PORTC = 0x06;
				velocity(36,49);
				_delay_ms(1);
			}while((left_sensor <= LEFT_THRESH || right_Sensor <= RIGHT_THRESH) && center_sensor >= CENTER_THRESH);
		}
		else if(left_sensor >= LEFT_THRESH && center_sensor >= CENTER_THRESH && right_Sensor <= RIGHT_THRESH){
			do{
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_Sensor = ADC_Conversion(3);
				PORTC = 0x06;
				velocity(36,49);
				_delay_ms(1);
			}while((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor >= CENTER_THRESH);
		}
		else if(left_sensor <= LEFT_THRESH && right_Sensor <= RIGHT_THRESH && center_sensor >= CENTER_THRESH){
			forward();
			_delay_ms(115);
			stop_forward();
			break;
		}
	}
}

//this function helps the bot to travel through the WALL PATH.
int wall_forward(){
	int node_num;
	unsigned char left_sharp,right_sharp;
	unsigned char left_sensor,center_sensor,right_Sensor;
	while(1){
		left_sensor = ADC_Conversion(1);
		center_sensor = ADC_Conversion(2);
		right_Sensor = ADC_Conversion(3);
		left_sharp = ADC_Conversion(4);
		right_sharp = ADC_Conversion(5);
		PORTC = 0x05;
		velocity(43.5,49);
		_delay_ms(20);
		if(left_sensor <= LEFT_THRESH && center_sensor >= CENTER_THRESH && right_Sensor <= RIGHT_THRESH){
			PORTC = 0x05;
			velocity(43.5,49);
			_delay_ms(10);
		}
		else if(left_sensor >= LEFT_THRESH && center_sensor <= CENTER_THRESH && right_Sensor <= RIGHT_THRESH){
			do{
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_Sensor = ADC_Conversion(3);
				PORTC = 0x06;
				velocity(30,40);
				_delay_ms(0.1);
			}while((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor <= CENTER_THRESH);
		}
		else if(left_sensor <= LEFT_THRESH && center_sensor <= CENTER_THRESH && right_Sensor >= RIGHT_THRESH){
			do{
				left_sensor = ADC_Conversion(1);
				center_sensor = ADC_Conversion(2);
				right_Sensor = ADC_Conversion(3);
				PORTC = 0x09;
				velocity(30,40);
				_delay_ms(0.1);
			}while((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor <= CENTER_THRESH);
		}
		else if(left_sensor >= LEFT_THRESH && right_Sensor >= RIGHT_THRESH && center_sensor >= CENTER_THRESH){
			forward();
			_delay_ms(132);
			stop_forward();
			node_num = node_count();
			break;
		}
		else if((left_sensor >= LEFT_THRESH || right_Sensor >= RIGHT_THRESH) && center_sensor >= CENTER_THRESH){
			forward();
			_delay_ms(132);
			stop_forward();
			node_num = node_count();
			break;
		}
		else if( 0x1F <= right_sharp && right_sharp <= 0x80){
			do{
				right_sharp = ADC_Conversion(5);
				PORTC = 0x06;
				velocity(50,30);
				_delay_ms(1);
			}while(0x1F <= right_sharp && right_sharp <= 0x60);
		}
		else if(0x60 <= left_sharp && left_sharp <= 0xC2 ){
			do{
				left_sharp = ADC_Conversion(4);
				PORTC = 0x09;
				velocity(35,50);
				_delay_ms(1);
			}while(0x60 <= left_sharp && left_sharp <= 0x80);
		}
	}
	return node_num;
}

//In this function we will pass the picking CM's node number and node number of its respective house in which it is to be placed.
void path_following(int tar_pick,int tar_place){
	if(current_pos == 1){
		if(tar_pick > 8 ){
			left();
			direction = EAST;
			current_pos = forward_wls(1);
			left();
			direction = NORTH;
			while(current_pos != tar_pick){
				current_pos = forward_wls(1);
			}
			if(pick_right[current_pos]){
				right();
				pick_block();
				direction = EAST;
				pick_right[current_pos] = false;
				if(tar_place > 8){
					if(tar_pick > tar_place){
						left();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						place_block();
						direction = EAST;
					}
					else if(tar_pick < tar_place){
						right();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						place_block();
						direction = EAST;
					}
				}
				else if(tar_place < 8){
					if(tar_pick == 14){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 12){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 10){
						right();
						direction = SOUTH;
						current_pos = forward_wls(1);
						right();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
			else {
				left();
				pick_block();
				direction = WEST;
				if(tar_place > 8){
					if(tar_pick > tar_place){
						right();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						place_block();
						direction = EAST;
					}
					else if(tar_pick < tar_place){
						left();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						place_block();
						direction = EAST;
					}
				}
				else if(tar_place < 8){
					if(tar_pick == 14){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 12){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 10){
						left();
						direction = SOUTH;
						current_pos = forward_wls(1);
						right();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
		}
		else if(tar_pick < 8){
			right();
			direction = WEST;
			current_pos = forward_wls(1);
			right();
			direction = NORTH;
			while(current_pos != tar_pick){
				current_pos = forward_wls(1);
			}
			if(pick_right[current_pos]){
				right();
				pick_block();
				direction = EAST;
				pick_right[current_pos] = false;
				if(tar_place < 8){
					if(tar_pick > tar_place){
						right();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						place_block();
						direction = WEST;
					}
					else if(tar_pick < tar_place){
						left();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						place_block();
						direction = WEST;
					}
				}
				else if(tar_place > 8){
					if(tar_pick == 3){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 5){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 7){
						right();
						direction = SOUTH;
						current_pos = forward_wls(1);
						left();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
			else{
				left();
				pick_block();
				direction = WEST;
				if(tar_place < 8){
					if(tar_pick > tar_place){
						left();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						place_block();
						direction = WEST;
					}
					else if(tar_pick < tar_place){
						right();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						place_block();
						direction = WEST;
					}
				}
				else if(tar_place > 8){
					if(tar_pick == 3){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 5){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_pick == 7){
						left();
						direction = SOUTH;
						current_pos = forward_wls(1);
						left();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
		}
	}
	else if(current_pos == 4){
		if(tar_pick > 8){
			right();
			right(); 
			direction = EAST;
			current_pos = wall_forward();
			if(current_pos > tar_pick){
				left();
				direction = NORTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){ 
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						if(tar_pick > tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							place_block();
							direction = EAST;
						}
						else if(tar_pick < tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							place_block();
							direction = EAST;
						}
					}
					else if(tar_place < 8){
						if(tar_pick == 12){
							left();
							direction = NORTH;
							current_pos = forward_wls(1);
							left();
							direction = WEST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								left();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
						else if(tar_pick == 10){
							right();
							direction = SOUTH;
							current_pos = forward_wls(1);
							right();
							direction = WEST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								left();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						if(tar_pick > tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							place_block();
							direction = EAST;
						}
						else if(tar_pick < tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							place_block();
							direction = EAST;
						}
					}
					else if(tar_place < 8){
						if(tar_pick == 12){
							right();
							direction = NORTH;
							current_pos = forward_wls(1);
							left();
							direction = WEST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								left();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
						else if(tar_pick == 10){
							left();
							direction = SOUTH;
							current_pos = forward_wls(1);
							right();
							direction = WEST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								left();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
			}
			else if(current_pos < tar_pick){
				right();
				direction = SOUTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						left();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = EAST;
						place_block();
					}
					else if(tar_place < 8){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						right();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = EAST;
						place_block();
					}
					else if(tar_place < 8){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
							direction = WEST;
						}
					}
				}
			}
		}
		else if(tar_pick < 8){
			if(current_pos > tar_pick){
				left();
				direction = SOUTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					left();
					pick_block();
					direction = EAST;
					pick_right[current_pos]=false;
					if(tar_place < 8){
						left();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						place_block();
						direction = WEST;
					}
					else if(tar_place > 8){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place < 8){
						right();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						place_block();
						direction = WEST;
					}
					else if(tar_place > 8){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
			else if(current_pos < tar_pick){
				right();
				direction = NORTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					right();
					direction = EAST;
					pick_block();
					if(tar_place < 8){
						if(current_pos == 5){
							if(current_pos > tar_place){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
							else if(current_pos < tar_place){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 7){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
					}
					else if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 7){
							right();
							direction = SOUTH;
							current_pos = forward_wls(1);
							left();
							direction = EAST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								right();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place < 8){
						if(current_pos == 5){
							if(current_pos > tar_place){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
							else if(current_pos < tar_place){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 7){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
					}
					else if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 7){
							left();
							direction = SOUTH;
							current_pos = forward_wls(1);
							left();
							direction = EAST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								right();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
			}
		}
	}
	else if(current_pos == 13){							
		if(tar_pick > 8){
			if(current_pos > tar_pick){
				left();
				direction = NORTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);	
				}
				if(pick_right[current_pos]){
					right();
					pick_block();
					direction = EAST;
					pick_right[current_pos]=false;
					if(tar_place < 8){
						if(current_pos == 12){
							if(tar_place == 6){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								current_pos = forward_wls(1);
								place_block();
							}
							else if(tar_place == 4){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								current_pos = wall_forward();
								place_block();
							}
						}
						else if(current_pos == 10){
							right();
							direction = SOUTH;
							current_pos = forward_wls(1);
							right();
							direction = WEST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								left();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
					else if(tar_place > 8){
						if(current_pos > tar_place){
							left();
							direction = NORTH;
							current_pos = forward_wls(1);
							right();
							direction = EAST;
							place_block();
						}
						else{
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);	
							}
							left();
							direction = EAST;
							place_block();
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place < 8){
						if(current_pos == 12){
							if(tar_place == 6){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								current_pos = forward_wls(1);
								place_block();
							}
							else if(tar_place == 4){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								current_pos = wall_forward();
								place_block();
							}
						}
						else if(current_pos == 10){
							left();
							direction = SOUTH;
							current_pos = forward_wls(1);
							right();
							direction = WEST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								left();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
					else if(tar_place > 8){
						if(current_pos > tar_place){
							right();
							direction = NORTH;
							current_pos = forward_wls(1);
							right();
							direction = EAST;
							place_block();
						}
						else{
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
					}
				}
			}
			else if(current_pos < tar_pick){
				right();
				direction = SOUTH; 
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place < 8){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_place > 8){
						left();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = EAST;
						place_block();
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						right();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						place_block();
						direction = EAST;
					}
					else if(tar_place < 8){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
		}
		else if(tar_pick < 8){
			right();
			right();//180 turn.
			direction = WEST;
			current_pos = wall_forward();
			if(current_pos > tar_pick){
				left();
				direction = SOUTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						left();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_place < 8){
						left();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						direction = WEST;
						place_block();
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						right();
						direction = NORTH;
						current_pos = forward_wls(1);
						right();
						direction = EAST;
						current_pos = wall_forward();
						if(current_pos != tar_place){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_place < 8){
						right();
						direction = NORTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						direction = WEST;
						place_block();
					}
				}
			}
			else if(current_pos < tar_pick){
				right();
				direction = NORTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 11){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
							else if(tar_place == 13){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
						}
						else if(current_pos == 7){
							right();
							direction = SOUTH;
							current_pos = forward_wls(1);
							left();
							direction = EAST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								right();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
					else if(tar_place < 8){
						if(current_pos == 5){
							if(tar_place == 6){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
							else if(tar_place == 4){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 7){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 7){
							left();
							direction = SOUTH;
							current_pos = forward_wls(1);
							left();
							direction = EAST;
							current_pos = forward_wls(1);
							if(current_pos != tar_place){
								right();
								direction = SOUTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
			}
		}
	}
	else if(current_pos == 6){						
		if(tar_pick > 8){
			right();
			right(); 
			direction = EAST;
			current_pos = forward_wls(1);
			if(current_pos < tar_pick){
				right();
				direction = SOUTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						if(current_pos == 14){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
						else if(current_pos == 12){
							if(current_pos > tar_place){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								place_block();
								direction = EAST;
							}
							else if(current_pos < tar_place){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								place_block();
							}
						}
					}
					else if(tar_place < 8){
						if(tar_pick == 12){
							if(tar_place == 4){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								current_pos = wall_forward();
								place_block();
							}
							else if(current_pos == 6){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(tar_pick == 14){
							left();
							direction = NORTH;
							current_pos = forward_wls(1);
							left();
							direction = WEST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								right();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						if(current_pos > tar_place){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							place_block();
							direction = EAST;
						}
						else if(current_pos < tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							place_block();
							direction = EAST;
						}
					}
					else if(tar_place < 8){
						if(tar_pick == 14){
							right();
							direction = NORTH;
							current_pos = forward_wls(1);
							left();
							direction = WEST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								right();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
						else if(tar_pick == 12){
							if(tar_place == 4){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 6){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
					}
				}
			}
			else if(current_pos > tar_pick){
				left();
				direction = NORTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						right();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						direction = EAST;
						place_block();
					}
					else if(tar_place < 8){
						right();
						direction = SOUTH;
						current_pos = forward_wls(1);
						right();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						left();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						direction = EAST;
						place_block();
					}
					else if(tar_place < 8){
						left();
						direction = SOUTH;
						current_pos = forward_wls(1);
						right();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
		}
		else if(tar_pick < 8){
			if(current_pos < tar_pick){
				right();
				direction = NORTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place < 8){
						right();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = WEST;
						place_block();
					}
					else if(tar_place > 8){
						right();
						direction = SOUTH;
						current_pos = forward_wls(1);
						left();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place < 8){
						left();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = WEST;
						place_block();
					}
					else if(tar_place > 8){
						left();
						direction = SOUTH;
						current_pos = forward_wls(1);
						left();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
			else if(current_pos > tar_pick){
				left();
				direction = SOUTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 3){
							left();
							direction = NORTH;
							current_pos = forward_wls(1);
							right();
							direction = EAST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								left();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}			
					}
					else if(tar_place < 8){
						if(current_pos == 5){
							if(tar_place == 6){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
							else if(tar_place == 4){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 3){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 3){
							right();
							direction = NORTH;
							current_pos = forward_wls(1);
							right();
							direction = EAST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								left();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
					else if(tar_place < 8){
						if(current_pos == 5){
							if(tar_place == 6){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
							else if(tar_place == 4){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 3){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
					}
				}
			}
		}
	}
	else if(current_pos == 11){
		if(tar_pick > 8){
			if(current_pos > tar_pick){
				left();
				direction = NORTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						right();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						direction = EAST;
						place_block();
					}
					else if(tar_place < 8){
						right();
						direction = SOUTH;
						current_pos = forward_wls(1);
						right();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						left();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						left();
						direction = EAST;
						place_block();
					}
					else if(tar_place < 8){
						left();
						direction = SOUTH;
						current_pos = forward_wls(1);
						right();
						direction = WEST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							left();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = WEST;
							place_block();
						}
						else{
							place_block();
						}
					}
				}
			}
			else if(current_pos < tar_pick){
				right();
				direction = SOUTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						if(current_pos == 12){
							if(current_pos < tar_place){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								place_block();
							}
							else if(current_pos > tar_place){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								place_block();
							}
						}
						else if(current_pos == 14){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
					}
					else if(tar_place < 8){
						if(current_pos == 12){
							if(tar_place == 4){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 6){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 14){
							left();
							direction = NORTH;
							current_pos = forward_wls(1);
							left();
							direction = WEST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								right();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								direction = WEST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						if(current_pos == 12){
							if(current_pos > tar_place){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								place_block();
							}
							else if(current_pos < tar_place){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								place_block();
							}
						}
						else if(current_pos == 14){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							right();
							direction = EAST;
							place_block();
						}
					}
					else if(tar_place < 8){
						if(current_pos == 12){
							if(tar_place == 4){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 6){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 14){
							right();
							direction = NORTH;
							current_pos = forward_wls(1);
							left();
							direction = WEST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								right();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								left();
								place_block();
								direction = WEST;
							}
							else{
								place_block();
							}
						}
					}
				}
			}
		}
		else if(tar_pick < 8){
			right();
			right();//180 turn.
			direction = WEST;
			current_pos = forward_wls(1);
			if(current_pos > tar_pick){
				left();
				direction = SOUTH;
				while(current_pos != tar_pick){
					current_pos = forward_wls(1);
				}
				if(pick_right[current_pos]){
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 3){
							left();
							direction = NORTH;
							current_pos = forward_wls(1);
							right();
							direction = EAST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								left();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
					else if(tar_place < 8){
						if(current_pos == 5){
							if(tar_place == 6){
								left();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
							else if(tar_place == 4){
								right();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 3){
							left();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
					}
				}
				else{
					right();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						if(current_pos == 5){
							if(tar_place == 13){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								left();
								direction = EAST;
								current_pos = wall_forward();
								place_block();
							}
							else if(tar_place == 11){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								right();
								direction = EAST;
								current_pos = forward_wls(1);
								place_block();
							}
						}
						else if(current_pos == 3){
							right();
							direction = NORTH;
							current_pos = forward_wls(1);
							right();
							direction = EAST;
							current_pos = wall_forward();
							if(current_pos != tar_place){
								left();
								direction = NORTH;
								while(current_pos != tar_place){
									current_pos = forward_wls(1);
								}
								right();
								direction = EAST;
								place_block();
							}
							else{
								place_block();
							}
						}
					}
					else if(tar_place < 8){
						if(current_pos == 5){
							if(tar_place == 6){
								right();
								direction = NORTH;
								current_pos = forward_wls(1);
								left();
								direction = WEST;
								place_block();
							}
							else if(tar_place == 4){
								left();
								direction = SOUTH;
								current_pos = forward_wls(1);
								right();
								direction = WEST;
								place_block();
							}
						}
						else if(current_pos == 3){
							right();
							direction = NORTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = WEST;
							place_block();
						}
					}
				}
			}
			else if(current_pos < tar_pick){
				right();
				direction = NORTH;
				current_pos = forward_wls(1);
				if(pick_right[current_pos]){
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					if(tar_place > 8){
						right();
						direction = SOUTH;
						current_pos = forward_wls(1);
						left();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_place < 8){
						right();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = WEST;
						place_block();
					}
				}
				else{
					left();
					direction = WEST;
					pick_block();
					if(tar_place > 8){
						left();
						direction = SOUTH;
						current_pos = forward_wls(1);
						left();
						direction = EAST;
						current_pos = forward_wls(1);
						if(current_pos != tar_place){
							right();
							direction = SOUTH;
							while(current_pos != tar_place){
								current_pos = forward_wls(1);
							}
							left();
							direction = EAST;
							place_block();
						}
						else{
							place_block();
						}
					}
					else if(tar_place < 8){
						left();
						direction = SOUTH;
						while(current_pos != tar_place){
							current_pos = forward_wls(1);
						}
						right();
						direction = WEST;
						place_block();
					}
				}
			}
		}
	}
}

//In this function we will pass the node number of the CM's which is to be picked and placed in special house.
void special_house(int special_pick,int special_pick_2){
	current_pos = forward_wls(1);
	if(current_pos == 1){
		if(special_pick > 8){
			left();
			direction = EAST;
			current_pos = forward_wls(1);
			left();
			direction = NORTH;
			while(current_pos != special_pick){
				current_pos = forward_wls(1);
			}
			right();
			direction = EAST;
			pick_block();
			pick_right[current_pos] = false;
			left();
			direction = NORTH;
			while(current_pos != 9){
				current_pos = forward_wls(1);
			}
			left_white();
			direction = WEST;
			white_path();
			place_block();
			white_path();
			current_pos = 8;
			left();
			direction = SOUTH;
			if(special_pick_2 < 8){
				while(current_pos != special_pick_2){
					current_pos = forward_wls(1);
				}
				right();
				direction = WEST;
				pick_block();
				right();
				direction = NORTH;
				while(current_pos != 8){
					current_pos = forward_wls(1);
				}
				right_white();
				direction = EAST;
				_delay_ms(500);
				white_path();
				place_block();
				white_path();
				current_pos = 9;
				right();
				direction = SOUTH;
				while(current_pos != 11){
					current_pos = forward_wls(1);
				}
				left();
				direction = EAST;
			}
			else if(special_pick_2 > 8){
				while(current_pos != 6){
					current_pos = forward_wls(1);
				}
				left();
				direction = EAST;
				current_pos = forward_wls(1);
				if(current_pos < special_pick_2){
					right();
					direction = SOUTH;
					while(current_pos != special_pick_2){
						current_pos = forward_wls(1);
					}
					left();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					left();
					direction = NORTH;
					while(current_pos != 9){
						current_pos = forward_wls(1);
					}
					left_white();
					direction = WEST;
					white_path();
					place_block();
					white_path();
					current_pos = 8;
					left();
					direction = SOUTH;
					while(current_pos != 6){
						current_pos = forward_wls(1);
					}
					right();
					direction = WEST;
				}
				else if(current_pos > special_pick_2){
					left();
					direction = NORTH;
					current_pos = forward_wls(1);
					right();
					direction = EAST;
					pick_block();
					pick_right[current_pos] = false;
					left();
					direction = NORTH;
					current_pos = forward_wls(1);
					left_white();
					direction = WEST;
					white_path();
					place_block();
					white_path();
					current_pos = 8;
					left();
					direction = SOUTH;
					while(current_pos != 6){
						current_pos = forward_wls(1);
					}
					right();
					direction = WEST;
				}
			}
		}
		else if(special_pick < 8){
			right();
			direction = WEST;
			current_pos = forward_wls(1);
			right();
			direction = NORTH;
			while(current_pos != special_pick){
				current_pos = forward_wls(1);
			}
			left();
			direction = WEST;
			pick_block();
			right();
			direction = NORTH;
			while(current_pos != 8){
				current_pos = forward_wls(1);
			}
			right_white();
			direction = EAST;
			_delay_ms(500);
			white_path();
			place_block();
			white_path();
			current_pos = 9;
			right();
			direction = SOUTH;
			if(special_pick_2 > 8){
				while(current_pos != special_pick_2){
					current_pos = forward_wls(1);
				}
				left();
				direction = EAST;
				pick_block();
				pick_right[current_pos] = false;
				left();
				direction = NORTH;
				while(current_pos != 9){
					current_pos = forward_wls(1);
				}
				left_white();
				direction = WEST;
				_delay_ms(500);
				white_path();
				place_block();
				white_path();
				current_pos = 8;
				left();
				direction = SOUTH;
				while(current_pos != 6){
					current_pos = forward_wls(1);
				}
				right();
				direction = WEST;
			}
			else if(special_pick_2 < 8){
				while(current_pos != 11){
					current_pos = forward_wls(1);
				}
				right();
				direction = WEST;
				current_pos = forward_wls(1);
				if(current_pos < special_pick_2){
					right();
					direction = NORTH;
					current_pos = forward_wls(1);
					left();
					direction = WEST;
					pick_block();
					right();
					direction = NORTH;
					current_pos = forward_wls(1);
					right_white();
					direction = EAST;
					_delay_ms(500);
					white_path();
					place_block();
					white_path();
					current_pos = 9;
					right();
					direction = SOUTH;
					while(current_pos != 11){
						current_pos = forward_wls(1);
					}
					left();
					direction = EAST;
				}
				else if(current_pos > special_pick_2){
					left();
					direction = SOUTH;
					while(current_pos != special_pick_2){
						current_pos = forward_wls(1);
					}
					right();
					direction = EAST;
					pick_block();
					right();
					direction = NORTH;
					while(current_pos != 8){
						current_pos = forward_wls(1);
					}
					right_white();
					direction = EAST;
					_delay_ms(500);
					white_path();
					place_block();
					white_path();
					current_pos = 9;
					right();
					direction = SOUTH;
					while(current_pos != 11){
						current_pos = forward_wls(1);
					}
					left();
					direction = WEST;
				}
			}
		}
	}
}

int main(void)
{
	init();
	_delay_ms(500);
	special_house(3,12);
	for(int i=0;i<8;i++){
		path_following(pick_array[i],place_array[i]);
	}
	buzzer_on();
}
