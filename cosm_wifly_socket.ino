/*
 * WiFlyHQ Example httpclient.ino
 *
 * This sketch implements a simple Web client that connects to a 
 * web server, sends a GET, and then sends the result to the 
 * Serial monitor.
 *
 * This sketch is released to the public domain.
 *
 */

#include <WiFlyHQ.h>

#include <SoftwareSerial.h>
//Setting the TX & TRX pins, in this case (8,9) 
SoftwareSerial wifiSerial(8,9);

//#include <AltSoftSerial.h>
//AltSoftSerial wifiSerial(8,9);

WiFly wifly;

/* Change these to match your WiFi network */
const char mySSID[] = "internetz";
const char myPassword[] = "1nt3rn3tz";
char site[] = "api.cosm.com";

// Your Cosm key to let you upload data
char feedId[] = "104810"; //FEED ID
char cosmKey[] = "ewTCG0qri8i6jXsXxwXxnrAZpnKSAKxHL0tnbndNeEpPdz0g"; //API KEY
char sensorId[] = "wiflytest"; //This should not contain a space ' ' char

int counter =0;

//const char site[] = "arduino.cc";
//const char site[] = "www.google.co.nz";
//const char site[] = "arduino.cc";
//const char site[] = "google.com/search?q=arduino";

long interval = 1000;
long prevMillis =0;

void terminal();

void setup()
{
    char buf[32];

    Serial.begin(115200);
    Serial.println("Starting");
    Serial.print("Free memory: ");
    Serial.println(wifly.getFreeMemory(),DEC);

    wifiSerial.begin(9600);
    if (!wifly.begin(&wifiSerial, &Serial)) {
        Serial.println("Failed to start wifly");
	terminal();
    }

    /* Join wifi network if not already associated */
    if (!wifly.isAssociated()) {
	/* Setup the WiFly to connect to a wifi network */
	Serial.println("Joining network");
	wifly.setSSID(mySSID);
	wifly.setPassphrase(myPassword);
	wifly.enableDHCP();

	if (wifly.join()) {
	    Serial.println("Joined wifi network");
	} else {
	    Serial.println("Failed to join wifi network");
	    terminal();
	}
    } else {
        Serial.println("Already joined network");
    }

    //terminal();

    Serial.print("MAC: ");
    Serial.println(wifly.getMAC(buf, sizeof(buf)));
    Serial.print("IP: ");
    Serial.println(wifly.getIP(buf, sizeof(buf)));
    Serial.print("Netmask: ");
    Serial.println(wifly.getNetmask(buf, sizeof(buf)));
    Serial.print("Gateway: ");
    Serial.println(wifly.getGateway(buf, sizeof(buf)));

    wifly.setDeviceID("Wifly-WebClient");
    Serial.print("DeviceID: ");
    Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

    if (wifly.isConnected()) {
        Serial.println("Old connection active. Closing");
	wifly.close();
    }

    if (wifly.open(site, 8081)) {
        Serial.print("Connected to ");
	Serial.println(site);

	/* Send the request */
//	wifly.println("GET / HTTP/1.0");
//	wifly.println();
    } else {
        Serial.println("Failed to connect");
    }
}

void loop()
{
    unsigned long currentMillis = millis();
    if((currentMillis - prevMillis) > interval){
       sendDataToCosm(counter);
        counter++;
        prevMillis = currentMillis;
    }
   
    if (wifly.available() > 0) {
	char ch = wifly.read();
	Serial.write(ch);
	if (ch == '\n') {
	    /* add a carriage return */ 
	    Serial.write('\r');
	}
    }

    if (Serial.available() > 0) {
	wifly.write(Serial.read());
    }
}

void sendDataToCosm(int data){
  wifly.print("{\"method\" : \"put\",");
  wifly.print("\"resource\" : \"/feeds/");
  wifly.print(feedId);
  wifly.print("\", \"headers\" :{\"X-ApiKey\" : \"");
  wifly.print(cosmKey);
  wifly.print("\"},\"body\" :{ \"version\" : \"1.0.0\",\"datastreams\" : [{\"id\" : \"");
  wifly.print(sensorId);
  wifly.print("\",\"current_value\" : \"");
  wifly.print(data);
  wifly.print("\"}]}}");
  wifly.println();
}
/* Connect the WiFly serial to the serial monitor. */
void terminal()
{
    while (1) {
	if (wifly.available() > 0) {
	    Serial.write(wifly.read());
	}


	if (Serial.available() > 0) {
	    wifly.write(Serial.read());
	}
    }
}
