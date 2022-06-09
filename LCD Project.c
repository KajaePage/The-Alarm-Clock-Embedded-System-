*
 * LCD PROJECT.c
 *
 * Created: 25/11/2020 1:36:34 AM
 * Author : Kajae Page
 */ 

// INCLUDES AND DEFFINTIONS

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

//CONFIGURATION OF MICROCONTROLLER

#define LCD_DATA_PRT PORTB		// Defines the the LCD data port as PORTB on the microcontroller
#define LCD_DATA_DDR DDRB		// Defines the the LCD data data direction register DDRB on the microcontroller
#define LCD_DATA_PIN PINB		// Defines the the LCD data PIN as PINB on the microcontroller
#define LCD_CNTRL_PRT PORTD		// Defines the the LCD control port as PORTD on the microcontroller
#define LCD_CNTRL_DDR DDRD		// Defines the the LCD control data direction register DDRD on the microcontroller
#define LCD_CNTRL_PIN PIND		// Defines the the LCD control PIN as PIND on the microcontroller
#define LCD_RS 5				// Defines the RS pin on the LCD to PD4 on the microcontroller
#define LCD_RW 6				// Defines the RW pin on the LCD to PD6 on the microcontroller
#define LCD_Enable 4			// Defines the enable pin on the LCD to PD4 on the microcontroller

// DEFINITIONS OF THE LCD FUNCTIONS

void LCD_send_command( unsigned char cmnd);				// a function that sends commands to the LCD display
void LCD_send_data(unsigned char data);					// a function that sends data to the LCD display	
void LCD_initialize(void);								// a function that initializes the LCD display 
void LCD_gotoPos(unsigned char x, unsigned char y);		// a function that goes to a specific position on the LCD display
void LCD_print( const char *data);						// a function that prints alphanumeric data on the LCD display

// DEFINITIONS OF INTERRUPTS AND TIMER FUNCTIONS

ISR(TIMER1_COMPA_vect);		// Timer for the digital clock
ISR(INT0_vect);				// Interrupt for setting the clock time 
ISR(INT1_vect);				// Interrupt for setting the clock alarm time

// DEFINITION OF GLOBAL VARIABLES 

unsigned char hours =12;		// Stores the value of the hours of the clock time 
unsigned char minutes = 58;		// Stores the value of the minutes of the clock time 
unsigned char seconds =58;		// Stores the value of the seconds of the clock time 

char display_secconds [2];		// allows the variable to store the seconds of the clock with 2 characters
char display_minutes [2];		// allows the variable to store the minutes of the clock with 2 characters
char display_hours [2];			// allows the variable to store the hours of the clock with 2 characters

int Alr_minutes = 0;			// Stores the value of the hours of the clock alarm time 
int Alr_hours = 0;				// Stores the value of the minutes of the clock alarm time

char display_Ahours[2];			// allows the variable to store the hours of the alarm time of the clock with 2 characters
char display_Aminutes[2];		// allows the variable to store the minutes of the alarm time of the clock clock with 2 characters

//APPLICATION CODE
int main(void)
{
	LCD_initialize(); // initializes LCD
	
	while (1)
	{
		LCD_gotoPos(1,1);						// Goes to the position in the first row and the first column of the LCD
		LCD_print("TIME:");						// Prints the word TIME: at the said position
		
		// UPDATES THE HOURS OF THE TIME ON THE DIGITAL CLOCK
		
		itoa(hours/10,display_hours,10);
		LCD_print(display_hours);
		itoa(hours%10,display_hours,10);
		LCD_print(display_hours);
		LCD_print(":");
		
		// UPDATES THE MINUTES OF THE TIME ON THE DIGITAL CLOCK
		
		itoa(minutes/10,display_minutes,10);
		LCD_print(display_minutes);
		itoa(minutes%10,display_minutes,10);
		LCD_print(display_minutes);
		LCD_print(":");
		
		// UPDATES THE SENCONDS OF THE TIME ON THE DIGITAL CLOCK
		
		itoa(seconds/10,display_secconds,10);
		LCD_print(display_secconds);
		itoa(seconds%10,display_secconds,10);
		LCD_print(display_secconds);
		
		// DISPLAYS IF THE ALARM IS ON OR OFF 
		
		if((Alr_hours == hours)&(Alr_minutes == minutes))	// Check if the alarm hours and minutes are the same as that of the clock time
		{
			LCD_gotoPos(1,2);								// Goes to the position in the second row and the first column of the LCD
			LCD_print("ALR:ON ");							// Prints the word ALR:ON at the said position
			PORTA |=(1<<PINA2);								// Sets PORTA PINA2 to HIGH which sounds the buzzer
		}
		else{
			
			LCD_gotoPos(1,2);								// Goes to the position in the second row and the first column of the LCD
			LCD_print("ALR:OFF ");							// Prints the word ALR:OFF at the said position
			PORTA &= ~ (1<<PINA2);							// Sets PORTA PINA2 to LOW which does not sounds the buzzer
		}
		
		LCD_gotoPos(9,2);									// Goes to the position in the second row and the ninth column of the LCD
		
		// UPDATES THE HOURS OF THE ALARM TIME ON THE DIGITAL CLOCK
		
		itoa(Alr_hours/10,display_Ahours,10);
		LCD_print(display_Ahours);
		itoa(Alr_hours%10,display_Ahours,10);
		LCD_print(display_Ahours);
		LCD_print(":");
		
		// UPDATES THE MINUTES OF THE ALARM TIME ON THE DIGITAL CLOCK
		
		itoa(Alr_minutes/10,display_Aminutes,10);
		LCD_print(display_Aminutes);
		itoa(Alr_minutes%10,display_Aminutes,10);
		LCD_print(display_Aminutes);

	}
	return 0;
}

// A FUNCTION THAT SENDS A COMMAND TO THE LCD DISPLAY
void LCD_send_command( unsigned char cmnd)
{
	LCD_DATA_PRT = cmnd;
	LCD_CNTRL_PRT &= ~ (1<<LCD_RS);
	LCD_CNTRL_PRT  |= (1<<LCD_Enable);
	_delay_ms(0.1);
	LCD_CNTRL_PRT  &= ~(1<<LCD_Enable);
	_delay_ms(0.1);
}

// A FUNCTION THAT SENDS DATA TO THE LCD DISPLAY
void LCD_send_data(unsigned char data)
{
	LCD_DATA_PRT = data;
	LCD_CNTRL_PRT |= (1<<LCD_RS);
	LCD_CNTRL_PRT |= (1<<LCD_Enable);
	_delay_ms(0.1);
	LCD_CNTRL_PRT &= ~ (1<<LCD_Enable);
	_delay_ms(0.1);
}

// A FUNCTION THAT INITIALIZES THE LCD DISPLAY AND CONFIGURATION OF THE MICROCONTROLLER 
void LCD_initialize()
{
	LCD_DATA_DDR = 0xFF;					// Makes PORTB output
	LCD_CNTRL_DDR = 0xFF;					// MAKES PRORTD input
	
	//CONFIGURES THE LCD PINS AND COMMANDS
	
	LCD_CNTRL_PRT  &= ~(1<<LCD_Enable);		
	_delay_ms(0.1);
	LCD_send_command(0x38);					// 2 lines and 5×7 matrix
	LCD_send_command(0x0E);					// LCD command to allow the display to be on with the cursor blinking
	LCD_send_command(0x01);					// LCD command to clear display screen
	_delay_ms(0.1);
	LCD_send_command(0x06);					// LCD command to increment cursor (shift cursor to right)
	
	//CONFIGUTATION OF MICROCONTROLLER
	
	DDRD &= ~(1<<2);						// Sets PIND2 to LOW
	DDRD &= ~(1<<3);						// Sets PIND3 to LOW
	DDRD &= ~(1<<1);						// Sets PIND1 to LOW
	PORTD |= (1<<2);						// Enables pull-up resistor  on PIND2
	PORTD |= (1<<3);						// Enables pull-up resistor  on PIND3
	PORTD |= (1<<1);						// Enables pull-up resistor  on PIND1
	MCUCR |= (1 << ISC11 | 1 << ISC01);		// External interrupt triggered on falling edge
	GIMSK |= (1 << INT0 | 1 << INT1);		// External interrupt request 0 and 1 enable
	TCCR1B |= (1<<CS12|1<<CS10|1<<WGM12);	// Configure Timer 1 in CTC (Clear timer on Compare match) mode
	OCR1A =950;								// Sets the OCR1A value
	sei();									// Enables global interrupts
	TIMSK |= (1<<OCIE1A);					// Output compare interrupt enabled
	
}

// A FUNCTION THAT GOES TO A SPECIFIC POSITION ON THE LCD DISPLAY
void LCD_gotoPos(unsigned char x, unsigned char y)
{
	unsigned char firstCharAddr[] = {0x80,0xC0, 0x94,0xD4};
	LCD_send_command(firstCharAddr[y-1] + x-1);
	_delay_ms(0.1);
}

// A FUNCTION THAT PRINTS ALPHANUMERIC CHARACTERS ON THE LCD DISPLAY
void LCD_print( const char *string)
{
	unsigned char i = 0;
	while(string[i]!=0)
	{
		LCD_send_data(string[i]);
		i++;
	}
}

// TIMER INTERRUPT
ISR(TIMER1_COMPA_vect)
{
	seconds++;
	if(seconds == 60)
	{
		seconds = 0;
		minutes++;
	}
	if(minutes ==60)
	{
		minutes=0;
		{
		}
		hours++;
	}
	if(hours >12)
	hours=1;
}

// INTERRUPT FOR SETTING THE CLOCK TIME 
ISR(INT0_vect)
{
	while (!(PIND & (1<<2)))
	{
		
		if (!(PIND &(1<<1)))
		{
			hours++;
			_delay_ms(50);
			if(hours>12)
			{
				hours = 1;
			}
		}
		else if (!(PIND & (1<<3)))
		{
			minutes ++;
			_delay_ms(50);
			if (minutes> 59)
			{
				minutes =0;
			}
		}
	}
}

// INTERRUPT FOR SETTING THE CLOCK ALARM TIME 
ISR(INT1_vect)
{
	while (!(PIND & (1<<3)))
	{
		if (!(PIND &(1<<1)))
		{
			Alr_hours++;
			_delay_ms(50);
			if(Alr_hours>12)
			{
				Alr_hours = 1;
			}
		}
		else if (!(PIND & (1<<2)))
		{
			Alr_minutes ++;
			_delay_ms(50);
			if (Alr_minutes> 59)
			{
				Alr_minutes =0;
			}
		}
	}
}
