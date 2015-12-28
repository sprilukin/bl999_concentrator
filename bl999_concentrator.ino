/*
 * An example of lib_bl999 library - library for Arduino to work with
 * BL999 temperature/humidity sensor
 * See README file in this directory for more documentation
 *
 * Author: Sergey Prilukin sprilukin@gmail.com
 */

#include <lib_bl999.h>
#include <esp8266_AT.h>
#include <SoftwareSerial.h>

//pin numbers of helper debug software serial
#define rx_pin 3
#define tx_pin 4


// replace value with access point name
#define ap_name "nord"

// replace value with access point password
#define ap_pass "asusn18urouter"

#define s_tryingToConnect "trying to connect"
#define s_connectedToWifi "connected to wifi"
#define s_payLoad "payload: "


#define bl_channel "channel="
#define bl_poweUid "&powerUUID="
#define bl_battery "&battery="
#define bl_temp "&temperature="
#define bl_hum "&humidity="

#define s_serverIp "192.168.1.47"
#define s_serverPort 3000
#define s_postMimeType "application/x-www-form-urlencoded"
#define s_serverUrl "/meteo"

//speed of the both wifi and debug serial ports should be the same.
//see speed for your version esp8266
#define SERIAL_SPEED 115200
#define SERIAL_TIMEOUT 8000

// RGB led pins
#define redPin 9
#define greenPin 10
#define bluePin 11

//SoftwareSerial debugSerial(rx_pin, tx_pin);
//Esp8266AT esp8266AT(&Serial, &debugSerial);
Esp8266AT esp8266AT(&Serial);

static BL999Info info;
boolean connected = false;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(SERIAL_SPEED);
    while (!Serial) { ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.setTimeout(SERIAL_TIMEOUT);

    //set digital pin to read info from
    bl999_set_rx_pin(2);

    //start reading data from sensor
    bl999_rx_start();

    // set the data rate for the SoftwareSerial port
    //debugSerial.begin(SERIAL_SPEED);
}

void loop() {
    if (!connected) {
        //debugSerial.println(s_tryingToConnect);
        //to allow wifi to start correctly
        purple();
        delay(8000);

        if (!esp8266AT.setup(ap_name, ap_pass)) {
            red();
        } else {
            //debugSerial.println(s_connectedToWifi);
            green();
            connected = true;
        }
    }

    if (connected) {

        //blocks until message will not be read
        bl999_wait_rx();

        //read message to info and if check sum correct - output it to serial port
        if (bl999_get_message(info)) {
            blue();

            //please replace ip, port request and response messages appropriately
            String payload = bl_channel + String(info.channel) + bl_poweUid + String(info.powerUUID) +
                             bl_battery + String(info.battery) + bl_temp + String(info.temperature / 10.0) + bl_hum +
                             String(info.humidity);

            //debugSerial.println(s_payLoad);
            //debugSerial.println(payload);

            if (!esp8266AT.post(s_serverIp, s_serverPort, s_serverUrl, s_postMimeType, payload, HTTP_200_OK)) {
                red();
            } else {
                green();
            }
        }
    }
}


void red() {
    setColor(255, 0, 0);  // red
}

void green() {
    setColor(0, 255, 0);  // green
}

void blue() {
    setColor(0, 0, 255);  // blue
}

void purple() {
    setColor(80, 0, 80);  // purple
}

void setColor(int red, int green, int blue) {
#ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
#endif
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
}
