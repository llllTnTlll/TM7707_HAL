#ifndef TM7707_HPP
#define TM7707_HPP

#include "main.h"
#include "delay.h"
#include "stm32f1xx_hal_spi.h"
#include "spi.h"
#include <string>

enum Channel
{
    AIN1,
    AIN2,
    DONT_CARE = 0,
};

enum RegisterIndex : uint8_t
{
    ComminicationRegister,
    SetupRegister,
    FilterHighRegister,
    DataRegister,
    TestRegister,
    FilterLowRegister,
    ZSRegister,
    FSRegister,
};

enum CalibrationMode : uint8_t
{
    Normal,
    Self,
    ZS,
    FS,
};

enum GainMode : uint8_t
{
    GAIN_1X,
    GAIN_2X,
    GAIN_4X,
    GAIN_8X,
    GAIN_16X,
    GAIN_32X,
    GAIN_64X,
    GAIN_128X,
};

struct AdcSettings
{
    uint16_t CLOCK_IN;
    uint16_t DIGITAL_FILTER_CUTOFF;
    GainMode GAIN;
    bool IS_UNIPOLAR;
    bool IS_CLKDIS;
    bool IS_BST;
    bool IS_BUF;
};


class TM7707
{
private:
    AdcSettings Settings;

    // 方便移植
    void CS(bool status);
    void delayMS(uint16_t ms);

    void sendByte(uint8_t data);
    uint8_t readByte();
    void syncSPI();

    void initRegisters(Channel ch);
    void writeCommunicationRegister(bool isWrite, RegisterIndex next, bool isSleep, Channel ch);
    void writeSetupRegister(CalibrationMode caliMode, GainMode gainMode, bool isUnipolar, bool isBUF, bool isFSYNC, Channel ch);
    void writeFilterRegister(bool isBST, bool isCLKDIS, uint16_t cutOff);
public:
    TM7707(AdcSettings settings);
    ~TM7707();

    uint32_t getADCData(Channel ch);
};

#endif
