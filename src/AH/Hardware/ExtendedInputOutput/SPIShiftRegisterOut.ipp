#ifdef ARDUINO // TODO: I'm too lazy to mock the SPI library

#include "ExtendedInputOutput.hpp"
#include "SPIShiftRegisterOut.hpp"

AH_DIAGNOSTIC_EXTERNAL_HEADER()
#include <SPI.h>
AH_DIAGNOSTIC_POP()

BEGIN_AH_NAMESPACE

template <uint8_t N, class SPIDriver>
SPIShiftRegisterOut<N, SPIDriver>::SPIShiftRegisterOut(SPIDriver spi,
                                                       pin_t latchPin,
                                                       BitOrder_t bitOrder)
    : ShiftRegisterOutBase<N>(latchPin, bitOrder),
      spi(std::forward<SPIDriver>(spi)) {}

template <uint8_t N, class SPIDriver>
void SPIShiftRegisterOut<N, SPIDriver>::begin() {
    ExtIO::pinMode(this->latchPin, OUTPUT);
    spi.begin();
    updateBufferedOutputs();
}

template <uint8_t N, class SPIDriver>
void SPIShiftRegisterOut<N, SPIDriver>::updateBufferedOutputs() {
    if (!this->dirty)
        return;
    spi.beginTransaction(settings);
    ExtIO::digitalWrite(this->latchPin, LOW);
    const uint8_t bufferLength = this->buffer.getBufferLength();
    if (this->bitOrder == LSBFIRST)
        for (uint8_t i = 0; i < bufferLength; i++)
            spi.transfer(this->buffer.getByte(i));
    else
        for (int8_t i = bufferLength - 1; i >= 0; i--)
            spi.transfer(this->buffer.getByte(i));
    ExtIO::digitalWrite(this->latchPin, HIGH);
    spi.endTransaction();
    this->dirty = false;
}

END_AH_NAMESPACE

#endif