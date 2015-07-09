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
  if(getLocation()){
    Serial.println("GPS is working ");
  }
}


boolean getLocation() {
    String content = "";
    char character;
    int complete = 0;
    char c;
    unsigned long commandClock = millis();                      // Start the timeout clock
    fonaSS.println("AT+CIPGSMLOC=1,1");
    while(!complete && commandClock <= millis()+ATtimeOut) { // Need to give the modem time to complete command
        while(!fonaSS.available() && commandClock <= millis()+ATtimeOut); //wait while there is no data
        while(fonaSS.available()) {   // if there is data to read...
            c = fonaSS.read();
            if(c == 0x0A || c == 0x0D) {
            } else {
                content.concat(c);
            }
        }
        if(content.startsWith("+CIPGSMLOC: 0,")) {
            Serial.println("Got Location"); //+CIPGSMLOC: 0,-73.974037,40.646976,2015/02/16,21:05:11OK
            Lon = content.substring(14, 24);
            Lat = content.substring(25, 34);
            Date = content.substring(35, 45);
            Time = content.substring(46,54);
            return 1;
        } else {
            Serial.print("ERROR: ");
            Serial.println(content);
            return 0;
        }
        complete = 1; //this doesn't work. 
    }
}

void setupGPRS() { //all the commands to setup a GPRS context and get ready for HTTP command
    //the sendATCommand sends the command to the FONA and waits until the recieves a response before continueing on. 
    Serial.print("Disable echo: ");
    if(sendATCommand("ATE0")) { //disable local echo
        Serial.println(response);
    }
    Serial.print("Set to TEXT Mode: ");
    if(sendATCommand("AT+CMGF=1")){ //sets SMS mode to TEXT mode....This MIGHT not be needed. But it doesn't break anything with it there. 
        Serial.println(response);
    }
    Serial.print("Attach GPRS: ");
    if(sendATCommand("AT+CGATT=1")){ //Attach to GPRS service (1 - attach, 0 - disengage)
        Serial.println(response);
    }
    Serial.print("Set Connection Type To GPRS: "); //AT+SAPBR - Bearer settings for applications based on IP
    if(sendATCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")){ //3 - Set bearer perameters
        Serial.println(response);
    }
    Serial.print("Set APN: ");
    if(setAPN()) {
        Serial.println(response);
    }
    if(sendATCommand("AT+SAPBR=1,1")) { //Open Bearer
        if(response == "OK") {
            Serial.println("Engaged GPRS");
        } else {
            Serial.println("GPRS Already on");
        }
    }
}

boolean sendATCommand(char Command[]) { //Send an AT command and wait for a response
    int complete = 0; // have we collected the whole response?
    char c; //capture serial stream
    String content; //place to save serial stream
    unsigned long commandClock = millis(); //timeout Clock
    fonaSS.println(Command); //Print Command
    while(!complete && commandClock <= millis() + ATtimeOut) { //wait until the command is complete
        while(!fonaSS.available() && commandClock <= millis()+ATtimeOut); //wait until the Serial Port is opened
        while(fonaSS.available()) { //Collect the response
            c = fonaSS.read(); //capture it
            if(c == 0x0A || c == 0x0D); //disregard all new lines and carrige returns (makes the String matching eaiser to do)
            else content.concat(c); //concatonate the stream into a String
        }
        //Serial.println(content); //Debug
        response = content; //Save it out to a global Variable (How do you return a String from a Function?)
        complete = 1;  //Lable as Done.
    }
    if (complete ==1) return 1; //Is it done? return a 1
    else return 0; //otherwise don't (this will trigger if the command times out) 
    /*
        Note: This function may not work perfectly...but it works pretty well. I'm not totally sure how well the timeout function works. It'll be worth testing. 
        Another bug is that if you send a command that returns with two responses, an OK, and then something else, it will ignore the something else and just say DONE as soon as the first response happens. 
        For example, HTTPACTION=0, returns with an OK when it's intiialized, then a second response when the action is complete. OR HTTPREAD does the same. That is poorly handled here, hence all the delays up above. 
    */
}


boolean setAPN() { //Set the APN. See sendATCommand for full comments on flow
    int complete = 0;
    char c;
    String content;
    unsigned long commandClock = millis();                      // Start the timeout clock
    fonaSS.print("AT+SAPBR=3,1,\"APN\",\"");
    fonaSS.print(APN);
    fonaSS.print("\"");
    fonaSS.println();
    while(!complete && commandClock <= millis() + ATtimeOut) {
        while(!fonaSS.available() && commandClock <= millis()+ATtimeOut);
        while(fonaSS.available()) {
            c = fonaSS.read();
            if(c == 0x0A || c == 0x0D);
            else content.concat(c);
        }
        response = content;
        complete = 1; 
    }
    if (complete ==1) return 1;
    else return 0;
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

