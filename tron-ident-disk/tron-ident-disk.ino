#include <Wire.h>
#include <EEPROM.h>

volatile int8_t r_pin = 5;
volatile int8_t g_pin = 3;
volatile int8_t b_pin = 2;

volatile bool stage_one_unlocked;
volatile bool stage_two_unlocked;

volatile int8_t red;
volatile int8_t green;
volatile int8_t blue;

volatile char handle[32] = "";
volatile int8_t handleLength = 0;
volatile uint8_t readMode = 0;


void setup() {

	pinMode(r_pin, OUTPUT);
  	pinMode(g_pin, OUTPUT);
  	pinMode(b_pin, OUTPUT);

	red = EEPROM.read(1);	
	green = EEPROM.read(2);	
	blue = EEPROM.read(3);	
	
	stage_one_unlocked = EEPROM.read(4);	
	stage_two_unlocked = EEPROM.read(5);	


	handleLength = EEPROM.read(6);

	for(int8_t i = 0; i < handleLength; i++){
		handle[i] = EEPROM.read(7+i);
	}
	
	Wire.begin(0x42);
  	Wire.onRequest(requestEvent);
  	Wire.onReceive(receiveEvent);
	
	setColor(red, green, blue, false);
}

void loop() {
}

void receiveEvent(){
	readMode = Wire.read();	
	if(readMode == 0x20){
		bool save = Wire.read();
		uint8_t r = Wire.read();
		uint8_t g = Wire.read();
		uint8_t b = Wire.read();
		setColor(r, g, b, save);
	}else if(readMode == 0x22){
		handleLength = Wire.read();
		EEPROM.write(6, handleLength);
		for(int8_t i = 0; i < handleLength; i++){
			char data = Wire.read();
			EEPROM.write(7+i, data);
			handle[i] = data;
		}
	}else if(readMode == 0x24){
		bool data = Wire.read() & 0x01;
		if(data != stage_one_unlocked){
			stage_one_unlocked = data;
			EEPROM.write(4, stage_one_unlocked);
		}
	}else if(readMode == 0x26){
		bool data = Wire.read() & 0x01;
		if(data != stage_two_unlocked){
			stage_two_unlocked = data;
			EEPROM.write(5, stage_one_unlocked);
		}
	}
		
}

void requestEvent(){
	if(readMode == 0x21){
		Wire.write(red);	
		Wire.write(green);	
		Wire.write(blue);	
	}else if(readMode == 0x23){
		Wire.write(handleLength);
		for(int8_t i = 0; i < handleLength; i++){
			Wire.write(handle[i]);
		}
	}else if(readMode == 0x25){
		Wire.write(stage_one_unlocked);
	}else if(readMode == 0x27){
		Wire.write(stage_two_unlocked);
	}
	readMode = 0x0;

}

void setColor(uint8_t r, uint8_t g, uint8_t b, bool save){
	analogWrite(r_pin, r ^ 0xFF);
  	analogWrite(g_pin, g ^ 0xFF);
  	analogWrite(b_pin, b ^ 0xFF);
	if(save){
		EEPROM.write(1, (uint8_t)r);
		EEPROM.write(2, (uint8_t)g);
		EEPROM.write(3, (uint8_t)b);
		red = r;	
		green = g;
		blue = b;
	}
}

