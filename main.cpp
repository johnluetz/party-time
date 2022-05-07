#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <command_structure.h>

#define NUM_LEDS 50
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

// Set WiFi credentials
#define WIFI_SSID "free-utexas-wifi"
#define WIFI_PASS "fourwordsalluppercase"
#define UDP_PORT 4210
 
// UDP
WiFiUDP UDP;
IPAddress multicastAddr(224,1,1,1);
byte packet[255];
char reply[] = "Packet received!";

//packet
struct command
{ 
  uint32_t target;
  uint32_t commandType;
  uint32_t animation;
  uint32_t onTime;
  uint32_t offTime;
  uint32_t intensity;
  char primaryColor;
  char secondaryColor;
  char thirdColor;
};

// Light State
int deviceType = TARGET_LIGHT; //i am a light
int lastUpdate = 0;
bool lightState = false;
int lightCycle = 0;
int currentAnimation = ANIMATION_STROBE;
int currentOnTime = 1000;
int currentOffTime = 1000;
CRGB currentPrimaryColor = CRGB::Red;
CRGB currentSecondaryColor = CRGB::Green;
CRGB currentThirdColor = CRGB::Blue;

int currentIntensity = 255;

command paccit;
command recv;

void setup() {
  // Setup serial port
  Serial.begin(115200);
  Serial.println();
  
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(currentIntensity);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  
  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
 
  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  UDP.beginMulticast(multicastAddr, UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);

  // Initialize paccit with default values
  paccit.target = TARGET_LIGHT;
  paccit.commandType = TYPE_SET_ANIMATION;
  paccit.onTime = 1000;
  paccit.offTime = 1000;
  paccit.intensity = 
  paccit.primaryColor = 'R';
  paccit.secondaryColor = 'G';
  paccit.thirdColor = 'B';
  
}

void loop() {

  // If packet received...
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize);
    int len = UDP.read(packet, 255);
    if (len > 0)
    {
      packet[len] = '\0';
    }
    command *recv = reinterpret_cast<command*>(packet);
    paccit.target = recv->target;
    paccit.commandType = recv->commandType;
    paccit.animation = recv->animation;
    paccit.onTime = recv->onTime;
    paccit.offTime = recv->offTime;
    paccit.intensity = recv->intensity;
    paccit.primaryColor = recv->primaryColor;
    paccit.secondaryColor = recv->secondaryColor;
    paccit.thirdColor = recv->thirdColor;
    printPacket();



    /******** DECIDE WHAT TO DO ********/
    if (paccit.target == TARGET_LIGHT)
    {
      switch(paccit.commandType)
      {
        case TYPE_SYNC:
          syncDevice();
          break;
        case TYPE_SET_ANIMATION:
          setAnimation();
          break;
        case TYPE_SET_INTENSITY:
          setIntensity();
          break;
        case TYPE_SET_ONOFF:
          setOnOff();
          break;
        case TYPE_SET_COLORS:
          setColors();
          break;
        default:
          Serial.println("UNKNOWN COMMAND TYPE");
      }
    }
  } // end recv packet

  Animate();
}


void toggleStrobe(void)
{
  if(lightState)
  {
      for(int dot = 0; dot < NUM_LEDS; dot++)
      {
            leds[dot] = currentPrimaryColor;
      }
  }
  else
  {
      for(int dot = 0; dot < NUM_LEDS; dot++)
      {
            leds[dot] = currentSecondaryColor;
      }
  }
  FastLED.show();
  lightState = !lightState;
  lastUpdate = millis();
}

void cycleNext(void)
{
  CRGB cycleColor;
  if(lightCycle == 0)
    cycleColor = currentPrimaryColor;
  else if (lightCycle == 1)
    cycleColor = currentSecondaryColor;
  else
    cycleColor = currentThirdColor;
  
  for(int dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot] = cycleColor;
  }
  
  FastLED.show();
  lightCycle = (lightCycle + 1)%3; //cycle between 0-2
  lastUpdate = millis();

}

void printPacket(void)
{
    Serial.print("Target: ");
    Serial.println(paccit.target);
    Serial.print("Command Type: ");
    Serial.println(paccit.commandType);
    Serial.print("Animation: ");
    Serial.println(paccit.animation);
    Serial.print("On Time: ");
    Serial.println(paccit.onTime);
    Serial.print("Off Time: ");
    Serial.println(paccit.offTime);
    Serial.print("Intensity: ");
    Serial.println(paccit.intensity);
    Serial.print("Primary Color: ");
    Serial.println(paccit.primaryColor);
    Serial.print("Secondary Color: ");
    Serial.println(paccit.secondaryColor);
    Serial.print("Third Color: ");
    Serial.println(paccit.thirdColor);
}

void syncDevice(void)
{
  lastUpdate = 0;
  lightState = false;
  lightCycle = 0;
}

void setLight(bool state)
{
  if(state)
  {
      for(int dot = 0; dot < NUM_LEDS; dot++)
      {
            leds[dot] = currentPrimaryColor;
      }
  }
  else
  {
      for(int dot = 0; dot < NUM_LEDS; dot++)
      {
            leds[dot] = CRGB::Black;
      }
  }
  FastLED.show();
}

void setAnimation(void)
{
    currentAnimation = paccit.animation;
    currentOnTime = paccit.onTime;
    currentOffTime = paccit.offTime;
    currentPrimaryColor = convertColor(paccit.primaryColor);
    currentSecondaryColor = convertColor(paccit.secondaryColor);
    currentThirdColor = convertColor(paccit.thirdColor);

    if(currentIntensity != paccit.intensity)
    {
      setIntensity();
    }
}

void Animate(void)
{

  switch(currentAnimation)
  {
    case ANIMATION_STROBE:
      
      if (lightState)
      {
        if(millis() - lastUpdate > currentOnTime)
        {
          toggleStrobe();
        }
      }
      else if (!lightState)
      {
        if(millis() - lastUpdate > currentOffTime)
        {
          toggleStrobe();
        }
      }
      break;

    case ANIMATION_CYCLE:
      if (millis() - lastUpdate > currentOnTime)
      {
        cycleNext();
      }
      break;

    case ANIMATION_ON:
      setLight(true);
      break;
    case ANIMATION_OFF:
      setLight(false);
      break;

  }

}

void setIntensity(void)
{
  currentIntensity = paccit.intensity;
  FastLED.setBrightness(currentIntensity);
}

void setOnOff(void)
{
  currentOnTime = paccit.onTime;
  currentOffTime = paccit.offTime;
}

void setColors(void)
{
  currentPrimaryColor = convertColor(paccit.primaryColor);
  currentSecondaryColor = convertColor(paccit.secondaryColor);
  currentThirdColor = convertColor(paccit.thirdColor);
}

CRGB convertColor(char charColor)
{
  switch (charColor)
  {
  case 'R':
    return CRGB::Red;
  case 'G':
    return CRGB::Green;
  case 'B':
   return CRGB::Blue;
  
  case 'P':
    return CRGB::DeepPink;
  case 'O':
    return CRGB(255,130,5);

  case 'X':
   return CRGB::Black;

  default:
    return CRGB::Cyan;
  }
}