# STM32-Nucleo-64-board-with-MAX30105-Pulse-Oximeter-and-Heart-Rate-Sensor

A simple Arduino project that reads heart rate and blood-oxygen saturation (SpO₂) using the SparkFun MAX30105 sensor and displays live readings on a 16×2 I²C LCD.

## Background

Originally, I built this project around the **MAX30100** sensor. During development, I ran into multiple problems:

- **Unstable Readings**: SpO₂ values jumped wildly from one measurement to the next.  
- **Detection Errors**: The sensor frequently failed to detect a finger or reported implausible values.  
- **Low Sampling Rate**: The MAX30100’s limited sample rate impacted responsiveness and accuracy.

To try to fix the MAX30100 issues, I followed the **2nd Method** described here:  
https://how2electronics.com/interfacing-max30100-pulse-oximeter-sensor-arduino/  

Despite wiring adjustments and placement tweaks per that guide, the readings remained unreliable.

To address these issues, I upgraded to the **MAX30105** sensor. Compared to the MAX30100, the MAX30105 provides:

- Higher sensitivity on both IR and RED channels  
- Sampling rates up to 3200 SPS for finer resolution  
- Better on-board noise rejection and FIFO buffering  

After switching, readings stabilized immediately. Below you can see side-by-side comparisons of my troubleshooting steps with the MAX30100 and the final working MAX30105 setup.

## Features

- **Initial Calibration**: Reads 100 samples (~4 s) to establish a baseline.  
- **Rolling Update**: Calculates HR & SpO₂ every second using Maxim’s algorithm.  
- **Live Display**: Shows BPM and SpO₂% on a 16×2 I²C LCD.  
- **Serial Output**: Streams raw samples and calculated values at 115200 bps for logging.  
- **Indicator LEDs**:  
  - PWM “Pulse” LED (D11) shows measurement in progress  
  - Blink LED (D13) pulses on each sample read  

## Hardware Requirements

- **Microcontroller**: Arduino Uno (or compatible)  
- **Sensor**: SparkFun MAX30105 Particle Sensor  
- **Display**: 16×2 I²C LCD (PCF8574 backpack recommended)  
- **Misc**: Jumper wires, breadboard

## Wiring

### MAX30105 to Arduino

| MAX30105 Pin | Arduino Pin |
|--------------|-------------|
| VIN          | 5 V         |
| GND          | GND         |
| SDA          | A4 (SDA)    |
| SCL          | A5 (SCL)    |
| INT          | (not used)  |

### LCD Backpack to Arduino

| LCD Pin | Arduino Pin |
|---------|-------------|
| VCC     | 5 V         |
| GND     | GND         |
| SDA     | A4          |
| SCL     | A5          |

### LEDs

| Purpose          | Arduino Pin |
|------------------|-------------|
| Pulse Indicator  | D11 (PWM)   |
| Read Indicator   | D13         |

## Software Installation

1. Install the [SparkFun MAX3010x Sensor Library](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)  
2. Install the [LiquidCrystal_I2C Library](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library)  
3. Clone this repository:  
   ```bash
   git clone https://github.com/<your-username>/max30105-spo2-heart-rate-monitor.git
   cd max30105-spo2-heart-rate-monitor

4. Open Max30105SPO2Monitor.ino in the Arduino IDE
5. Upload to your board

## Usage
1. Power the Arduino.
2. Place your fingertip gently on the MAX30105 window.
3. The LCD will display “Calibrating…” for ~4 s while gathering initial samples.
4. Once calibration completes, live HR and SpO₂ readings update every second.
5. View detailed graphs or logs via Serial at 115200 bps.

## Media
Troubleshooting with MAX30100
Initial MAX30100 wiring—noisy readings and frequent detection errors.
Experimenting with finger position; readings remained unstable.
Final MAX30105 Demo
Clean wiring on breadboard with MAX30105 and I²C LCD.
Stable BPM and SpO₂ values are shown on the 16×2 display.
<video src="media/video/max30105-demo.mp4" controls width="480"> Your browser does not support the video tag. </video>
