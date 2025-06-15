#ifndef DISTANCEPARSER_H
#define DISTANCEPARSER_H

#include <Arduino.h>

class DistanceParser {
  public:
    DistanceParser();
    void appendData(uint8_t* data, size_t length);
    void processBuffer();
  private:
    static const int BUFFER_SIZE = 64;
    uint8_t buffer[BUFFER_SIZE];
    int bufferLength;
    void shiftBuffer(int processed);
};

#endif
