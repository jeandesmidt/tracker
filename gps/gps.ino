#include <SoftwareSerial.h>

//Pin configuration
#define FONA_RX 2
#define FONA_TX 3
#define FONA_KEY 6
#define FONA_PS 7

String APN = "internet";
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
String response; //pulling AT command responses
int keyTime = 2000; // Time takes for fona to turn on
unsigned long ATtimeOut = 10000; //Time given for an at command to complete

//Sparkfun server applicable
const String publicKey = "";
const String privateKey = "";
const byte NUM_FIELDS = 4; //Number of fields in data stream
const String fieldNames[NUM_FIELDS] = {"latitude","longtitude","date","time"};
String fieldData[NUM_FIELDS]; // holder of date values

String Lat;
String Lon;
String Date;
String Time;

void setup () {
	pinMode(FONA_PS, INPUT);
	pinMode(FONA_KEY, OUTPUT);
	digitalWrite(FONA_KEY, HIGH); //To turn on device make it low for 2 seconds
	Serial.begin(9600);
	Serial.println("Started setup");
	fonaSS.begin(9600);
}

void loop (){
	//1. Turn on fona
	//2. SetupGPRS
	//3. Get location
	//wait x amount of seconds then get location again
	//4. Compare locations
	//increment counter if locations are the same
	//5. switch off tracker if counter is x
	//6. switch device off if tracker is x
	//switch device back on if user presses track
	//send location to server if valid data
	turnOnFona();
	delay(10000);
	turnOffFona();
	delay(10000);
}

void turnOnFona(){
	if(!digitalRead(FONA_PS)){
		Serial.print("Device is turning on");
		digitalWrite(FONA_KEY,LOW);
		unsigned long KeyPress = millis();
		while(KeyPress +keyTime >= millis()){}
		digitalWrite(FONA_KEY,HIGH);
		Serial.println("FONA Powered UP");	
	}
	else{
		Serial.println("Fona is already on.");
	}
}

void turnOffFona(){
        if(digitalRead(FONA_PS)){
                Serial.print("Device is turning off");
                digitalWrite(FONA_KEY,LOW);
                unsigned long KeyPress = millis();
                while(KeyPress +keyTime >= millis()){}
                digitalWrite(FONA_KEY,HIGH);
                Serial.println("FONA Powered UP");  
        }
        else{
		    Serial.println("Fona is already off.");
        }
}

