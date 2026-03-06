/*
   2026-03-06 13:57
   Pericom serial terminal keyboard á 1983 - to - USB
   Written for Teensy 2.0 using Arduino IDE
   REV.1.3.2
   Joel Tegnér

   Functions in this version:
   All keys work
   Media keys work
   Capslock, numlock and scrollock work and they check what the host status is for the LED lights
   <>| key works now

   Added in this version:
   * Jiggler with SETUP+J


   To program this code into the Teensy 2.0, install Arduino IDE, then add the teensy library from within
   Arduino IDE. Select board Teensy 2.0, select usb type Keyboard + Mouse Joystick. Use the regular
   Upload button in the IDE to upload. Install the "Keyboard" library from withtin Arduino IDE.

   This project was inspired by Seth's VT100 to USB project on github.
   The VT100 and Pericom keyboards share lots of similarities on the outside, but the insides are completely
   different both by protocol and also the electronics. This keyboard also has twentyfive extra function keys
   which makes my life so much easier.
   Switches in this keyboard (made by Rotec) are pretty much identical to the SOL-20 computer's keyboard.
   However, the connection between the pad and the board is made conductively in this one instead of direct
   contact to the traces.
   Each key press or release sends one scancode containing 7 bits of keydata and one MSB bit of press/release data.
   The CAPSLOCK key physically toggles which means that I have to keep track of the state (creates a problem
   when pressed down while powering on the PC because the PC has it inactivated in software at boot).

   Keyboard is fed with 5V, drawing around 0.5A. This makes it possible to connect it directly to the PC,
   eliminating any extra power adapter.

  DATA sent from the terminal (inverted TTL) looks like this:
    STARTBIT
    LSB   Bit = Online / Local
          Bit = Setup A-B-C
          Bit = L1 Toggle I/O
          Bit = L2 TX Speed
          Bit = L3 RX Speed
          Bit = L4 80/132
          Bit = L5 RESET
    MSB   Bit = Beeper signal
    STOPBIT
  It is originally sent continously from the terminal, but it not required for the keyboard to be able to send key data

  DATA sent from the keyboard (inverted TTL) looks like this:
    STARTBIT
    LSB   Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
    MSB   Bit = Key press/Key release (1=released, 0=pressed, reversed in code to release=false and pressed=true)
    STOPBIT
  This data (scancode for the key and the keystate) is sent every time a key is pressed or released.

        -= Special keys =-
   ----------------------------
  111 F1           = 
   78 F2           = 
   63 F3           = 
   46 F4           = KEYPAD +
      PF1-PF12     = F1-F12
   88 PF13         = 
   73 PF14         = 
  126 PF15         = 
  110 PF16         = SCROLL LOCK
   79 PF17         = NUM LOCK
   62 PF18         = LOWER VOL
   47 PF19         = HIGHER VOL
   30 PF20         = MUTE VOL
   31 PF21         = 
   29 PF22         = HOME
   28 PF23         = END
   23 PF24         = PAGE UP
   16 PF25         = PAGE DOWN
    5 LINE FEED    = ALT GR
   42 HOME         = WINDOWS KEY
    3 NO SCROLL    = LEFT ALT
   12 BREAK        = Prints a pipe (Swedish keyboard has en extra key for this)
      SETUP + C    = Toggle keyclick
      SETUP + J    = Toggle Jiggler (win key every minute)
      SETUP + Z    = Escape you bosses wraith
*/

#include <Keyboard.h>
extern uint8_t keyboard_leds;  // Host LED state: bit0/L1 Num, bit1/L2 Caps, bit2/L3 Scroll





/* Lookup table for converting the scancodes to USB key codes */
static const uint16_t usb_key_codes[128] = {
  0, //NOKEY
  KEY_TAB,
  KEY_DELETE,
  KEY_LEFT_ALT,
  KEY_LEFT_CTRL,
  KEY_RIGHT_ALT,
  0, //NOKEY
  0, //NOKEY
  KEY_F1, //PF1
  0, //NOKEY
  KEY_1,
  KEY_ESC,
  KEY_NON_US_BS,
  0, //NOKEY
  0, //NOKEY
  0, //NOKEY
  KEY_PAGE_DOWN,
  0, //NOKEY
  0, //NOKEY
  KEY_L,
  KEY_CAPS_LOCK,
  KEY_LEFT_SHIFT,
  KEY_COMMA,
  KEY_PAGE_UP,
  0, //NOKEY
  KEY_F2, //PF2
  KEY_9,
  KEY_0,
  KEY_END,
  KEY_HOME,
  KEY_MEDIA_MUTE,
  KEY_I, 
  KEYPAD_ENTER,
  KEY_I,
  KEY_Q,
  KEY_A,
  KEY_K,
  KEY_M,
  0, //NOKEY
  0, //NOKEY
  KEY_F3, //PF3
  KEY_F10, //PF10
  KEY_LEFT_GUI,
  KEY_2,
  KEYPAD_MINUS,
  KEY_COMMA,
  KEYPAD_PLUS, //F4
  KEY_MEDIA_VOLUME_INC,
  KEY_PERIOD,
  KEY_W,
  KEY_O,
  KEY_J,
  KEY_S,
  KEY_Z,
  KEY_N,
  KEYPAD_3,
  KEY_F11, //PF11
  0, //NOKEY
  KEY_3,
  KEY_8,
  KEYPAD_6,
  KEYPAD_9,
  KEY_MEDIA_VOLUME_DEC,
  0, //F3
  KEYPAD_2,
  KEY_RIGHT_BRACE,
  KEY_E,
  KEY_D,
  KEY_ENTER,
  KEY_RIGHT_SHIFT,
  KEY_X,
  0, //NOKEY
  KEY_F5, //PF5
  0, //PF14
  KEY_LEFT,
  KEY_4,
  KEYPAD_8,
  KEYPAD_5,
  0, //F2
  KEY_NUM_LOCK, //PF17
  0, //NOKEY
  KEY_R,
  KEY_LEFT_BRACE,
  KEY_QUOTE,
  KEY_F,
  KEY_C,
  KEY_SLASH,
  0, //NOKEY
  0, //PF13
  KEY_F6, //PF6
  KEY_5,
  KEY_DOWN,
  KEY_TILDE,
  KEY_EQUAL,
  0, //NOKEY
  0, //NOKEY
  KEYPAD_0,
  KEY_P,
  KEY_T,
  KEY_G,
  KEY_SEMICOLON,
  KEY_PERIOD,
  KEY_V,
  KEYPAD_1,
  KEY_F7, //PF7
  KEY_F12, //PF12
  KEY_UP,
  KEY_6,
  KEYPAD_4,
  KEYPAD_7,
  KEY_SCROLL_LOCK, //PF16
  0, //F1
  0, //NOKEY
  KEY_Y,
  KEY_U,
  KEY_BACKSLASH,
  KEY_H,
  KEY_B,
  KEY_SPACE,
  0, //NOKEY
  KEY_F9, //PF9
  KEY_F8, //PF8
  KEY_7,
  KEY_RIGHT,
  KEY_MINUS,
  KEY_BACKSPACE,
  0, //PF15
  0, //NOKEY
};

bool buff_prev = false;          // Buffer prevention variable
byte incomingByte = 0;           // Refined scancode from the keyboard
bool newKEYstate = false;        // State of the current key. 1=Key was just pressed, and 0=Key was just released.
uint16_t newKEY;                 // Converted scancode (USB key code)
uint16_t new_key_buffer[6];      // Six slot key buffer, new keys
uint16_t old_key_buffer[6];      // Six slot key buffer, old keys
bool keyclick_state = false;     // Enables or disables keyclick
int kbd_LED = 0;                 // Bits for statuslights
bool SETUP_state = false;        // 1=Pressed, 0=Released. Remember this for the setup handling
const byte SETUP_SCAN_CODE = 9;  // This is the SET-UP key
uint8_t last_kbd_LED = 0;
bool jiggler_active = false;
unsigned long jiggler_last_time = 0;
unsigned long jiggler_led_last_time = 0;
bool jiggler_led_on = false;

// Bit masks for the keyboard's LED/status byte
const uint8_t LED_ONLINE_LOCAL = 0x01;  // bit 0 – Caps Lock indicator
const uint8_t LED_SETUP_ABC    = 0x02;  // bit 1 – Keyclick indicator
const uint8_t LED_L1           = 0x04;  // bit 2 – free (Toggle I/O), not used for locks
const uint8_t LED_L2           = 0x08;  // bit 3 – free (TX Speed), not used for locks
const uint8_t LED_L3           = 0x10;  // bit 4 – free (RX Speed), not used for locks
const uint8_t LED_L4           = 0x20;  // bit 5 – Scroll Lock indicator
const uint8_t LED_L5           = 0x40;  // bit 6 – Num Lock indicator
const uint8_t LED_BEEP         = 0x80;  // bit 7 – Beeper

void (*resetFunc)(void) = 0;  // Software reset function

void setup() {
  delay(1400);             // Wait for the keyboard
  digitalWrite(11, HIGH);  // Status light on Teensy board
  Keyboard.begin();        // Initiate keyboard HID control
  Serial.begin(57600);     // Initiate USB serial for debugging at 57600 baud
  Serial1.begin(1200);     // Initiate RX(pin7) and TX(pin8) for communication with the keyboard at 1200 baud
  startup_routine();       // Blink LEDs and make cool sounds
}

void loop() {
  unsigned long now = millis();

  // Mouse-jiggler timing
  if (jiggler_active) {
    if (now - jiggler_last_time >= 60000UL) { // Set time interval for the jiggler here
      jiggler_last_time = now;
      Keyboard.press(KEY_LEFT_GUI);
      delay(30);
      Keyboard.release(KEY_LEFT_GUI);
    }

    // Blink LED L1 every 0.5 seconds
    if (now - jiggler_led_last_time >= 500UL) {
      jiggler_led_last_time = now;
      jiggler_led_on = !jiggler_led_on;
    }
  } else {
    jiggler_led_on = false;
  }

  // Always keep LEDs in sync with host/keyclick/jiggler
  update_kbd_leds_from_host();
  if (kbd_LED != last_kbd_LED) {
    Serial1.write(kbd_LED);
    last_kbd_LED = kbd_LED;
  }

  if (Serial1.available() <= 0) return;  // Exit loop until UART has a new key to offer

  incomingByte = Serial1.read();               // Save the received key for later use
  newKEYstate = !((incomingByte & 128) >> 7);  // Remember if the key was pressed or released by reading the keystate bit
  incomingByte &= ~(1 << 7);                   // Clean off that pesky keystate bit from the scancode, because we have already saved that
  newKEY = usb_key_codes[incomingByte];        // Translate the scancode into usb key code through the lookup table
  buff_prev = false;                           // Reset the key buffer slot writing prevention mechanism thingy

  if (keyclick_state && newKEYstate) {  // Click for every key if it was enabled with SETUP + C
    kbd_BEEP(1);
  }

  // Transfer the old keys to the old key buffer so that the new key can be compared to these later
  for (int i = 0; i < 6; i++) {
    old_key_buffer[i] = new_key_buffer[i];  // Now the previous key buffer is saved as we pave way for the new key that is being pressed and put into the new key buffer
  }

  // Handle the SETUP key
  if (incomingByte == SETUP_SCAN_CODE) {
    SETUP_state = newKEYstate;  // Remember if the SETUP key was pressed
  }

  // Handle the keyclick engagement (if SETUP key was pressed, and new key is pressed which is not the SETUP key)
  else if (SETUP_state && newKEYstate) {
    setuphandler();  // If SETUP key was pressed together with another key
  }

  // Handle the capslock key
  else if (newKEY == KEY_CAPS_LOCK) {
    toggleCAPSLOCK();  // Toggle CAPSLOCK state and LED light
  }

  // Handle the scrollock key
  else if (newKEY == KEY_SCROLL_LOCK) {
    toggleSCOLL_LOCK();  // Toggle SCROLL LOCK state and LED light
  }

  // Write/clear regular keys to/from the key buffer. Compare to see what was released and pressed.
  else {
    if (!newKEYstate) {                     // If the key was released (false)
      for (int i = 5; i >= 0; i--) {        // Go through the key buffer array...
        if (new_key_buffer[i] == newKEY) {  // ...and see if the key was pressed earlier
          new_key_buffer[i] = 0;            // If it was, then clear the key slot so that the USB thing knows that the key is not pressed anymore
        }
      }
    } else {                                         // If the key was pressed (true)
      for (int i = 5; i >= 0; i--) {                 // Go through the key buffer array...
        if (new_key_buffer[i] == 0 && !buff_prev) {  // ...and look for a vacant key slot
          new_key_buffer[i] = newKEY;                // If a vacant key slot is found, store the pressed key there
          buff_prev = true;                          // Prevent the key from being written to more than one slot
        }
      }
    }
    for (int i = 5; i >= 0; i--) {
      if (new_key_buffer[i] != old_key_buffer[i] && new_key_buffer[i] == 0) {         // If the key differs from the old key buffer, and the new key is set as released
        Keyboard.release(old_key_buffer[i]);                                          // Write the change to the keyboard library
      } else if (new_key_buffer[i] != old_key_buffer[i] && old_key_buffer[i] == 0) {  // If the key differs from the old key buffer, and the old key is vacant (no key pressed), then a new key has been pressed
        Keyboard.press(new_key_buffer[i]);                                            // Write the change to the keyboard library
      }
    }
  }
}

/* Handles the key that was pressed with the SETUP key */
void setuphandler() {
  switch (newKEY) {
    case KEY_C:
    keyclick_state = !keyclick_state;  // Toggle the keyclick state
    break;

    case KEY_J:
    // Toggle mouse-jiggler
    jiggler_active = !jiggler_active;
    if (!jiggler_active) {
      jiggler_led_on = false;
      jiggler_last_time = 0;
      jiggler_led_last_time = 0;
    } else {
      unsigned long now = millis();
      jiggler_last_time = now;
      jiggler_led_last_time = now;
    }
    break;
  }
}

void update_kbd_leds_from_host() {
  uint8_t new_led = 0;

  // Keyclick LED on Setup A‑B‑C (bit 1)
  if (keyclick_state) {
    new_led |= LED_SETUP_ABC;
  }

  // Preserve beeper state if currently on
  if (kbd_LED & LED_BEEP) {
    new_led |= LED_BEEP;
  }

  // Map host LED bits:
  // host bit1 = Capslock → Online/Local (bit 0)
  if (keyboard_leds & 0x02) {
    new_led |= LED_ONLINE_LOCAL;
  }

  // host bit2 = Scrollock → L4 (bit 5)
  if (keyboard_leds & 0x04) {
    new_led |= LED_L4;
  }

  // host bit0 = Numlock → L5 (bit 6)
  if (keyboard_leds & 0x01) {
    new_led |= LED_L5;
  }

  // Mouse-jiggler indicator: blink L1 when active
  if (jiggler_active && jiggler_led_on) {
    new_led |= LED_L1;
  }

  kbd_LED = new_led;
}


/* Toggles CAPSLOCK (tell the host) */
void toggleCAPSLOCK() {
  Keyboard.press(KEY_CAPS_LOCK);
  delay(10);
  Keyboard.release(KEY_CAPS_LOCK);
}

/* Toggles SCROLL LOCK (tell the host) */
void toggleSCOLL_LOCK() {
  if (newKEYstate) {   // only on key press, not release
    Keyboard.press(KEY_SCROLL_LOCK);
    delay(10);
    Keyboard.release(KEY_SCROLL_LOCK);
  }
}

/* Beeps with specified duration in mS. 1 mS for click. */
void kbd_BEEP(int duration) {
  kbd_LED ^= LED_BEEP;      // Flip the msb BIT controlling the beeper
  Serial1.write(kbd_LED);
  delay(duration);
  kbd_LED ^= LED_BEEP;
  Serial1.write(kbd_LED);
}

/* LED blinks on startup */
void startup_routine() {
  kbd_LED = 1;
  for (int i = 0; i < 6; i++) {
    Serial1.write(kbd_LED);
    kbd_LED = kbd_LED << 1;
    delay(50);
  }
  for (int i = 0; i < 7; i++) {
    Serial1.write(kbd_LED);
    kbd_LED = kbd_LED >> 1;
    delay(50);
  }
  for (int i = 0; i < 3; i++) {
    kbd_LED = 126;
    Serial1.write(kbd_LED);
    kbd_BEEP(120);
    kbd_LED = 0;
    Serial1.write(kbd_LED);
    delay(80);
  }
}
