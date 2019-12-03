/*
 *  @file Casket.h
 *
 *  https://github.com/osafune/osafune.github.io/blob/master/casket_quickdocs.md
 */
#ifndef _GOMIHGY_CASKET_H_
#define _GOMIHGY_CASKET_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <SPI.h>

/* LEDペリフェラル パラメータ */
#define CASKET_MEM_SIZE_CHANNEL   12  // チャネル数
#define CASKET_MEM_SIZE_LEDCOUNT  256 // チャネル当たりのLED数

/* SPI */
#define CASKET_SPI_MAX_SPEED      20000000  // SCLKの最大周波数 
#define CASKET_SPI_SINGLE_READ    0xC0      // シングルリード時の送信データの上位2bit
#define CASKET_SPI_SINGLE_WRITE   0x80      // シングルライト時の送信データの上位2bit
#define CASKET_SPI_MULTI_READ     0x40      // マルチリード時の送信データの上位2bit
#define CASKET_SPI_MULTI_WRITE    0x00      // マルチライト時の送信データの上位2bit

/* レジスタアドレス */
#define CASKET_REG_ADDR_SYSTEMID          0x000000 // システムIDレジスタ
#define CASKET_REG_ADDR_TIMECODE          0x000004 // 生成タイムコードレジスタ
#define CASKET_REG_ADDR_UID_LOW           0x000008 // チップユニークIDレジスタ(下位32bit)
#define CASKET_REG_ADDR_UID_HIGH          0x00000C // チップユニークIDレジスタ(上位32bit)
#define CASKET_REG_ADDR_STATUS            0x000100 // ステータスレジスタ
#define CASKET_REG_ADDR_CTL               0x000104 // コントロールレジスタ
#define CASKET_REG_ADDR_BASE_SCROLL       0x000108 // ベーススクロールレジスタ
#define CASKET_REG_ADDR_BASE_BRIGHTNESS   0x00010C // ベース輝度ゲイン設定レジスタ
#define CASKET_REG_ADDR_OVERLAY_SCROOL    0x000110 // オーバーレイスクロールレジスタ
#define CASKET_REG_ADDR_OVERLAY_GAIN      0x000114 // オーバーレイ輝度ゲイン設定レジスタ
#define CASKET_REG_ADDR_EFFECT_SEED       0x000118 // エフェクトシードレジスタ
#define CASKET_REG_ADDR_EFFECT_GAIN       0x00011C // エフェクト輝度ゲイン設定レジスタ
#define CASKET_REG_ADDR_MEM_CH0           0x008000 // ベースレイヤーのピクセルデータレジスタ
#define CASKET_REG_ADDR_OVMEM_CH0         0x00C000 // オーバーレイレイヤーのピクセルデータレジスタ

class Casket {
  public:
    void begin(int pin_ss, uint8_t spi_bus = HSPI, int clock_speed = CASKET_SPI_MAX_SPEED);
    void begin(int pin_sck, int8_t pin_miso, int pin_mosi, int pin_ss, uint8_t spi_bus = HSPI, int clock_speed = CASKET_SPI_MAX_SPEED);
    void end();
    uint32_t readSystemID();
    uint32_t readTimeCode();
    uint32_t readUIDLow();
    uint32_t readUIDHigh();
    uint32_t readStatusReg();
    bool isReady();
    void writeStatusReg(uint32_t reg);
    void startContinuousMode();
    void startOneshotMode();
    uint32_t readControlReg();
    void writeControlReg(uint32_t reg);
    void resetLedPeripheral();
    void writeScanNum(uint16_t size);
    uint32_t readBaseScroolReg();
    void writeBaseScroolReg(uint32_t reg);
    uint32_t readBaseBrightnessReg();
    void writeBaseBrightnessReg(uint8_t r, uint8_t g, uint8_t b);
    uint32_t readOverlayScroolReg();
    void writeOverlayScroolReg(uint32_t reg);
    uint32_t readOverlayBrightnessReg();
    bool writeOverlayBrightnessReg(uint8_t mode, uint8_t r, uint8_t g, uint8_t b);
    uint32_t readFreerunSeedReg();
    void writeFreerunSeedReg(uint32_t seed);
    uint32_t readEffectBrightnessReg();
    bool writeEffectBrightnessReg(bool freerun, uint8_t mode, uint8_t r, uint8_t g, uint8_t b);
    uint32_t readBasePixel(uint8_t ch, uint8_t index);
    bool writeBasePixel(uint8_t ch, uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    bool writeBasePixels(uint8_t ch, uint8_t pixel[][3], uint16_t size);
    uint32_t readOverlayPixel(uint8_t ch, uint8_t index);
    bool writeOverlayPixel(uint8_t ch, uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    bool writeOverlayPixels(uint8_t ch, uint8_t pixel[][3], uint16_t size);
  private:
    SPIClass * _spi;
    int _pin_ss;
    int _clock_speed;

    uint32_t spiSingleRead(uint32_t reg);
    void spiSingleWrite(uint32_t reg, uint32_t value);
    void spiMultiRead(uint32_t reg, uint32_t value[], uint16_t size);
    void spiMultiWrite(uint32_t reg, uint32_t value[], uint16_t size);
};

#endif /* _GOMIHGY_CASKET_H_ */
