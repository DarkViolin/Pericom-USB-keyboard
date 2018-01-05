/*
   2018-01-04 19:09
   Pericom serial terminal keyboard á 1983 - to - USB
   REV.1.0

   This is the first working version of my program. Media keys are not working, I think it is because the library isn't supporting them yet.
   Please mind the horrific nested if else statements and so on. I'm sorry. It looks utterly terrible, but it works!

   TODO NEXT: Something fun with the keyboard LED status lights and beeper


   //Joel Tegnér



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
It is sent continously, but it not required for the keyboard to be able to send key data

DATA sent from the keyboard (inverted TTL) looks like this:
    STARTBIT
    LSB   Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
          Bit = SCANCODE
    MSB   Bit = Key press/Key release (1=released, 0=pressed)
    STOPBIT
This data (scancode for the key and the keystate) is sent every time a key is pressed or released.



        -= Special keys =-
   ----------------------------
      F1-F4        = F1-F4
      PF1-PF12     = F1-F12
      PF13
      PF14
      PF15
      PF16
      PF17
      PF18         = DECREASE VOLUME
      PF19         = INCREASE VOLUME
      PF20         = MUTE SOUND
      PF21         = NUM LOCK
      PF22         = HOME
      PF23         = END
      PF24         = PAGE UP
      PF25         = PAGE DOWN
      LINE FEED    = ALT GR
      HOME         = WINDOWS KEY
      NO SCROLL    = LEFT ALT
      BREAK        = PAUSE/BREAK
*/

#include <Keyboard.h>

/* Lookup table for converting the scancodes to USB key codes */
static const uint16_t usb_key_codes[128] = {
  0,                     KEY_TAB,               KEY_DELETE,            KEY_LEFT_CTRL,
  KEY_LEFT_GUI,          KEY_RIGHT_ALT,         0,                     0,
  KEY_F1,                0,                     KEY_1,                 KEY_ESC,
  KEY_PAUSE,             0,                     0,                     0,
  KEY_PAGE_DOWN,         0,                     0,                     KEY_L,
  KEY_CAPS_LOCK,         KEY_LEFT_SHIFT,        KEY_COMMA,             KEY_PAGE_UP,
  0,                     KEY_F2,                KEY_9,                 KEY_0,
  KEY_END,               KEY_HOME,              KEY_MEDIA_MUTE,        KEY_SCROLL_LOCK,
  KEYPAD_ENTER,          KEY_I,                 KEY_Q,                 KEY_A,
  KEY_K,                 KEY_M,                 0,                     0,
  KEY_F3,                KEY_F10,               KEY_LEFT_GUI,          KEY_2,
  KEYPAD_MINUS,          KEY_COMMA,             KEY_F4,                KEY_MEDIA_VOLUME_INC,
  KEY_PERIOD,            KEY_W,                 KEY_O,                 KEY_J,
  KEY_S,                 KEY_Z,                 KEY_N,                 KEYPAD_3,
  KEY_F11,               KEY_F4,                KEY_3,                 KEY_8,
  KEYPAD_6,              0,                     KEY_MEDIA_VOLUME_DEC,  KEY_F3,
  KEYPAD_2,              KEY_RIGHT_BRACE,       KEY_E,                 KEY_D,
  KEY_ENTER,             KEY_RIGHT_SHIFT,       KEY_X,                 0,
  KEY_F5,                0,                     KEY_LEFT,              KEY_4,
  KEYPAD_8,              KEYPAD_5,              KEY_F2,                0,
  0,                     KEY_R,                 KEY_LEFT_BRACE,        KEY_QUOTE,
  KEY_F,                 KEY_C,                 KEY_SLASH,             0,
  0,                     KEY_F6,                KEY_5,                 KEY_DOWN,
  KEY_TILDE,             KEY_EQUAL,             0,                     0,
  KEYPAD_0,              KEY_P,                 KEY_T,                 KEY_G,
  KEY_SEMICOLON,         KEY_PERIOD,            KEY_V,                 KEYPAD_1,
  KEY_F7,                KEY_F12,               KEY_UP,                KEY_6,
  KEYPAD_4,              KEYPAD_7,              0,                     KEY_F1,
  0,                     KEY_Y,                 KEY_U,                 KEY_BACKSLASH,
  KEY_H,                 KEY_B,                 KEY_SPACE,             0,
  KEY_F9,                KEY_F8,                KEY_7,                 KEY_RIGHT,
  KEY_MINUS,             KEY_BACKSPACE,         0,                     0,
};

int x;                                            // Buffer prevention variable
int readByte = 0;                                 // RAW scancode from the keyboard
byte incomingByte = 0;                            // Refined scancode from the keyboard
byte newKEYstate;                                 // State of the current key
uint16_t newKEY;                                  // Converted scancode (USB key code)
uint16_t new_key_buffer[6];                       // Six slot key buffer, new keys
uint16_t old_key_buffer[6];                       // Six slot key buffer, old keys

void setup() {
  digitalWrite(11, HIGH);                         // Status light
  Keyboard.begin();                               // Initiate keyboard HID control
  Serial.begin(57600);                            // Initiate USB serial for debugging at 57600 baud
  Serial1.begin(1200);                            // Initiate RX(pin7) and TX(pin8) for communication with the keyboard at 1200 baud
}



void loop() {
  if (Serial1.available() > 0) {                  // Check if the UART has a new key to offer
    incomingByte = Serial1.read();                // Save the received key for later use
    newKEYstate = bitRead(incomingByte, 7);       // Remember if the key was pressed or released
    bitWrite(incomingByte, 7, 0);                 // Clean off that pesky keystate from the scancode, because we have already saved that
    newKEY = usb_key_codes[incomingByte];         // Translate the scancode into usb key code through
    x = 0;                                        // Reset the key buffer slot writing prevention mechanism

    /* Transfer the old keys to the old key buffer */
    for (int i = 0; i < 6; i++) {
      old_key_buffer[i] = new_key_buffer[i];      // Now the previous key buffer is saved as we pave way for the new key that is being pressed and put in the new key buffer
    }

    /* Check for CAPSLOCK or write/clear key to/from the key buffer */
    if (newKEY == KEY_CAPS_LOCK) {
      Keyboard.press(KEY_CAPS_LOCK);
      delay(10);
      Keyboard.release(KEY_CAPS_LOCK);
    } else {
      if (newKEYstate == 1) {                     // If the key was released
        for (int i = 5; i >= 0; i--) {            // Go through the key buffer array...
          if (new_key_buffer[i] == newKEY) {      // ...and see if the key was pressed earlier
            new_key_buffer[i] = 0;                // If it was, then clear the key slot so that the USB thing knows that the key is not pressed anymore
          }
        }
      } else if (newKEYstate == 0) {              // If the key was pressed
        for (int i = 5; i >= 0; i--) {            // Go through the key buffer array...
          if (new_key_buffer[i] == 0) {           // ...and look for a vacant key slot
            if (x == 0) {                         // Prevention check
              new_key_buffer[i] = newKEY;         // If a vacant key slot is found, store the pressed key there
              x = 1;                              // Prevent the key from being written to more than one slot
            }
          }
        }
      }
      for (int i = 5; i >= 0; i--) {
        if (new_key_buffer[i] != old_key_buffer[i] && new_key_buffer[i] == 0) {     // If the key differs from the old key buffer, and the new key is set as released
          Keyboard.release(old_key_buffer[i]);                                      // Write the change to the keyboard library
        } else if
        (new_key_buffer[i] != old_key_buffer[i] && old_key_buffer[i] == 0) {        // If the key differs from the old key buffer, and the old key vacant (no key pressed)
          Keyboard.press(new_key_buffer[i]);                                        // Write the change to the keyboard library
        }
      }
    }
  }
}
