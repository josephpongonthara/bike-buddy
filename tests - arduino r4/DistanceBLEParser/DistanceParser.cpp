#include "DistanceParser.h"

DistanceParser::DistanceParser() {
  bufferLength = 0;
}

void DistanceParser::appendData(uint8_t* data, size_t length) {
  // Add new data to the rolling buffer
  for (size_t i = 0; i < length && bufferLength < BUFFER_SIZE; i++) {
    buffer[bufferLength++] = data[i];
  }
  processBuffer();
}

void DistanceParser::processBuffer() {
  int processedUpTo = 0;

  for (int i = 0; i <= bufferLength - 4; i++) {
    if (buffer[i] == 0x54) {
      uint8_t header = buffer[i];
      uint8_t highByte = buffer[i + 1];
      uint8_t lowByte = buffer[i + 2];
      uint8_t checksum = buffer[i + 3];
      uint8_t calculatedChecksum = header ^ highByte ^ lowByte;

      if (checksum == calculatedChecksum) {
        uint16_t distance = (highByte << 8) | lowByte;
        Serial.print("Distance parsed: ");
        Serial.println(distance);
        i += 3;
        processedUpTo = i + 1;
      } else {
        Serial.println("Checksum mismatch in rolling buffer.");
      }
    }
  }

  // Shift remaining unprocessed bytes
  if (processedUpTo > 0 && processedUpTo < bufferLength) {
    shiftBuffer(processedUpTo);
  } else if (processedUpTo > 0) {
    // Everything processed
    bufferLength = 0;
  }
}

void DistanceParser::shiftBuffer(int processed) {
  for (int i = processed; i < bufferLength; i++) {
    buffer[i - processed] = buffer[i];
  }
  bufferLength -= processed;
}
