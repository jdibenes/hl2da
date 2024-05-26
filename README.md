# HoloLens 2 Data Acquisition (WIP)

Unified HoloLens 2 plugin for acquiring sensor data.

**Supported interfaces**

- Research Mode Visible Light Cameras (640x480 @ 30 FPS, Grayscale)
  - Left Front
  - Left Left
  - Right Front
  - Right Right
- Research Mode Depth
  - AHAT (512x512 @ 45 FPS, 16-bit Depth + 16-bit AB)
  - Long Throw (320x288 @ 5 FPS, 16-bit Depth + 16-bit AB + 8-bit Sigma)
- Research Mode IMU
  - Accelerometer
  - Gyroscope
  - Magnetometer
- Front Camera

## References

This project uses the HoloLens 2 Research Mode API, available at the [HoloLens2ForCV](https://github.com/microsoft/HoloLens2ForCV) repository.
