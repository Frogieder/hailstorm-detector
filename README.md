# Hailstorm detector

This device connects ATtiny402 (and possibly other simmilar chips) to HX711 and a strain gauge to detect force spikes on the sensor, that is, when a hailstone hits the sensor.
The intended design has a net mounted to a strain gauge so that water droplets fall through, while hailstones trigger the sensor.

The default connections are:
* HX711 SCK -> PA6 / Arduino pin 0 / Physical pin 2
* HX711 DT  -> PA7 / Arduino pin 1 / Physical pin 3
* The output is on PA3 / Arduino pin 4 / Physical pin 7
The pins may be switched as needed

HX711 is MODIFIED from factory default, the 0R resistor has to be moved, so that the chip operates at 80 SPS (although my experiments showed it running at around 89 SPS instead).

## Usage
Connect the output pin to your host microcontroller. When nothing is detected, the output is low.
On hailstone impact, the chip connects internal pullup to the output pin.
This ensures that host microcontroller can read HIGH input on this pin and can pull it to LOW safely, which resets the sensor.

Example implementation:
```c++
if (digitalRead(HAILSTORM_SENSOR_PIN) == HIGH) {
    
    // Take whatever action is necessary
    
    pinMode(HAILSTORM_SENSOR_PIN);
    digitalWrite(HAILSTORM_SENSOR_PIN, LOW);
    delay(10);
    digitalWrite(HAILSTORM_SENSOR_PIN, HIGH);
    pinMode(HAILSTORM_SENSOR_PIN, INPUT);
}
```
This snippet should be run periodically (at least ~80 times per second) to ensure that the hailstones are detected immediately.