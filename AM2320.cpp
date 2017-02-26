#include "AM2320.h"
#include <Wire.h>
// 
// AM2321 Temperature & Humidity Sensor library for Arduino
//
// The MIT License (MIT)
//
// Copyright (c) 2015 THAKSHAK GUDIMETLA
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

unsigned int CRC16(byte *ptr, byte length) 
{ 
      unsigned int crc = 0xFFFF; 
      uint8_t s = 0x00; 

      while(length--) {
        crc ^= *ptr++; 
        for(s = 0; s < 8; s++) {
          if((crc & 0x01) != 0) {
            crc >>= 1; 
            crc ^= 0xA001; 
          } else crc >>= 1; 
        } 
      } 
      return crc; 
} 

AM2320::AM2320()
{
}

int AM2320::Read()
{
	byte buf[8];
	for(int s = 0; s < 8; s++) buf[s] = 0x00; 

	Wire.beginTransmission(AM2320_address);
	Wire.endTransmission();
	// запрос 4 байт (температуры и влажности)
	Wire.beginTransmission(AM2320_address);
	Wire.write(0x03);// запрос
	Wire.write(0x00); // с 0-го адреса
	Wire.write(0x04); // 4 байта
	if (Wire.endTransmission() != 0) return 1;
	delayMicroseconds(1600); //>1.5ms
	// считываем результаты запроса
	Wire.requestFrom(AM2320_address, 0x08); 
	for (int i = 0; i < 0x08; i++) buf[i] = Wire.read();
	if (Wire.endTransmission() != 0) return 1;

	// CRC check
	unsigned int Rcrc = buf[7] << 8;
	Rcrc += buf[6];
	if (Rcrc == CRC16(buf, 6)) {
		unsigned int temperature = ((buf[4] & 0x7F) << 8) + buf[5];
		t = temperature / 10.0;
		t = ((buf[4] & 0x80) >> 7) == 1 ? t * (-1) : t;

		unsigned int humidity = (buf[2] << 8) + buf[3];
		h = humidity / 10.0;
		return 0;
	}
        return 2;
}
