




/****************************************************************************
CAN Read Demo for the SparkFun CAN Bus Shield. 

Written by Stephen McCoy. 
Original tutorial available here: http://www.instructables.com/id/CAN-Bus-Sniffing-and-Broadcasting-with-Arduino
Used with permission 2016. License CC By SA. 

also used example by Sandeep Mistry. All rights reserved.
Licensed under the MIT license. See LICENSE file in the project root for full license information.

Distributed as-is; no warranty is given.

This Arduino Sketch aims to provide an interface solution to Continental Corporation UNI NOx Sensor.
The sensor is capable of reading both the O2 [%] and the NOx [ppm].
On cold switch ON, the sensor requires few minutes to warm up. During this time, the serial monitor
shows "Sensor NOT OK ... wait". Once the sensor warms up, the value of O2 [%] and NOx [ppm] settles down
to the actual values and the serial monitor shows "Sensor OK".

Sensor OK => 
- Sensor Supply Status: Supply in Range
- Sensor Temperature Status: At Temperature
- NOx Signal Status: Valid
- O2 Signal Status: Valid
- Heater Mode Status: Auto Mode
- Heater Circuit Status: No Error
- NOx Circuit Status: No Error
- O2 Circuit Status: No Error
*************************************************************************/

#include <CAN.h>   // part of CAN-0.3.1.zip
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

//********************************Setup Loop*********************************//
char booted =0;
float O2=0;
float NOx=0;
unsigned int UNIdata0;
unsigned int UNIdata1;
unsigned int UNIdata2;
unsigned int UNIdata3;
unsigned int UNIdata4;
unsigned int UNIdata5;
unsigned int UNIdata6;
unsigned int UNIdata7;

int d=500;  // delay to not have to much printing on screen , also used in the time to resend ON command
float i=0;  // counter to keep some timing to know when to resend Heater ON command

void setup() {
  Serial.begin(9600); // For debug use
  Serial.println("CAN Read - Testing receival of CAN Bus message");  
  delay(1000);
  
  if(Canbus.init(CANSPEED_250))  //Initialise MCP2515 CAN controller at the specified speed, UNI NOx is 250 CAn bus speed
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
    
  delay(1000);
}

//********************************Main Loop*********************************//

void loop(){
if (booted==0)
{
  // sending the On command copy of program from CANSender by Sandeep Mistry for the extended 29 bit ID of the Uni Nox sensor
  
  CAN.beginExtendedPacket(0x18FEDF00);
  CAN.write(0x0);
  CAN.write(0x0);
  CAN.write(0x0);
  CAN.write(0x0);
  CAN.write(0x0);
  CAN.write(0x0);
  CAN.write(0x0);
  CAN.write(0x04);
  CAN.endPacket();
  booted = 1;
} // end of sending ON message  

 // try to parse packet
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.print(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.print(packetSize);

      // only print packet data for non-RTR packets
     // while (CAN.available()) {
      //  Serial.print((char)CAN.read());
     // }

                UNIdata0 = (unsigned int)CAN.read();
                UNIdata1 = (unsigned int)CAN.read();
                UNIdata2 = (unsigned int)CAN.read();
                UNIdata3 = (unsigned int)CAN.read();
                UNIdata4 = (unsigned int)CAN.read();
                UNIdata5 = (unsigned int)CAN.read();
                UNIdata6 = (unsigned int)CAN.read();
                UNIdata7 = (unsigned int)CAN.read();

                 if((UNIdata4==0x55) and (UNIdata5==0x9F) and (UNIdata6==0x1F) and (UNIdata7==0xFF))
                  {
                  Serial.print(" Sensor OK "); // see notes above
                  }
                  else

                  Serial.print(" Sensor NOT OK ... Wait ");  // see notes above
                 

                Serial.print(" NOx ppm is ");
                NOx=(float)((((UNIdata1*256)+UNIdata0)*(0.049971))-200);
                Serial.print(NOx);
                Serial.print(" O2 % is ");
                O2=(float)((((((UNIdata3*256))+UNIdata2))*(0.000514))-12);
                      
                Serial.print(O2);
                
                Serial.println("");
                               
               delay(500);
               i++;
               if(i>120)    // here the number of iterations before resending on command is calculated i.e. approx 1 minute divided by delay
                {
                 i=0; 
                 booted=0;
                }
             //}
           }}

}
