#include "src/MFRC522v2.h"
#include "src/MFRC522Debug.h"
#include "src/MFRC522DriverSPI.h"

#if defined(_WIN32)
#define MAKE_WIN_TEST
#endif

#ifndef MAKE_WIN_TEST
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <iostream>

static void pabort(const char* s)
{
	perror(s);
	abort();
}

static const char* device = "/dev/spidev0.0";
static uint8_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 4000000;// 4MHz
static uint16_t delay_us = 0;

int fd = 0;

class tDevSPI : public MFRC522DriverSPI
{
	std::vector<uint8_t> Transaction(const std::vector<uint8_t>& tx) override
	{
    	std::vector<uint8_t> rx;
    	rx.resize(tx.size());

    	struct spi_ioc_transfer tr = {
    		.tx_buf = (unsigned long)tx.data(),
    		.rx_buf = (unsigned long)rx.data(),
    		.len = tx.size(),
    		.speed_hz = speed,
    		.delay_usecs = delay_us,
    		.bits_per_word = bits,
      };

      int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
      if (ret < 1)
          pabort("can't send spi message");

      return rx;
	}
};

tDevSPI g_DriverSPI;
MFRC522 mfrc522(g_DriverSPI);

Print g_Log;

void loop()
{
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( !mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards.
	if ( !mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called.
	MFRC522Debug::PICC_DumpToSerial(mfrc522, g_Log, &(mfrc522.uid));
}

int main()
{
	g_time_start = std::chrono::steady_clock::now();

	//parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);

	mfrc522.PCD_Init(); // Init MFRC522 card
	delay(4);			// Optional delay. Some board do need more time after init to be ready, see Readme
	MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, g_Log);
	g_Log.println(mfrc522.PCD_GetAntennaGain(), HEX);
	g_Log.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

	//mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
	//mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_min);
	//g_Log.println(mfrc522.PCD_GetAntennaGain(), HEX);

	while (true)
		loop();

	close(fd);

	return 0;
}
#else // MAKE_WIN_TEST
std::vector<uint8_t> SPITransfer(const std::vector<uint8_t>& tx)
{
	return {};
}

int main()
{
	g_time_start = std::chrono::steady_clock::now();
	delay(10);
	auto time = millis();

	return 0;
}
#endif // MAKE_WIN_TEST
