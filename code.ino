// Transmitter (Controller) Code
#include <SoftwareSerial.h>
#include <Wire.h> // I2C communication library

SoftwareSerial BT_Serial(2, 3); // RX, TX
const int MPU = 0x68; // I2C address of the MPU6050 accelerometer
int16_t AcX, AcY, AcZ;
int flag = 0;

void setup() {
    Serial.begin(9600); // Start serial communication at 9600bps
    BT_Serial.begin(9600);
    
    // Initialize interface to the MPU6050
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);
    
    delay(2000);
}

void loop() {
    Read_accelerometer(); // Read MPU6050 accelerometer
    
    if (AcX < 60 && flag == 0) { flag = 1; BT_Serial.write('F'); }
    if (AcX > 130 && flag == 0) { flag = 1; BT_Serial.write('B'); }
    if (AcY < 60 && flag == 0) { flag = 1; BT_Serial.write('L'); }
    if (AcY > 130 && flag == 0) { flag = 1; BT_Serial.write('R'); }
    if ((AcX > 70) && (AcX < 120) && (AcY > 70) && (AcY < 120) && (flag == 1)) {
        flag = 0;
        BT_Serial.write('N');
    }
    delay(100);
}

void Read_accelerometer() {
    // Read the accelerometer data
    Wire.beginTransmission(MPU);
    Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    
    // Read 6 registers total, each axis value is stored in 2 registers
    AcX = Wire.read() << 8 | Wire.read(); // X-axis value
    AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
    AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value
    
    AcX = map(AcX, -17000, 17000, 0, 180);
    AcY = map(AcY, -17000, 17000, 0, 180);
    AcZ = map(AcZ, -17000, 17000, 0, 180);
    
    Serial.print(AcX);
    Serial.print("\t");
    Serial.print(AcY);
    Serial.print("\t");
    Serial.println(AcZ);
}


// Receiver (RC Car) Code
#include <SoftwareSerial.h>

SoftwareSerial BT_Serial(2, 3); // RX, TX

#define enA 10 // Enable1 L298 Pin enA
#define in1 9  // Motor1 L298 Pin in1
#define in2 8  // Motor1 L298 Pin in2
#define in3 7  // Motor2 L298 Pin in3
#define in4 6  // Motor2 L298 Pin in4
#define enB 5  // Enable2 L298 Pin enB

char bt_data; // Variable to receive data from the serial port
int Speed = 150; // Motor speed (0 to 255)

void setup() {
    Serial.begin(9600); // Start serial communication at 9600bps
    BT_Serial.begin(9600);
    
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(enB, OUTPUT);
    
    delay(200);
}

void loop() {
    if (BT_Serial.available() > 0) {
        bt_data = BT_Serial.read();
        Serial.println(bt_data);
    }
    
    if (bt_data == 'f') { forward(); Speed = 180; }
    else if (bt_data == 'b') { backward(); Speed = 180; }
    else if (bt_data == 'l') { turnLeft(); Speed = 250; }
    else if (bt_data == 'r') { turnRight(); Speed = 250; }
    else if (bt_data == 's') { Stop(); }
    
    analogWrite(enA, Speed);
    analogWrite(enB, Speed);
    delay(50);
}

void forward() {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

void backward() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

void turnRight() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

void turnLeft() {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
}

void Stop() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}
