#include "joystick.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED 1000000
#define MCP3208_READ_CMD 0x06

static int spi_fd = -1;
static int readChannel(int channel);

// initialize the joystick spi communication
bool joystick_initialize(void)
{
    // open spi device
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0)
    {
        perror("Joystick SPI open failed");
        return false;
    }

    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED;

    // send configuration to spi driver using ioctl system calls
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0)
    {
        perror("SPI set mode failed");
    }

    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0)
    {
        perror("SPI set bits failed");
    }

    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        perror("SPI set speed failed");
    }

    printf("Joystick initialized.\n");
    return true;
}

// read joystick position and determine direction
joystickDirection joystick_getDirection(void)
{
    int x = readChannel(0);
    int y = readChannel(1);

    // MCP3208 gives 12-bit values (0-4095)
    // Center is ~2048, threshold for direction detection
    const int CENTER = 2048;
    const int THRESHOLD = 800; // Adjust this if too sensitive/insensitive

    // X-axis controls forward/backward
    // X up (forward) is ~4095, X down (backward) is ~0
    if (x > CENTER + THRESHOLD)
    {
        return joy_up; // Forward
    }
    if (x < CENTER - THRESHOLD)
    {
        return joy_down; // Backward
    }

    // Y-axis controls left/right
    // Y left is ~4095, Y right is ~0
    if (y > CENTER + THRESHOLD)
    {
        return joy_left; // Left
    }
    if (y < CENTER - THRESHOLD)
    {
        return joy_right; // Right
    }

    return joy_rest;
}

// close spi device when finished
void joystick_cleanup(void)
{
    if (spi_fd >= 0)
        close(spi_fd);
    spi_fd = -1;
}

// read adc channel from the chip using spi
static int readChannel(int channel)
{
    // prepare spi transmit buffer
    uint8_t tx[3], rx[3];
    tx[0] = MCP3208_READ_CMD | ((channel & 0x07) >> 2);
    tx[1] = ((channel & 0x07) << 6);
    tx[2] = 0;

    // transfer structure
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 1)
    {
        perror("SPI read failed");
        return -1;
    }

    // combine received bytes into a 12-bit adc value
    return ((rx[1] & 0x0F) << 8) | rx[2];
}