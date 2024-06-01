# HoloLens 2 Data Acquisition

Unified HoloLens 2 plugin for acquiring sensor data.
The plugin enables access to most of the HoloLens sensors, including the Research Mode sensors, through a common interface and allows associating data from multiple streams via timestamps.
It is compatible with Unity, Unreal, and native UWP applications.
A sample Unity project (2020.3.42f1) can be found in the [hl2da_unity](hl2da_unity) folder and a sample Unreal project (4.27.2) is in the [hl2da_unreal](hl2da_unreal) folder.

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
- Front Camera (Configurable resolution and framerate, NV12)
- Microphone (2 or 5 channels @ 48000 Hz, 32-bit Float)
- Spatial Input (30 Hz)
  - Head Tracking
  - Eye Tracking
  - Hand Tracking
- Extended Eye Tracking (30, 60, or 90 FPS)

## References

This project uses the HoloLens 2 Research Mode API, available at the [HoloLens2ForCV](https://github.com/microsoft/HoloLens2ForCV) repository.
