#pragma once

#include "wled.h"
#include <WiFiUdp.h>

// Maximum number of lights supported
<<<<<<< HEAD
#define MAX_WIZ_LIGHTS 10

// UDP object, to send messages
WiFiUDP UDP;

// Function to send a color to a light
void sendColor(IPAddress ip, uint32_t color) {
    UDP.beginPacket(ip, 38899);
    if (color == 0) {
      UDP.print("{\"method\":\"setPilot\",\"params\":{\"state\":false}}");
    } else {
      UDP.print("{\"method\":\"setPilot\",\"params\":{\"state\":true, \"r\":");
      UDP.print(R(color));
      UDP.print(",\"g\":");
      UDP.print(G(color));
      UDP.print(",\"b\":");
      UDP.print(B(color));
      UDP.print("}}");
    }
    UDP.endPacket();
}

// Create label for the usermode page (I cannot make it work with JSON arrays...)
String getJsonLabel(uint8_t i) {
  return "ip_light_" + String(i);
}

class WizLightsUsermod : public Usermod {
  private:
    // Keep track of the last time the lights were updated
    unsigned long lastTime = 0;

    // Specify how often WLED sends data to the Wiz lights
    long updateInterval;
    
    // Save the IP of the lights
    IPAddress lightsIP[MAX_WIZ_LIGHTS];
    bool lightsValid[MAX_WIZ_LIGHTS];

    // Variable that keeps track of RBG values for the lights
    uint32_t colorsSent[MAX_WIZ_LIGHTS];

  public:
    //Functions called by WLED

    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     */
    void loop() {
      // Calculate how long since the last update
      unsigned long ellapsedTime = millis() - lastTime;

      if (ellapsedTime > updateInterval) {
        // Keep track of whether we are updating any of the lights
        bool update = false;

        // Loop through the lights
        for (uint8_t i = 0; i < MAX_WIZ_LIGHTS; i++) {
          // Check if we have a valid IP
          if (!lightsValid[i]) { continue; }

          // Get the first colors in the strip
          uint32_t new_color = strip.getPixelColor(i);

          // Check if the color has changed from the last one sent
          //   Force an update every 5 minutes, in case the colors don't change
          //   (the lights could have been reset by turning off and on)
          if ((new_color != colorsSent[i]) | (ellapsedTime > 5*60000)) {
            // It has changed, send the new color to the light
            update = true;
            sendColor(lightsIP[i], new_color);
            colorsSent[i] = new_color;
          }
        }
        
        // We sent an update, wait until we do this again
        if (update) {
          lastTime = millis();
        }
      }
    }

    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("wizLightsUsermod");
      top["interval_ms"] = updateInterval;
=======
#define MAX_WIZ_LIGHTS 15

WiFiUDP UDP;




class WizLightsUsermod : public Usermod {
  
  private:
    unsigned long lastTime = 0;
    long updateInterval;
    long sendDelay;
    
    long forceUpdateMinutes;
    bool forceUpdate;

    bool useEnhancedWhite;
    long warmWhite;
    long coldWhite;

    IPAddress lightsIP[MAX_WIZ_LIGHTS];    // Stores Light IP addresses
    bool      lightsValid[MAX_WIZ_LIGHTS]; // Stores Light IP address validity
    uint32_t  colorsSent[MAX_WIZ_LIGHTS];  // Stores last color sent for each light



  public:



    // Send JSON blob to WiZ Light over UDP
    // RGB or C/W white
    // TODO:
    //   Better utilize WLED existing white mixing logic
    void wizSendColor(IPAddress ip, uint32_t color) {
      UDP.beginPacket(ip, 38899);

      // If no LED color, turn light off. Note wiz light setting for "Off fade-out" will be applied by the light itself.
      if (color == 0) { 
        UDP.print("{\"method\":\"setPilot\",\"params\":{\"state\":false}}");

      // If color is WHITE, try and use the lights WHITE LEDs instead of mixing RGB LEDs
      } else if (color == 16777215 && useEnhancedWhite){

        // set cold white light only
        if (coldWhite > 0 && warmWhite == 0){
          UDP.print("{\"method\":\"setPilot\",\"params\":{\"c\":"); UDP.print(coldWhite) ;UDP.print("}}");}
          
        // set warm white light only
        if (warmWhite > 0 && coldWhite == 0){
          UDP.print("{\"method\":\"setPilot\",\"params\":{\"w\":"); UDP.print(warmWhite) ;UDP.print("}}");}
          
        // set combination of warm and cold white light
        if (coldWhite > 0 && warmWhite > 0){
          UDP.print("{\"method\":\"setPilot\",\"params\":{\"c\":"); UDP.print(coldWhite) ;UDP.print(",\"w\":"); UDP.print(warmWhite); UDP.print("}}");}

      // Send color as RGB  
      } else {
        UDP.print("{\"method\":\"setPilot\",\"params\":{\"r\":");
        UDP.print(R(color));
        UDP.print(",\"g\":");
        UDP.print(G(color));
        UDP.print(",\"b\":");
        UDP.print(B(color));
        UDP.print("}}");
      }
    
      UDP.endPacket();
    }



    // TODO: Check millis() rollover
    void loop() {
      
      // Make sure we are connected first
      if (!WLED_CONNECTED) return;
      
      unsigned long ellapsedTime = millis() - lastTime;
      if (ellapsedTime > updateInterval) {
        bool update = false;
        for (uint8_t i = 0; i < MAX_WIZ_LIGHTS; i++) {
          if (!lightsValid[i]) { continue; }
          uint32_t newColor = strip.getPixelColor(i);
          if (forceUpdate || (newColor != colorsSent[i]) || (ellapsedTime > forceUpdateMinutes*60000)){
            wizSendColor(lightsIP[i], newColor);
            colorsSent[i] = newColor;
            update = true;
            delay(sendDelay);
            }
          }
        if (update) lastTime = millis();
        }
    }



    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("wizLightsUsermod");
      top["Interval (ms)"]                = updateInterval;
      top["Send Delay (ms)"]              = sendDelay;
      top["Use Enhanced White *"]         = useEnhancedWhite;
      top["* Warm White Value (0-255)"]   = warmWhite;
      top["* Cold White Value (0-255)"]   = coldWhite;
      top["Always Force Update"]          = forceUpdate;
      top["Force Update Every x Minutes"] = forceUpdateMinutes;
      
>>>>>>> fd4c0e795a4f70a0b35ab437a51f93d3f127544d
      for (uint8_t i = 0; i < MAX_WIZ_LIGHTS; i++) {
        top[getJsonLabel(i)] = lightsIP[i].toString();
      }
    }

<<<<<<< HEAD
    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     */
    bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      JsonObject top = root["wizLightsUsermod"];

      bool configComplete = !top.isNull();

      // Read interval to update the lights
      configComplete &= getJsonValue(top["interval_ms"], updateInterval, 1000);
=======


    bool readFromConfig(JsonObject& root)
    {
      JsonObject top = root["wizLightsUsermod"];
      bool configComplete = !top.isNull();

      configComplete &= getJsonValue(top["Interval (ms)"],                updateInterval,     1000);  // How frequently to update the wiz lights
      configComplete &= getJsonValue(top["Send Delay (ms)"],              sendDelay,          0);     // Optional delay after sending each UDP message
      configComplete &= getJsonValue(top["Use Enhanced White *"],         useEnhancedWhite,   false); // When color is white use wiz white LEDs instead of mixing RGB
      configComplete &= getJsonValue(top["* Warm White Value (0-255)"],   warmWhite,          0);     // Warm White LED value for Enhanced White
      configComplete &= getJsonValue(top["* Cold White Value (0-255)"],   coldWhite,          50);   // Cold White LED value for Enhanced White
      configComplete &= getJsonValue(top["Always Force Update"],          forceUpdate,        false); // Update wiz light every loop, even if color value has not changed
      configComplete &= getJsonValue(top["Force Update Every x Minutes"], forceUpdateMinutes, 5);     // Update wiz light if color value has not changed, every x minutes
>>>>>>> fd4c0e795a4f70a0b35ab437a51f93d3f127544d
      
      // Read list of IPs
      String tempIp;
      for (uint8_t i = 0; i < MAX_WIZ_LIGHTS; i++) {
        configComplete &= getJsonValue(top[getJsonLabel(i)], tempIp, "0.0.0.0");
        lightsValid[i] = lightsIP[i].fromString(tempIp);
        
        // If the IP is not valid, force the value to be empty
<<<<<<< HEAD
        if (!lightsValid[i]) {
          lightsIP[i].fromString("0.0.0.0");
        }
      }

      return configComplete;
    }
    
    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_WIZLIGHTS;
    }
};
=======
        if (!lightsValid[i]){lightsIP[i].fromString("0.0.0.0");}
        }

      return configComplete;
    }


   // Create label for the usermod page (I cannot make it work with JSON arrays...)
    String getJsonLabel(uint8_t i) {return "WiZ Light IP #" + String(i+1);}
        
    uint16_t getId(){return USERMOD_ID_WIZLIGHTS;}
};
>>>>>>> fd4c0e795a4f70a0b35ab437a51f93d3f127544d
