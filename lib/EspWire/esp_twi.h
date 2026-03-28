#ifndef ESP_TWI_H
#define ESP_TWI_H

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_OK                      0
#define I2C_SCL_HELD_LOW            1
#define I2C_SCL_HELD_LOW_AFTER_READ 2
#define I2C_SDA_HELD_LOW            3
#define I2C_SDA_HELD_LOW_AFTER_INIT 4

void esp_twi_init(unsigned char sda, unsigned char scl);
void esp_twi_stop(void);
void esp_twi_setClock(unsigned int freq);
void esp_twi_setClockStretchLimit(uint32_t limit);
uint8_t esp_twi_writeTo(unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);
uint8_t esp_twi_readFrom(unsigned char address, unsigned char * buf, unsigned int len, unsigned char sendStop);
uint8_t esp_twi_status();

#ifdef __cplusplus
}
#endif

#endif
