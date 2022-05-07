/* READ ME

PACKET FORMAT: 
  uint32_t target;
  uint32_t commandType;
  uint32_t animation;
  uint32_t onTime;
  uint32_t offTime;
  uint32_t intensity;
  char primaryColor;
  char secondaryColor;
  char thirdColor;

  TYPE SYNC

/*


/* TARGET DEVICE */
#define TARGET_LIGHT 0
#define TARGET_FOG 1
#define TARGET_LASER 2

/* COMMAND TYPE */
#define TYPE_SYNC 0
#define TYPE_ON 1
#define TYPE_OFF 2
#define TYPE_SET_ANIMATION 3
#define TYPE_SET_INTENSITY 4
#define TYPE_SET_ONOFF 5
#define TYPE_SET_COLORS 6

/* ANIMATION */
#define ANIMATION_STROBE 0
#define ANIMATION_CYCLE 1
#define ANIMATION_CHASE 2
#define ANIMATION_ON 3
#define ANIMATION_OFF 4

// Toggle between primary and secondary color
void toggleStrobe(void);

// Prints out the packet
void printPacket(void);

// Set device to known state (zero timer, light off)
void syncDevice(void);

// Set light to on/off
void setLight(bool state);

// Set animation
void setAnimation(void);

// Run Animations
void Animate(void);

// Set global brightness
void setIntensity(void);

// Set onoff times
void setOnOff(void);

// Set Colors!
void setColors(void);

// Convert from character color into CRGB
CRGB convertColor(char charColor);