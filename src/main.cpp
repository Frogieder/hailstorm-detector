
// Current implementation for ATtiny402.
// Scale is connected as follows:
//      DT  -> PA6 / Arduino pin 0 / Physical pin 2
//      SCK -> PA7 / Arduino pin 1 / Physical pin 3
// The output is on PA3 / Arduino pin 3 / Physical pin 7

#include <Arduino.h>
//#include "Wire.h"
#include "HX711.h"

#define DIFF_THRESHOLD 350000
#define OUTPUT_PIN 4
#define DT_PIN 1
#define CLK_PIN 0
#define DEBUG_DT 2
#define DEBUG_CLK 3
//#define OUTPUT_PIN 17
//#define DT_PIN 2
//#define CLK_PIN 3

#define BUFFER_SIZE 45
#define ACCEPTED_DIFF_POWER 3

// Deque implementation
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

HX711 scale;

int32_t read_scale_blocking() {
    while (!scale.is_ready()) ;
    return scale.read() + 0x7FFFFF;
}

int32_t absolute(int32_t val) {
    return val < 0 ? -val : val;
}

FixedQueue<int32_t, BUFFER_SIZE> buffer;
int32_t sum;
bool outputting;


void get_new_data() {
    int32_t a;
    sum = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        a = read_scale_blocking();
        sum += a;
        buffer.push(a);
    }
}

void setup() {
    scale.begin(DT_PIN, CLK_PIN, 64);
//    scale.begin(2, 3);
//    scale.set_gain(64);
//    Serial1.begin(115200);
    pinMode(OUTPUT_PIN, OUTPUT);
    digitalWrite(OUTPUT_PIN, LOW);

    // debug output interface
    pinMode(DEBUG_DT, OUTPUT);
    pinMode(DEBUG_CLK, OUTPUT);

    // Fill buffer with first data
    get_new_data();
}

bool skip = false;

void loop() {

    int32_t mean = sum / BUFFER_SIZE;

    // read input
    int32_t a = read_scale_blocking();
//    Serial1.printf("a: %li, mean: %li, diff: %li\n", a, mean);

    int32_t val = mean;
    for (uint8_t i = 0; i < 32; i++)  {
        digitalWrite(DEBUG_DT, val & 0x01), val >>= 1;

        digitalWrite(DEBUG_CLK, HIGH);
        digitalWrite(DEBUG_CLK, LOW);
    }

    delay(1);
    val = absolute(a-mean);
    for (uint8_t i = 0; i < 32; i++)  {
        digitalWrite(DEBUG_DT, val & 0x01), val >>= 1;

        digitalWrite(DEBUG_CLK, HIGH);
        digitalWrite(DEBUG_CLK, LOW);
    }

//    if (no_detection == 0 && absolute(mean - a) > DIFF_THRESHOLD) {
    if (absolute(mean - a) > DIFF_THRESHOLD) {
        pinMode(OUTPUT_PIN, INPUT_PULLUP);
        delay(1000);
        while (digitalReadFast(OUTPUT_PIN));
        pinMode(OUTPUT_PIN, OUTPUT);
        digitalWrite(OUTPUT_PIN, LOW);
        get_new_data();
    }

    if (!skip) {
        sum -= buffer.pop();
        sum += a;
        buffer.push(a);
    }
    skip = !skip;
}