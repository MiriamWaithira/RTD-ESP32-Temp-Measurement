#include <SPI.h> 

// Pins  
#define ADC_CS         5  
#define EXCITATION_CTRL 17 // Controls REF200 current  

// Constants  
#define EXCITATION_HIGH 0.001 // 1mA  
#define EXCITATION_LOW  0.0001 // 0.1mA  
#define R0_PT100      100.0  
#define R0_PT1000    1000.0  

void setup() {  
  Serial.begin(115200);  
  pinMode(EXCITATION_CTRL, OUTPUT);  
  SPI.begin();  
}  

void setExcitationCurrent(float current) {  
  digitalWrite(EXCITATION_CTRL, (current > 0.0005) ? HIGH : LOW);  
  delay(10); // Stabilize  
}  

float readResistance() {  
  int32_t raw = readADC();  
  return (raw / 8388607.0) * 2.5 / getExcitationCurrent();  
}  

String detectRTDType(float &scaleFactor) {  
  setExcitationCurrent(EXCITATION_HIGH);  
  float R = readResistance();  

  if (R > 80 && R < 120) {  
    scaleFactor = 1.0;  
    return "PT100";  
  }  

  setExcitationCurrent(EXCITATION_LOW);  
  R = readResistance();  

  if (R > 800 && R < 1200) {  
    scaleFactor = 10.0;  
    return "PT1000";  
  }  

  return "UNKNOWN";  
}  

void loop() {  
  float scale;  
  String type = detectRTDType(scale);  

  float R = readResistance() * scale; // Apply scaling  
  float temp = calculateTemp(R, type);  

  Serial.printf("%s: %.2f°C (R=%.1fΩ)\n", type.c_str(), temp, R);  
  delay(1000);  
}  