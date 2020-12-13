/*
SAMD USB to serial

Author  : Quentin Bolsee
License : CC BY-NC-SA 4.0 https://creativecommons.org/licenses/by-nc-sa/4.0/
Date    : 2020-07-27
Version :
    - 0.0 : first draft
*/

//#define DEBUG
#define BAUD_DEFAULT 9600
#define PIN_TX 4


const byte config_lookup[3][2][4] = {
  {{SERIAL_5N1, SERIAL_6N1, SERIAL_7N1, SERIAL_8N1},
   {SERIAL_5N2, SERIAL_6N2, SERIAL_7N2, SERIAL_8N2}},
  {{SERIAL_5O1, SERIAL_6O1, SERIAL_7O1, SERIAL_8O1},
   {SERIAL_5O2, SERIAL_6O2, SERIAL_7O2, SERIAL_8O2}},
  {{SERIAL_5E1, SERIAL_6E1, SERIAL_7E1, SERIAL_8E1},
   {SERIAL_5E2, SERIAL_6E2, SERIAL_7E2, SERIAL_8E2}},
};


typedef struct {
  long unsigned  baud; 
  uint8_t   paritytype; // parity: 0=none, 1=odd, 2=even, 3=mark, 4=space
  uint8_t   stopbits; // stopbits: 0=1, 1=1.5, 2=2
  uint8_t   numbits; // databits: 5,6,7,8,16
} SerialConfig;


SerialConfig current_config = {BAUD_DEFAULT, 0, 1, 8};


void update_serial(SerialConfig new_config) {
  if (new_config.paritytype > 2 || !(new_config.stopbits == 0 || new_config.stopbits == 2) || new_config.numbits < 5 || new_config.numbits > 8) {
    return;
  }

  // lookup indexes
  int i = new_config.paritytype;
  int j = new_config.stopbits == 2 ? 1 : 0;
  int k = (int)new_config.numbits - 5;

  Serial2.flush();
  Serial2.end();
  Serial2.begin(new_config.baud, config_lookup[i][j][k]);
  current_config.baud = new_config.baud;
  current_config.paritytype = new_config.paritytype;
  current_config.stopbits = new_config.stopbits;
  current_config.numbits = new_config.numbits;
}


void setup() {
  // prevents small voltage drops when Serial is closed then opened
  pinMode(PIN_TX, OUTPUT);
  digitalWrite(PIN_TX, HIGH);
  SerialUSB.begin(BAUD_DEFAULT);
  Serial2.begin(BAUD_DEFAULT);
}


void loop() {
  // Detect config change
  SerialConfig new_config = {SerialUSB.baud(), SerialUSB.paritytype(), SerialUSB.stopbits(), SerialUSB.numbits()};
  if (new_config.baud != current_config.baud || new_config.paritytype != current_config.paritytype ||
      new_config.stopbits != current_config.stopbits || new_config.numbits != current_config.numbits) {
    update_serial(new_config);
  }

  #ifdef DEBUG
    SerialUSB.println("Settings: ");
    SerialUSB.print(current_config.baud);
    SerialUSB.print(", ");
    SerialUSB.print(current_config.paritytype);
    SerialUSB.print(", ");
    SerialUSB.print(current_config.stopbits);
    SerialUSB.print(", ");
    SerialUSB.println(current_config.numbits);
    delay(1000);
  #else
    char c;
    if (SerialUSB.available()) {
      c = (char)SerialUSB.read();
      Serial2.write(c);
    }
    if (Serial2.available()) {
      c = (char)Serial2.read();
      SerialUSB.write(c);
    }
  #endif
}
