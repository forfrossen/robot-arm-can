#include <SPI.h>
#include "mcp_can.h"

#define CAN_CS 10
#define CAN_INT 2  // Set INT to pin 2
#define CAN_SPEED CAN_500KBPS
#define CLOCK_SPEED MCP_8MHZ
MCP_CAN CAN(CAN_CS);

class Servo42D_CAN {
private:
  uint8_t can_id;


public:
  Servo42D_CAN(uint8_t id)
    : can_id(id) {}

  // Method to initialize the MCP2515 CAN controller
  static void begin() {
    if (CAN.begin(MCP_ANY, CAN_SPEED, CLOCK_SPEED) == CAN_OK) {
      CAN.setMode(MCP_NORMAL);  // Change to normal mode to allow messages to be transmitted
      pinMode(CAN_INT, INPUT);           
      Serial.println("MCP2515 Initialized Successfully!");
    } else {
      Serial.println("Error Initializing MCP2515...");
    }
  }
  
  // Method to send CAN message
  bool sendCANMessage(uint8_t *data, uint8_t length) {
    if((CAN.sendMsgBuf(can_id, 0, length, data) == CAN_OK)){
      //delay(50);
      //readCanResponse(data[0]);
      return true;
    }
    return false;
  }

  // Method to read CAN message
  bool readCANMessage(uint8_t *data, uint8_t &length, uint32_t &id) {
    if (CAN.checkReceive() == CAN_MSGAVAIL) {
      CAN.readMsgBuf(&id, &length, data);
      return true;
    }
    return false;
  }

  void readCanResponse(uint8_t data){
    unsigned long rxId;
    byte len;
    byte rxBuf[8];
    char msgString[128];
    
    if(CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK) // Read data: len = data length, buf = data byte(s)
    {
      if(rxBuf[0] == data){
        if(rxBuf[1] == 1){
          Serial.println("Motor reports SUCCES!");
        } else {
          Serial.println("!!Motor reports FAILURE!!");
        }
      } else{
        sprintf_P(msgString, PSTR("NO MATCH! 0x%.2X 0x%.2X"), rxBuf[0], data);
        Serial.print(msgString);
      }
    }
  }
  
  byte calculateCRC(uint8_t data[], uint8_t length) {
    byte crc = can_id;  // Start with the CAN ID
    for (int i = 0; i < length; ++i) {
      crc += data[i];
    }
    return crc & 0xFF;
  }

  // Set Speed and Acceleration together
  void setSpeedAndAcceleration(uint16_t speed, uint8_t acceleration, bool direction = true) {
    uint8_t data[5];
    data[0] = 0xF6;                     // Command code for setting speed and acceleration
    data[1] = direction ? 0x00 : 0x80;  // Direction bit (0 for CW, 1 for CCW in the most significant bit)
    data[1] |= (speed >> 8) & 0x7F;     // Combine direction bit with the upper 7 bits of speed
    data[2] = speed & 0xFF;             // Lower 8 bits of speed
    data[3] = acceleration;             // Acceleration
    data[4] = calculateCRC(data, 4);    // CRC including CAN ID (0x01)
    
    
    Serial.println("Sending setSpeedAndAcceleration command!");
    if (sendCANMessage(data, 5)) {
      
//      readCanMessages();

      uint8_t recvData[6];
      uint8_t recvLen;
      uint32_t recvId;

      if (readCANMessage(recvData, recvLen, recvId)) {
        // Empfange Daten verarbeiten
        uint8_t status = recvData[1];
        if(status == 1){
          Serial.println("setSpeedAndAcceleration: Sent Successfully sent and processed!");          
        } else {
          Serial.println("setSpeedAndAcceleration: Sent Successfully but FAILED to be processed!");
        }
      } else {
        Serial.println("Could not read response to setSpeedAndAcceleration!");
      } 
    
    } else {
      Serial.println("setSpeedAndAcceleration: could not be sent!");        
    }
  }

  void stopMotor() {
    uint8_t data[4];
    data[0] = 0xF7;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = calculateCRC(data, 3);

    if (sendCANMessage(data, 4)) {
      Serial.println("Stop Motor Command Sent Successfully!");
    } else {
      Serial.println("Error Sending Stop Motor Command...");
    }
  }

  void enableMotor() {
    //F3 = Enable Command
    //01 = Enable / O0 = Disable
    uint8_t data[3] = { 0xF3, 0x01, 0x00 };  // Befehlscode zum Abfragen der Position
    data[2] = calculateCRC(data, 2);

    if (sendCANMessage(data, 3)) {
      Serial.println("Enable Command Sent Successfully!");
    } else {
      Serial.println("Error Sending Enable Command...");
    }

    // 85 = set enable pin
    // enable = 00 active low (L)
    // enable = 01 active high (H)
    // enable = 02 active always (Hold)

    uint8_t data2[3] = { 0x85, 0x02, 0x00 };  // Befehlscode zum Abfragen der Position
    data[2] = calculateCRC(data, 2);

    if (sendCANMessage(data, 3)) {
      Serial.println("Enable Pin Command Sent Successfully!");
    } else {
      Serial.println("Error Sending Enable Pin Command...");
    }

  }

  int queryMotorStatus() {
    uint8_t data[2] = { 0xF1, 0x00 };
    data[1] = calculateCRC(data, 1);

    if (sendCANMessage(data, 2)) {
      Serial.println("QueryStatus Command Sent Successfully!");
    } else {
      Serial.println("Error Sending QueryStatus Command...");
    }

    delay(10);
    uint8_t recvData[8];
    uint8_t recvLen;
    uint32_t recvId;
    if (readCANMessage(recvData, recvLen, recvId)) {
      // Empfange Daten verarbeiten
      uint8_t status = recvData[1];
      switch (status) {
        case 0:
          Serial.println("Abfrage fehlgeschlagen");
          break;
        case 1:
          Serial.println("Motor gestoppt");
          break;
        case 2:
          Serial.println("Motor beschleunigt");
          break;
        case 3:
          Serial.println("Motor verlangsamt");
          break;
        case 4:
          Serial.println("Motor volle Geschwindigkeit");
          break;
        case 5:
          Serial.println("Motor wird geparkt");
          break;
        case 6:
          Serial.println("Motor wird kalibriert");
          break;
        default:
          Serial.println("Unbekannter Status");
          break;
      }
      return status;
    }
    return -1;  // Fehler beim Lesen der Nachricht
  }

  
  int queryMotorPosition() {
    uint8_t data[2] = {0x30, 0x00}; // Command code for querying position (0x30)
    sendCANMessage(data, 2);
    delay(10);
    uint8_t recvData[8];
    uint8_t recvLen;
    uint32_t recvId;
    if (readCANMessage(recvData, recvLen, recvId)) {
        uint32_t position = (recvData[3] << 16) | (recvData[4] << 8) | recvData[5];
        Serial.print("Motor position: ");
        Serial.println(position);
        return position;
    }
    Serial.println("Error reading position");
    return -1; // Error reading message
  }

   void setTargetPosition(uint32_t position, uint8_t speed = 100, uint8_t acceleration = 5, bool absolute = true) {
    uint8_t data[8];
    data[0] = absolute ? 0xF5 : 0xF4;                  // Befehlscode für Position mode4: absolute motion by axis
    data[1] = (speed >> 8) & 0x7F;     // Combine direction bit with the upper 7 bits of speed
    data[2] = speed & 0xFF;             // Lower 8 bits of speed
    data[3] = acceleration; // Beschleunigung
    data[4] = (position >> 16) & 0xFF; // Obere 8 Bits der Position
    data[5] = (position >> 8) & 0xFF; // Mittlere 8 Bits der Position
    data[6] = position & 0xFF; // Untere 8 Bits der Position
    data[7] = calculateCRC(data, 7);    // Checksumme oder reserved (kann auf 0 gesetzt werden)

    if (sendCANMessage(data, 8)) {
      Serial.print("Zielposition gesetzt: ");
      Serial.print(position);
      Serial.print(", Geschwindigkeit: ");
      Serial.print(speed);
      Serial.print(", Beschleunigung: ");
      Serial.print(acceleration);
      Serial.print(", Modus: ");
      Serial.println(absolute ? "Absolut" : "Relativ");

      delay(10);
      
      if (sendCANMessage(data, 5)) {
        
  //      readCanMessages();

        uint8_t recvData[6];
        uint8_t recvLen;
        uint32_t recvId;

        if (readCANMessage(recvData, recvLen, recvId)) {
          // Empfange Daten verarbeiten
          uint8_t status = recvData[1];
          if(status == 1){
            Serial.println("setTargetPosition Command: Sent Successfully sent and processed!");          
          } else {
            Serial.println("setTargetPosition Command: Sent Successfully but FAILED to be processed!");
          }
        } else {
          Serial.println("Could not read response for setTargetPosition!");
        } 
      
      } else {
        Serial.println("setTargetPosition Command: could not be sent!");        
      }
    }
  }

  
  
  void readCanMessages(){
    if(!digitalRead(CAN_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
      uint32_t long rxId;
      byte len;
      byte rxBuf[8];

      if(CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK) // Read data: len = data length, buf = data byte(s)
      {
        char msgString[128]; // Array to store serial string
        if(rxId & CAN_IS_EXTENDED) // Determine if ID is standard (11 bits) or extended (29 bits)
          sprintf_P(msgString, PSTR("Extended ID: 0x%.8lX  DLC: %1d  Data:"), (rxId & CAN_EXTENDED_ID), len);
        else
          sprintf_P(msgString, PSTR("Standard ID: 0x%.3lX       DLC: %1d  Data:"), rxId, len);

        Serial.print(msgString);

      
        for(byte i=0;i<len;i++)
        {
          sprintf_P(msgString, PSTR(" 0x%.2X"), rxBuf[i]);
          Serial.print(msgString);
        }
      
        
        Serial.println();
      }
      else {
        Serial.println(F("Interrupt is asserted, but there were no messages to process..."));
      }
    }
  }  
};
