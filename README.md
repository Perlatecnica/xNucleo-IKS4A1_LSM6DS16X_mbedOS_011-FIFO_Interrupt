# xNucleo-IKS4A1_LSM6DSV16X_mbedOS_011-FIFO_Interrupt

This application shows how to get accelerometer and gyroscope data from FIFO using interrupt and print them on terminal.

---
### Description

This code is an example of using the LSM6DSV16X inertial sensor to acquire data from the accelerometer and gyroscope using the FIFO (First-In, First-Out) mode. The sensor is configured to acquire data at a specific sampling frequency (ODR) and sensitivity (FS). Data is then read from the FIFO when a predefined threshold is exceeded.

### Configuration

- **SENSOR_ODR**: Sensor sampling frequency in Hz.
- **ACC_FS**: Accelerometer sensitivity in g.
- **GYR_FS**: Gyroscope sensitivity in dps (degrees per second).
- **MEASUREMENT_TIME_INTERVAL**: Time interval between each measurement in ms, calculated based on the sampling frequency.
- **FIFO_SAMPLE_THRESHOLD**: Threshold of the number of samples in the FIFO to trigger processing.
- **FLASH_BUFF_LEN**: Length of the buffer to store data read from the FIFO.
- **INT1_pin**: Microcontroller interrupt pin connected to the sensor INT1 output.

### Main Functions

- **main()**: Initializes the serial port, enables interrupt on the INT1 pin, initializes the LSM6DSV16X sensor, configures the ODR and sensitivity, and sets the FIFO mode. Handles data acquisition from the FIFO and printing to serial.
- **Read_FIFO_Data()**: Reads data from the FIFO when the threshold is exceeded and stores it in the buffer.
- **INT1_fullEvent_cb()**: Interrupt callback function called when the FIFO is full, setting a flag to signal that the data is ready to be read.

### Notes

- Interrupt handling on INT1 is essential to signal when the FIFO is full.
- Accelerometer and gyroscope data are read separately from the FIFO based on the tag.
- Data read from the FIFO is formatted with a timestamp and stored in a buffer for serial printing.

---

This description provides an overview of the code's functionality, including initialization, interrupt setup, sensor configuration, and wake-up event detection.