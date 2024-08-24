#include "tm7707.hpp"

TM7707::TM7707(AdcSettings settings) : Settings(settings)
{
    initRegisters(AIN1);
}

void TM7707::CS(bool status)
{
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, (GPIO_PinState)status);
}

void TM7707::delayMS(uint16_t ms)
{
    delayMSecs(ms);
}

void TM7707::sendByte(uint8_t data)
{
    CS(0);
    HAL_SPI_Transmit(&hspi1, &data, sizeof(data), HAL_MAX_DELAY);
    CS(1);
}

void TM7707::writeCommunicationRegister(bool isWrite, RegisterIndex next, bool isSleep, Channel ch)
{
    uint8_t cmd = 0;

    // 置DRDY
    if (!isWrite)
        cmd |= (1 << 7);
    // 置RS2~RS0
    cmd |= (next << 6);
    // 置R/W
    if (!isWrite)
        cmd |= (1 << 3);
    // 置STBY
    if (isSleep)
        cmd |= (1 << 2);
    // 置CH1~CH0
    cmd |= ch;

    sendByte(cmd);
}

void TM7707::writeSetupRegister(CalibrationMode caliMode, GainMode gainMode, bool isUnipolar, bool isBUF, bool isFSYNC, Channel ch)
{
    uint8_t cmd = 0;
    cmd |= (caliMode << 6);
    cmd |= (gainMode << 3);
    if (isUnipolar)
    {
        cmd |= (1 << 2);
    }
    if (isBUF)
    {
        cmd |= (1 << 1);
    }
    if (isFSYNC)
    {
        cmd |= 1;
    }

    writeCommunicationRegister(1, SetupRegister, false, ch);
    sendByte(cmd);
}

void TM7707::writeFilterRegister(bool isBST, bool isCLKDIS, uint16_t cutOff)
{
    uint8_t highCMD = 0;
    uint8_t lowCMD = 0;

    if (isBST)
    {
        highCMD |= (1 << 5);
    }
    if (isCLKDIS)
    {
        highCMD |= (1 << 4);
    }

    // 计算code
    uint16_t code = 0;
    // cutOff的范围必须在4.8Hz-1.01KHz
    if (cutOff > 5 && cutOff < 1010)
    {
        code = Settings.CLOCK_IN / 128 / cutOff;
        uint8_t highByte = (code >> 8) & 0xFF;
        uint8_t lowByte = code & 0xFF;

        highCMD |= highByte;
        lowCMD |= lowByte;

        writeCommunicationRegister(1, FilterHighRegister, 0, DONT_CARE);
        sendByte(highCMD);
        writeCommunicationRegister(1, FilterLowRegister, 0, DONT_CARE);
        sendByte(lowCMD);
    }
}

uint8_t TM7707::readByte()
{
    uint8_t buf;
    HAL_StatusTypeDef Status = HAL_SPI_Receive(&hspi1, &buf, 1, 3000);

    // 当通信不同步的时候进行一次同步
    if (Status != HAL_OK)
    {
        syncSPI();
    }
    return buf;
}

void TM7707::syncSPI()
{
    // 同步总线，返回上一个状态
    for (int i = 0; i < 3; i++)
    {
        sendByte(0b11111111);
    }

    // 重新进行寄存器初始化
    initRegisters(AIN1);
    initRegisters(AIN2);
}

void TM7707::initRegisters(Channel ch)
{
    // 等待晶振稳定
    // 根据手册2.4576MHz情况下稳定需要16ms
    delayMS(20);

    writeFilterRegister(Settings.IS_BST, Settings.IS_CLKDIS, Settings.DIGITAL_FILTER_CUTOFF);  // 无电流增强 无时钟输出 数字滤波截止频率500Hz
    writeSetupRegister(Self, Settings.GAIN, Settings.IS_UNIPOLAR, Settings.IS_BUF, false, ch); // 二倍增益 双极性 无缓冲 自校准

    getADCData(ch);
}

uint32_t TM7707::getADCData(Channel ch)
{
    // 读取数据
    uint32_t data = 0;

    writeCommunicationRegister(false, DataRegister, false, ch);
    while (HAL_GPIO_ReadPin(DRDY_GPIO_Port, DRDY_Pin) == GPIO_PIN_SET);
    data |= readByte();
    data <<= 8;
    data |= readByte();
    data <<= 8;
    data |= readByte();

    return data;
}
