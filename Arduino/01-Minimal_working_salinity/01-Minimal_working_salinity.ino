int potPin = 0;
float ppt = 0;
float refvoltage = 1.1;
unsigned int ADCValue;
double Vcc;

void setup()
{
  Serial.begin(9600);
  Serial.println("Sal sensor ");
  analogReference(INTERNAL);
}

void loop()
{
  Vcc = readVcc()/1000.0;
  int samples = 20;
  int aRead = 0;  

  for (int i = 0; i < samples ; i++) 
  {
    aRead += analogRead(potPin);
  }
  int aSamples = aRead/ samples;
  float voltage = 5.0 * aSamples/ 1023;  // assuming 5V reference
    // question: would it be better to use the following?:   voltage = Vcc * aSamples / 1024
    
  ppt = 16.3 * voltage;

  // print the data
  Serial.print("Voltage in: ");
  Serial.print(Vcc,3);
  Serial.print(" Analog in reading: ");
  Serial.print(aSamples);
  Serial.print(" - Calculated ppt: ");
  Serial.println(ppt, 2);
  delay(1000);
}


//Calculate the ADC voltage
long readVcc() { 
  long result; 
  // Read 1.1V reference against AVcc 
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
  delay(2); // Wait for Vref to settle 

  ADCSRA |= _BV(ADSC); // Convert 
  while (bit_is_set(ADCSRA,ADSC)); 
  result = ADCL; 
  result |= ADCH<<8; 
  result = 1125300L / result; // Back-calculate AVcc in mV 
  return result; 
}
