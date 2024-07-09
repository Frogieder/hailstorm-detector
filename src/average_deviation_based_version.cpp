/* Commented out, as we don't want PlatformIO to actually build it

// Debug version intended for pi pico with pico debug probe
// May contain bugs

#include "Arduino.h"
#include "HX711.h"

#define OUTPUT_PIN 4

#define POWER 5
#define BUFFER_SIZE (1 << POWER)
#define ACCEPTED_DIFF_MULTIPLIER 20

HX711 scale;

int32_t read_scale_blocking() {
    while (!scale.is_ready()) ;
    return scale.read() + 8388607;
}

template <typename T, size_t Size>
class FixedQueue {
public:
    FixedQueue() : front(0), back(0), count(0) {}

    void push(const T& element) {
        if (count == Size) {
            return;
        }
        data[back] = element;
        back = (back + 1) % Size;
        count++;
    }

    T pop() {
        T a = data[front];
        front = (front + 1) % Size;
        count--;
        return a;
    }

    T& operator[](size_t index) {
        return data[(front + index) % Size];
    }

    const T& operator[](size_t index) const {
        return data[(front + index) % Size];
    }

private:
    T data[Size];
    size_t front;
    size_t back;
    size_t count;
};

void print(int32_t val) {
    for (uint8_t i = 0; i < 32; i++)  {
        digitalWrite(2, val & 0x01), val >>= 1;

        digitalWrite(3, HIGH);
        digitalWrite(3, LOW);
    }
}

int32_t absolute(int32_t val) {
    return val < 0 ? -val : val;
}

long sum;
FixedQueue<int32_t, BUFFER_SIZE> buffer;

void setup() {
    scale.begin(1, 0, 64);
//    scale.begin(2, 3);
//    scale.set_gain(64);
    pinMode(OUTPUT_PIN, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    // Fill buffer with first data
    long a;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        a = read_scale_blocking();
        sum += a;
        buffer.push(a);
    }
}

void loop() {
//    auto reading = read_scale_blocking();
//    digitalWrite(4, reading > 200000);

    long mean = sum >> POWER;
    long diffs_sum = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        diffs_sum += absolute(buffer[i] - mean);
    }
    long accepted_diff = (ACCEPTED_DIFF_MULTIPLIER*diffs_sum) >> (POWER);

    // read input
    long a = read_scale_blocking();

    print(mean);
    delayMicroseconds(400);
    print(accepted_diff);
    delayMicroseconds(400);
    print(absolute(a-mean));

    if (absolute(a-mean) > accepted_diff) {
        pinMode(OUTPUT_PIN, INPUT_PULLUP);
        delay(1000);
        while (digitalRead(OUTPUT_PIN));
        pinMode(OUTPUT_PIN, OUTPUT);
        digitalWrite(OUTPUT_PIN, LOW);
    }
    sum -= buffer.pop();
    sum += a;
    buffer.push(a);
}
*/
