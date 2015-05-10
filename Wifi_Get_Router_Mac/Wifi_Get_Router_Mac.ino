#include <WiFi.h>

//SSID of your network 
char ssid[] = "yourNetwork";
//password of your WPA Network 
char pass[] = "secretPassword";

void setup()
{
  WiFi.begin(ssid, pass) ;
  /*
 if ( WiFi.begin(ssid, pass) != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected, print out info about the connection:
  else {
  // print the MAC address of the router you're attached to:
 */
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);
//  }
}

void loop ()
{

}
