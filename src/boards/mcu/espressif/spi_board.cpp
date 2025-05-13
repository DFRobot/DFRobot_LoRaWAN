#if defined ESP8266 || defined ESP32
#include <SPI.h>
#include "boards/mcu/board.h"

SPIClass SPI_LORA;
//?????
void initSPI(void)
{

	SPI_LORA.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

}
#endif