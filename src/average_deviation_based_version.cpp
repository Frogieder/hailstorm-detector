/* Commented out, as we don't want PlatformIO to actually build it

// Debug version intended for pi pico with pico debug probe
// May contain bugs

#include "Arduino.h"
#include "HX711.h"

#define POWER 5
#define BUFFER_SIZE (1 << POWER)
#define ACCEPTED_DIFF_POWER 3

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

long sum;
FixedQueue<int32_t, BUFFER_SIZE> buffer;

void setup() {
//    scale.begin(0, 1);
    scale.begin(2, 3);
//    scale.set_gain(64);
    Serial1.begin(115200);
    pinMode(17, OUTPUT);

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
        diffs_sum += abs(buffer[i] - mean);
    }
    long accepted_diff = diffs_sum >> (POWER - ACCEPTED_DIFF_POWER);

    // read input
    long a = read_scale_blocking();
    Serial1.printf("a: %li, mean: %li, accepted_diff: %li\n", a, mean, accepted_diff);

    bool is_peak;
    if (a > mean) {
        is_peak = (mean + accepted_diff) < a;
    }
    else {
        is_peak = (mean - accepted_diff) > a;
    }
    digitalWrite(17, is_peak);
    sum -= buffer.pop();
    sum += a;
    buffer.push(a);
}
*/
