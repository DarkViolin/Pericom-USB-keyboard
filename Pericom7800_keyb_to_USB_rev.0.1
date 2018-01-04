/*
   2018-01-03 20:50
   Pericom serial terminal keyboard á 1983 - to - USB


   This program prints out the keyboard buffer and key state for the current key press.
   I'm having problems fetching the usb key codes from the lookup table. The variables returns bad
   key codes for some reason. TAB should for example return 43, and not 65438 or whatever.

   Please excuse the messy notations and commented-out code


   //Joel Tegnér
*/



/*
   PF1-PF12     = F1-F12
   F1-F4        = F1-F4
   LINE FEED    = ALT GR
   HOME         = HOME
   PF21         = NUM LOCK
   PF22         = HOME
   PF23         = END
   PF24         = PAGE UP
   PF25         = PAGE DOWN
   NO SCROLL    = GUI

   CTRL         =
   L-SHIFT      =
   R-SHIFT      =

*/

/* Lookup table for converting the scancodes to USB key codes */
/*
static const uint8_t usb_key_codes[128] = {
  KEY_TAB,         KEY_DELETE,     0,              KEY_SCROLL_LOCK,
  KEY_ENTER,       0,              0,              KEY_F1,
  0,               KEY_1,          KEY_ESC,        KEY_PAUSE,
  0,               0,              0,              0,
  0,               0,              KEY_L,          0,
  0,               0,              0,              0,
  KEY_F2,          KEY_9,          0,              0,
  0,               0,              0,              KEYPAD_ENTER,
  KEY_I,           KEY_Q,          KEY_A,          KEY_K,
  KEY_M,           0,              0,              KEY_F3,
  KEY_F10,         0,              KEY_2,          KEYPAD_MINUS,
  KEY_COMMA,       KEY_F4,         0,              KEYPAD_PERIOD,
  KEY_W,           KEY_O,          KEY_J,          KEY_S,
  KEY_Z,           KEY_N,          KEYPAD_3,       KEY_F11,
  KEY_F4,          KEY_3,          KEY_8,          KEYPAD_6,
  0,               0,              KEY_F3,         KEYPAD_2,
  KEY_RIGHT_BRACE, KEY_E,          KEY_D,          KEY_ENTER,
  0,               KEY_X,          0,              KEY_F5,
  0 ,              KEY_LEFT,       KEY_4,          KEYPAD_8,
  KEYPAD_5,        KEY_F2,         0,              0,
  KEY_R,           KEY_LEFT_BRACE, KEY_QUOTE,      KEY_F,
  KEY_C,           KEY_SLASH,      0,              0,
  KEY_F6,          KEY_5,          KEY_DOWN,       KEY_TILDE,
  KEY_EQUAL,       0,              0,              KEYPAD_0,
  KEY_P,           KEY_T,          KEY_G,          KEY_SEMICOLON,
  KEY_PERIOD,      KEY_V,          KEYPAD_1,       KEY_F7,
  KEY_F12,         KEY_UP,         KEY_6,          KEYPAD_4,
  KEYPAD_7,        0,              KEY_F1,         0,
  KEY_Y,           KEY_U,          KEY_BACKSLASH,  KEY_H,
  KEY_B,           KEY_SPACE,      0,              KEY_F9,
  KEY_F8,          KEY_7,          KEY_RIGHT,      KEY_MINUS,
  KEY_BACKSPACE,   0,              0,              0
};
*/
int x; // Buffer prevention variable
int readByte = 0; // RAW scancode from the keyboard
byte incomingByte = 0; // Refined scancode from the keyboard
byte newKEYstate; // State of the current key
byte newKEY; // Converted scancode (USB keycode)
byte keyboard_keys[6]; // Six slot key buffer

void setup() {
  Serial.begin(57600); // USB serial for debugging
  Serial1.begin(1200); // RX(pin7), TX(pin8) - Communication with keyboard at 1200 baud
}



void loop() {
  if (Serial1.available() > 0) { // Check if the UART has a new key to offer
    incomingByte = Serial1.read(); // Save the received key for later user
    newKEYstate = bitRead(incomingByte, 7); // Remember if the key was pressed or released
    bitWrite(incomingByte, 7, 0); // Clean off that pesky keystate from the scancode, because we have already saved that
    //newKEY = usb_key_codes[incomingByte];
    newKEY = incomingByte;

    if (newKEYstate == 1) { // If the key was released
      for (int c = 5; c >= 0; c--) { // Go through the key buffer array...
        if (keyboard_keys[c] == newKEY) { // ...and see if the key was pressed earlier
          keyboard_keys[c] = 0; // If it was, then clear the key slot so that the USB thing knows that the key is not pressed anymore
        }
      }
    }

    x = 0; // Reset the slot writing prevention mechanism
    if (newKEYstate == 0) { // If the key was pressed
      for (int c = 5; c >= 0; c--) { // Go through the key buffer array...
        if (keyboard_keys[c] == 0) { // ...and look for a vacant key slot
          if (x == 0) { // Prevent the key from being written to more than one slot.
            keyboard_keys[c] = newKEY; // If a vacant key slot is found, store the pressed key there
            x = 1; // Prevention check
          }
        }
      }
    }

    // DEBUG: Print the key state and key buffer for the current key press
    Serial.println();
    Serial.println();
    Serial.println(newKEYstate);
    Serial.println();
    Serial.println(keyboard_keys[0]);
    Serial.println(keyboard_keys[1]);
    Serial.println(keyboard_keys[2]);
    Serial.println(keyboard_keys[3]);
    Serial.println(keyboard_keys[4]);
    Serial.println(keyboard_keys[5]);
    Serial.println();
    Serial.println();
    Serial.println();
  }
}






