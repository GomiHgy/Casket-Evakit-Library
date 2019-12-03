/*
 *  @file Casket.cpp
 *
 *  https://github.com/osafune/osafune.github.io/blob/master/casket_quickdocs.md
 */
#include "Casket.h"

union ArrayToInteger {
  uint8_t array[4];
  uint32_t integer;
};

void Casket::begin(SPIClass * spi, int pin_ss, int clock_speed) {
  _spi = spi;
  _pin_ss = pin_ss;
  if (clock_speed > CASKET_SPI_MAX_SPEED) {
    clock_speed = CASKET_SPI_MAX_SPEED;
  }
  _clock_speed = clock_speed;
  pinMode(_pin_ss, OUTPUT);
  _spi->begin();
}

void Casket::begin(SPIClass * spi, int pin_sck, int8_t pin_miso, int pin_mosi, int pin_ss, int clock_speed) {
  _spi = spi;
  _pin_ss = pin_ss;
  if (clock_speed > CASKET_SPI_MAX_SPEED) {
    clock_speed = CASKET_SPI_MAX_SPEED;
  }
  _clock_speed = clock_speed;
  pinMode(_pin_ss, OUTPUT);
  _spi->begin(pin_sck, pin_miso, pin_mosi);
}

void Casket::end() {
  _spi->end();
  pinMode(_pin_ss, INPUT);
}

// SYSTEMIDを取得します
uint32_t Casket::readSystemID() {
  return spiSingleRead(CASKET_REG_ADDR_SYSTEMID);
}

// TIMECODEを取得します
uint32_t Casket::readTimeCode() {
  return spiSingleRead(CASKET_REG_ADDR_TIMECODE);
}

// UID下位32bitを取得します
uint32_t Casket::readUIDLow() {
  return spiSingleRead(CASKET_REG_ADDR_UID_LOW);
}

// UID上位32bitを取得します
uint32_t Casket::readUIDHigh() {
  return spiSingleRead(CASKET_REG_ADDR_UID_HIGH);
}

// ステータスレジスタを取得します
uint32_t Casket::readStatusReg() {
  return spiSingleRead(CASKET_REG_ADDR_STATUS);
}

// ペリフェラルレディレジスタ(RDY)が1の時trueを返します
// ('0'の時はアクセス実行中またはペリフェラルリセット状態)
bool Casket::isReady() {
  uint32_t reg = readStatusReg();
  return (reg & 0x00000001) != 0;
}

// ステータスレジスタを設定します
void Casket::writeStatusReg(uint32_t reg) {
  spiSingleWrite(CASKET_REG_ADDR_STATUS, reg);
}

// コンティニュアスモードに設定し、スタートします
void Casket::startContinuousMode() {
  uint32_t reg = readStatusReg();
  reg &= 0xFFFFFFE5;
  reg |= 0x00000002;
  writeStatusReg(reg);
}

// ワンショットモードに設定し、スタートします
void Casket::startOneshotMode() {
  uint32_t reg = readStatusReg();
  reg &= 0xFFFFFFE5;
  reg |= 0x0000000A;
  writeStatusReg(reg);
}

// コントロールレジスタを取得します
uint32_t Casket::readControlReg() {
  return spiSingleRead(CASKET_REG_ADDR_CTL);
}

// コントロールレジスタを設定します
void Casket::writeControlReg(uint32_t reg) {
  spiSingleWrite(CASKET_REG_ADDR_CTL, reg);
}

// RSTレジスタをセットします
void Casket::resetLedPeripheral() {
  uint32_t reg = readControlReg();
  reg |= 0x8000;
  writeControlReg(reg);
}

// スキャンピクセル数を設定します
void Casket::writeScanNum(uint16_t size) {
  uint32_t reg = readControlReg();
  if (size > 4096) {
    size = 4096;
  }
  if (size != 0) {
    size--;
  }
  reg &= 0xFFFFF000;
  reg |= size;
  writeControlReg(reg);
}

// ベーススクロールレジスタを取得します
uint32_t Casket::readBaseScroolReg() {
  return spiSingleRead(CASKET_REG_ADDR_BASE_SCROLL);
}

// ベーススクロールレジスタを設定します
void Casket::writeBaseScroolReg(uint32_t reg) {
  spiSingleWrite(CASKET_REG_ADDR_BASE_SCROLL, reg);
}

// ベース輝度ゲイン設定レジスタを取得します
uint32_t Casket::readBaseBrightnessReg() {
  return spiSingleRead(CASKET_REG_ADDR_BASE_BRIGHTNESS);
}

// ベース輝度ゲイン設定レジスタを設定します
void Casket::writeBaseBrightnessReg(uint8_t r, uint8_t g, uint8_t b) {
  uint32_t reg = b;
  reg += (uint32_t)(r << 16) & 0x00FF0000;
  reg += (uint32_t)(g << 8) & 0x0000FF00;
  spiSingleWrite(CASKET_REG_ADDR_BASE_BRIGHTNESS, reg);
}

// オーバーレイスクロールレジスタを取得します
uint32_t Casket::readOverlayScroolReg() {
  return spiSingleRead(CASKET_REG_ADDR_OVERLAY_SCROOL);
}

// オーバーレイスクロールレジスタを設定します
void Casket::writeOverlayScroolReg(uint32_t reg) {
  spiSingleWrite(CASKET_REG_ADDR_OVERLAY_SCROOL, reg);
}

// オーバーレイ輝度ゲイン設定レジスタを取得します
uint32_t Casket::readOverlayBrightnessReg() {
  return spiSingleRead(CASKET_REG_ADDR_OVERLAY_GAIN);
}

// オーバーレイ輝度ゲイン設定レジスタを設定します
bool Casket::writeOverlayBrightnessReg(uint8_t mode, uint8_t r, uint8_t g, uint8_t b) {
  if (mode >= 4) {
    return false;
  }
  uint32_t reg = (uint32_t)(mode << 24) & 0x03000000;
  reg += (uint32_t)(r << 16) & 0x00FF0000;
  reg += (uint32_t)(g << 8) & 0x0000FF00;
  reg += b;
  spiSingleWrite(CASKET_REG_ADDR_OVERLAY_GAIN, reg);

  return true;
}

// エフェクトシードレジスタを取得します
uint32_t Casket::readFreerunSeedReg() {
  return spiSingleRead(CASKET_REG_ADDR_EFFECT_SEED);
}

// エフェクトシードレジスタを設定します
void Casket::writeFreerunSeedReg(uint32_t seed) {
  spiSingleWrite(CASKET_REG_ADDR_EFFECT_SEED, seed);
}

// エフェクト輝度ゲイン設定レジスタを取得します
uint32_t Casket::readEffectBrightnessReg() {
  return spiSingleRead(CASKET_REG_ADDR_EFFECT_GAIN);
}

// エフェクト輝度ゲイン設定レジスタを設定します
bool Casket::writeEffectBrightnessReg(bool freerun, uint8_t mode, uint8_t r, uint8_t g, uint8_t b) {
  if (mode >= 4) {
    return false;
  }
  uint32_t reg = freerun ? 0x04000000 : 0;
  reg += (uint32_t)(mode << 24) & 0x03000000;
  reg += (uint32_t)(r << 16) & 0x00FF0000;
  reg += (uint32_t)(g << 8) & 0x0000FF00;
  reg += b;
  spiSingleWrite(CASKET_REG_ADDR_EFFECT_GAIN, reg);

  return true;
}

// ピクセルレジスタを取得します
uint32_t Casket::readBasePixel(uint8_t ch, uint8_t index) {
  if (ch >= CASKET_MEM_SIZE_CHANNEL) {
    return 0;
  }
  if (index >= CASKET_MEM_SIZE_LEDCOUNT) {
    return 0;
  }
  return spiSingleRead(CASKET_REG_ADDR_MEM_CH0 + (ch * CASKET_MEM_SIZE_LEDCOUNT * 4));
}

// ピクセルの色を設定します
bool Casket::writeBasePixel(uint8_t ch, uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (ch >= CASKET_MEM_SIZE_CHANNEL) {
    return false;
  }
  uint32_t color = (r & 0x3F) << 18;
  color += (g & 0x3F) << 10;
  color += (b & 0x3F) << 2;
  spiSingleWrite(CASKET_REG_ADDR_MEM_CH0 + (ch * CASKET_MEM_SIZE_LEDCOUNT * 4) + (index * 4), color);

  return true;
}

// ピクセルチャンネルの色を指定します
bool Casket::writeBasePixels(uint8_t ch, uint8_t pixel[][3], uint16_t size) {
  if (ch >= CASKET_MEM_SIZE_CHANNEL) {
    return false;
  }
  if (size >= CASKET_MEM_SIZE_LEDCOUNT) {
    return false;
  }
  uint32_t pixels[size];
  for (int index = 0; index < size; index++) {
    pixels[index] = (pixel[index][0] & 0x3F) << 18;
    pixels[index] += (pixel[index][1] & 0x3F) << 10;
    pixels[index] += (pixel[index][2] & 0x3F) << 2;
  }
  spiMultiWrite(CASKET_REG_ADDR_MEM_CH0 + (ch * CASKET_MEM_SIZE_LEDCOUNT * 4), pixels, size);

  return true;
}

// オーバーレイピクセルレジスタを取得します
uint32_t Casket::readOverlayPixel(uint8_t ch, uint8_t index) {
  if (ch >= CASKET_MEM_SIZE_CHANNEL) {
    return 0;
  }
  if (index >= CASKET_MEM_SIZE_LEDCOUNT) {
    return 0;
  }
  return spiSingleRead(CASKET_REG_ADDR_OVMEM_CH0 + (ch * CASKET_MEM_SIZE_LEDCOUNT * 4));
}

// オーバーレイピクセルの色を指定します
bool Casket::writeOverlayPixel(uint8_t ch, uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (ch >= CASKET_MEM_SIZE_CHANNEL) {
    return false;
  }
  uint32_t color = (r & 0x3F) << 18;
  color += (g & 0x3F) << 10;
  color += (b & 0x3F) << 2;
  spiSingleWrite(CASKET_REG_ADDR_OVMEM_CH0 + (ch * CASKET_MEM_SIZE_LEDCOUNT * 4) + (index * 4), color);

  return true;
}

// オーバーレイピクセルチャンネルの色を指定します
bool Casket::writeOverlayPixels(uint8_t ch, uint8_t pixel[][3], uint16_t size) {
  if (ch >= CASKET_MEM_SIZE_CHANNEL) {
    return false;
  }
  if (size >= CASKET_MEM_SIZE_LEDCOUNT) {
    return false;
  }
  uint32_t pixels[size];
  for (int index = 0; index < size; index++) {
    pixels[index] = (pixel[index][0] & 0x3F) << 18;
    pixels[index] += (pixel[index][1] & 0x3F) << 10;
    pixels[index] += (pixel[index][2] & 0x3F) << 2;
  }
  spiMultiWrite(CASKET_REG_ADDR_OVMEM_CH0 + (ch * CASKET_MEM_SIZE_LEDCOUNT * 4), pixels, size);

  return true;
}

uint32_t Casket::spiSingleRead(uint32_t reg)
{
  uint8_t ret = 0;
  ArrayToInteger values;
  uint8_t data[4];
  
  values.integer = (reg & 0x3FFFFF);
  data[0] = (values.array[2] & 0x3F) | CASKET_SPI_SINGLE_READ;
  data[1] = values.array[1];
  data[2] = values.array[0];
  data[3] = 0xFF;

  _spi->beginTransaction(SPISettings(_clock_speed, MSBFIRST, SPI_MODE0));
  digitalWrite(_pin_ss, LOW);
  for (int index = 0; index < 4; index++) {
    _spi->transfer(data[index]);
  }
  for (int index = 0; index < 4; index++) {
    data[index] = _spi->transfer(0);
  }
  digitalWrite(_pin_ss, HIGH);
  _spi->endTransaction();

  values.integer = 0;
  for (int index = 0; index < 4; index++) {
    values.array[3 - index] = data[index];
  }

  return values.integer;
}

void Casket::spiSingleWrite(uint32_t reg, uint32_t value)
{
  uint8_t ret = 0;
  ArrayToInteger values;
  uint8_t data[3];

  values.integer = (reg & 0x3FFFFF);
  data[0] = (values.array[2] & 0x3F) | CASKET_SPI_SINGLE_WRITE;
  data[1] = values.array[1];
  data[2] = values.array[0];

  values.integer = value;

  _spi->beginTransaction(SPISettings(_clock_speed, MSBFIRST, SPI_MODE0));
  digitalWrite(_pin_ss, LOW);
  for (int index = 0; index < 3; index++) {
    _spi->transfer(data[index]);
  }
  for (int index = 0; index < 4; index++) {
    _spi->transfer(values.array[3 - index]);
  }
  digitalWrite(_pin_ss, HIGH);
  _spi->endTransaction();
}

void Casket::spiMultiRead(uint32_t reg, uint32_t value[], uint16_t size)
{
  uint8_t ret = 0;
  ArrayToInteger values;
  uint8_t data[4];
  
  values.integer = (reg & 0x3FFFFF);
  data[0] = (values.array[2] & 0x3F) | CASKET_SPI_MULTI_READ;
  data[1] = values.array[1];
  data[2] = values.array[0];
  data[3] = 0xFF;

  _spi->beginTransaction(SPISettings(_clock_speed, MSBFIRST, SPI_MODE0));
  digitalWrite(_pin_ss, LOW);
  for (int index = 0; index < 4; index++) {
    _spi->transfer(data[index]);
  }
  for (int index = 0; index < size; index++) {
    values.integer = 0;
    for (int index2 = 0; index2 < 4; index2++) {
      values.array[3 - index2] = _spi->transfer(0);
    }
    value[index] = values.integer;
  }

  digitalWrite(_pin_ss, HIGH);
  _spi->endTransaction();
}

void Casket::spiMultiWrite(uint32_t reg, uint32_t value[], uint16_t size)
{
  uint8_t ret = 0;
  ArrayToInteger values;
  uint8_t data[3];

  values.integer = (reg & 0x3FFFFF);
  data[0] = (values.array[2] & 0x3F) | CASKET_SPI_MULTI_WRITE;
  data[1] = values.array[1];
  data[2] = values.array[0];

  _spi->beginTransaction(SPISettings(_clock_speed, MSBFIRST, SPI_MODE0));
  digitalWrite(_pin_ss, LOW);

  for (int index = 0; index < 3; index++) {
    _spi->transfer(data[index]);
  }
  for (int index = 0; index < size; index++) {
    values.integer = value[index];
    for (int index2 = 0; index2 < 4; index2++) {
      _spi->transfer(values.array[3 - index2]);
    }
  }

  digitalWrite(_pin_ss, HIGH);
  _spi->endTransaction();
}
