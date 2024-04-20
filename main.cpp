#include "mbed.h"
#include "LSM6DSV16X.h"

#define SENSOR_ODR 104.0f // In Hertz
#define ACC_FS 2 // In g
#define GYR_FS 2000 // In dps
#define MEASUREMENT_TIME_INTERVAL (1000.0f/SENSOR_ODR) // In ms
#define FIFO_SAMPLE_THRESHOLD 199
#define FLASH_BUFF_LEN 8192
#define INT1_pin PA4 // MCU input pin connected to sensor INT1 output pin 

LSM6DSV16X AccGyr(I2C_SDA, I2C_SCL); 
Serial pc(USBTX, USBRX);
DigitalOut led(LED1);
InterruptIn intPin(D5);

volatile uint8_t fullFlag = 0; // FIFO full flag
uint8_t status = 0;
unsigned long timestamp_count = 0;
bool acc_available = false;
bool gyr_available = false;
int32_t acc_value[3];
int32_t gyr_value[3];
char buff[FLASH_BUFF_LEN];
uint32_t pos = 0;

void Read_FIFO_Data();
void INT1_fullEvent_cb();

// main() runs in its own thread in the OS
int main()
{
    pc.baud(115200);
 
    // Enable INT1 pin.
    //Interrupts.
    intPin.rise(&INT1_fullEvent_cb);

    // Initialize LSM6DSV16X.
    AccGyr.begin();
    status |= AccGyr.Enable_X();
    status |= AccGyr.Enable_G();

    // Configure ODR and FS of the acc and gyro
    status |= AccGyr.Set_X_ODR(SENSOR_ODR);
    status |= AccGyr.Set_X_FS(ACC_FS);
    status |= AccGyr.Set_G_ODR(SENSOR_ODR);
    status |= AccGyr.Set_G_FS(GYR_FS);

    // Configure FIFO BDR for acc and gyro
    status |= AccGyr.FIFO_Set_X_BDR(SENSOR_ODR);
    status |= AccGyr.FIFO_Set_G_BDR(SENSOR_ODR);

    // Set Set FIFO watermark level
    status |= AccGyr.FIFO_Set_Watermark_Level(FIFO_SAMPLE_THRESHOLD);
    // Set FIFO stop on watermark level
    status |= AccGyr.FIFO_Set_Stop_On_Fth(1);
    // Enable FIFO full interrupt on sensor INT1 pin
    status |= AccGyr.FIFO_Set_INT1_FIFO_Full(1);
    // Set FIFO in Continuous mode
    status |= AccGyr.FIFO_Set_Mode(LSM6DSV16X_STREAM_MODE);

    if (status != LSM6DSV16X_OK) {
        pc.printf("LSM6DSV16X Sensor failed to init/configure\r\n");
        while (1);
    }
    pc.printf("LSM6DSV16X FIFO Demo\r\n");

    while (true) {
        uint8_t fullStatus = 0;

        // If we reach the threshold we can empty the FIFO
        if (fullFlag != 0) {
            fullFlag = 0;

            if (AccGyr.FIFO_Get_Full_Status(&fullStatus) != LSM6DSV16X_OK) {
            pc.printf("LSM6DSV16X Sensor failed to get full status\r\n");
            while (1);
            }

            if (fullStatus) {
            fullStatus = 0;

            // Empty the FIFO
            Read_FIFO_Data();

            // Print FIFO data
            pc.printf("%s",buff);
            }
        }
            }
}

void Read_FIFO_Data()
{
  uint16_t i;
  uint16_t samples_to_read;

  // Check the number of samples inside FIFO
  if (AccGyr.FIFO_Get_Num_Samples(&samples_to_read) != LSM6DSV16X_OK) {
    pc.printf("LSM6DSV16X Sensor failed to get number of samples inside FIFO\r\n");
    while (1);
  }

  for (i = 0; i < samples_to_read; i++) {
    uint8_t tag;

    // Check the FIFO tag
    if (AccGyr.FIFO_Get_Tag(&tag) != LSM6DSV16X_OK) {
      pc.printf("LSM6DSV16X Sensor failed to get tag\r\n");
      while (1);
    }
    switch (tag) {
      // If we have a gyro tag, read the gyro data
      case 1: {
          if (AccGyr.FIFO_Get_G_Axes(gyr_value) != LSM6DSV16X_OK) {
            pc.printf("LSM6DSV16X Sensor failed to get gyroscope data\r\n");
            while (1);
          }
          gyr_available = true;
          break;
        }

      // If we have an acc tag, read the acc data
      case 2: {
          if (AccGyr.FIFO_Get_X_Axes(acc_value) != LSM6DSV16X_OK) {
            pc.printf("LSM6DSV16X Sensor failed to get accelerometer data\r\n");
            while (1);
          }
          acc_available = true;
          break;
        }

      // We can discard other tags
      default: {
          break;
        }
    }
    // If we have the measurements of both acc and gyro, we can store them with timestamp
    if (acc_available && gyr_available) {
      int num_bytes;
      num_bytes = snprintf(&buff[pos], (FLASH_BUFF_LEN - pos), "%lu %d %d %d %d %d %d\r\n", (unsigned long)((float)timestamp_count * MEASUREMENT_TIME_INTERVAL), (int)acc_value[0], (int)acc_value[1], (int)acc_value[2], (int)gyr_value[0], (int)gyr_value[1], (int)gyr_value[2]);
      pos += num_bytes;
      timestamp_count++;
      acc_available = false;
      gyr_available = false;
    }
  }
  // We can add the termination character to the string, so we are ready to print it on hyper-terminal
  buff[pos] = '\0';
  pos = 0;
}

// ISR callback for INT1
void INT1_fullEvent_cb()
{
  fullFlag = 1;
}

