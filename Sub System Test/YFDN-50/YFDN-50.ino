#define FLOW_SENSOR_PIN 18  // GPIO pin connected to the YFDN50 sensor

volatile int pulseCount = 0;
float flowRate = 0.0;
unsigned long lastTime = 0;

// Interrupt Service Routine (ISR) for counting pulses
void IRAM_ATTR pulseCounter() {
    pulseCount++;
}

void setup() {
    Serial.begin(115200);
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, RISING);
}

void loop() {
    unsigned long currentTime = millis();
    
    // Update every 1 second
    if (currentTime - lastTime >= 1000) {
        lastTime = currentTime;
        
        // YFDN50 has 450 pulses per liter
        flowRate = (pulseCount / 450.0) * 60.0;  // Convert to liters per minute (L/min)
        pulseCount = 0;  // Reset pulse count
        
        Serial.print("Flow Rate: ");
        Serial.print(flowRate);
        Serial.println(" L/min");
    }
}
