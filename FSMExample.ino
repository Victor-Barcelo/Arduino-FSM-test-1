#include <Wire.h>
#include <Time.h>

#define SLAVE_ADDRESS 0x05
#define DEBUG true

const int PIN_TEMPSENSOR = 0;
const int PIN_PUSHBUTTON = 7;

// Debounce para el botón: (http://arduino.cc/en/Tutorial/Debounce)
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50; 
//-------------------------

typedef enum {WaitingForOrders, ConfiguringTime, SendingSensorData, SendingButtonInfo} State;
State state;

typedef enum {RequestingSensorData, RequestingPushButtonTimes} RequestType;
RequestType requestType;

unsigned long pushButtonTimes[10];

void setup() {
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);
	// Wire.onRequest(sendSensorData);
	// Wire.onRequest(kk);
    Wire.onReceive(onRecieve);
	state = WaitingForOrders;
    if(DEBUG) Serial.println("Ready!");
}

void loop() {
  	delay(100);
	// switch (state) {
	//     case WaitingForOrders: 
 //      		checkForButtonPress();
	//       	break;
	//     case SendingSensorData:
	//     	sendSensorData();
	// 		state = WaitingForOrders;
	//     	break;
	//     case SendingButtonInfo: 
	//       	sendButtonInfo();
	// 		state = WaitingForOrders;
	//       	break;
	//     case ConfiguringTime: 
	//       	configureTime();
	// 		state = WaitingForOrders;
	//       	break;
	// }
}

void onRecieve(int byteCount){
	int cmd = Wire.read();
	Serial.println(cmd);
	if(cmd == 0) state = ConfiguringTime;
	if(cmd == 1) state = SendingSensorData;
	if(cmd == 2) state = SendingButtonInfo;
}

void kk(){}
void sendSensorData(){
	// int cmd = Wire.read();
	// Serial.println(cmd);

	byte data[2];
	int temperature = (125*analogRead(PIN_TEMPSENSOR))>>8; // raw to Celsius
  	if(DEBUG){
  		Serial.print("Temperatura: ");
  		Serial.println(temperature);
  	}
  	data[0] = lowByte(temperature);	
  	data[1] = highByte(temperature);	
	byte dataSend[] = {data[0],data[1]};
	Wire.write(dataSend, 2);
}

void checkForButtonPress(){
  	int reading = digitalRead(PIN_PUSHBUTTON);
  	if (reading != lastButtonState) {
    	lastDebounceTime = millis();
  	} 
  	if ((millis() - lastDebounceTime) > debounceDelay) {
    	if (reading != buttonState) {
      		buttonState = reading;
    		if (buttonState == HIGH) {
        		//Insert entry to pushButtonTimes
      		}
    	}
  	}
  	lastButtonState = reading;
}

void sendButtonInfo(){
	//send pushButtonTimes array
	//clear pushButtonTimes array
}

void configureTime(){
    unsigned long bytes[4];
    int i = 0;
    while(Wire.available()){
		bytes[i] = Wire.read();
		i++;
    }
	unsigned long result = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
	setTime(result);
}