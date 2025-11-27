# K10 Base64 Library

An Arduino library for converting K10 camera module images and stored image files to Base64 encoding format. Designed specifically for ESP32 platforms with K10 camera integration.

## Features

- 📷 **Camera Frame Capture**: Capture live frames from K10 camera module
- 🔄 **Base64 Encoding**: Convert camera frames to Base64 strings
- 🖼️ **File Conversion**: Convert stored PNG/JPG images to Base64
- ⚡ **Efficient Processing**: Optimized memory handling for embedded systems
- 🎯 **Queue-based Architecture**: Thread-safe frame handling with FreeRTOS queues

## Hardware Requirements

- ESP32 microcontroller
- K10 camera module
- Sufficient RAM for image processing (PSRAM recommended)

## Software Dependencies

- `base64` library (for encoding)
- `unihiker_k10` library (for camera interface)
- ESP32 Arduino Core

## Installation

### Arduino IDE

1. Download this repository as a ZIP file
2. Open Arduino IDE
3. Go to **Sketch** → **Include Library** → **Add .ZIP Library**
4. Select the downloaded ZIP file
5. Restart Arduino IDE

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps =
    k10_base64
```

## Quick Start

### Basic Camera to Base64 Example

```cpp
#include <k10_base64.h>
#include <unihiker_k10.h>

K10_base64 encoder;

void setup() {
    Serial.begin(115200);
    
    // Initialize K10 camera
    // (initialization code depends on your setup)
    
    Serial.println("Camera ready");
}

void loop() {
    // Capture frame and convert to Base64
    String base64Image = encoder.K10tobase64();
    
    if (base64Image != "NULL") {
        Serial.println("Base64 Image Length: " + String(base64Image.length()));
        Serial.println(base64Image);
    } else {
        Serial.println("Failed to capture frame");
    }
    
    delay(5000);  // Wait 5 seconds between captures
}
```

### Image File to Base64 Example

```cpp
#include <k10_base64.h>

K10_base64 encoder;

void setup() {
    Serial.begin(115200);
    
    // Convert stored image to Base64
    String base64Data = encoder.imgtobase64("S:/photo.jpg");
    
    if (base64Data.length() > 0) {
        Serial.println("Image converted successfully");
        Serial.println("Base64 length: " + String(base64Data.length()));
        Serial.println(base64Data);
    } else {
        Serial.println("Failed to convert image");
    }
}

void loop() {}
```

## API Reference

### Class: K10_base64

The main class for Base64 encoding operations.

#### Constructor

```cpp
K10_base64()
```

Creates a new K10_base64 instance.

**Example:**
```cpp
K10_base64 encoder;
```

---

### Methods

#### `String K10tobase64(void)`

Captures the current video frame from K10 camera and converts it to Base64 encoding.

**Returns:**
- Base64 encoded string of the captured frame on success
- `"NULL"` if frame capture or conversion fails

**Process:**
1. Receives frame data from camera queue
2. Converts frame to JPEG format (quality: 80%)
3. Encodes JPEG data to Base64
4. Returns the Base64 string

**Example:**
```cpp
K10_base64 encoder;
String base64Frame = encoder.K10tobase64();

if (base64Frame != "NULL") {
    // Use the Base64 string
    Serial.println("Frame captured: " + base64Frame.substring(0, 50) + "...");
} else {
    Serial.println("Frame capture failed");
}
```

**Notes:**
- Requires camera queue (`xQueueCamer`) to be properly initialized
- Frame buffer is automatically returned after processing
- JPEG buffer is automatically freed after encoding

---

#### `String imgtobase64(String img)`

Converts a stored image file (PNG/JPG) to Base64 encoding.

**Parameters:**
- `img`: File path to the image (e.g., "S:/image.jpg")

**Returns:**
- Base64 encoded string of the image on success
- Empty string (`""`) if file read fails

**Supported Formats:**
- JPEG (.jpg, .jpeg)
- PNG (.png)

**Example:**
```cpp
K10_base64 encoder;

// Convert JPG image
String jpgBase64 = encoder.imgtobase64("S:/photo.jpg");

// Convert PNG image
String pngBase64 = encoder.imgtobase64("S:/icon.png");

if (jpgBase64.length() > 0) {
    Serial.println("JPG converted successfully");
}
```

**Notes:**
- Uses LVGL file system (`lv_fs_*` functions)
- Automatically calculates file size
- Memory is allocated dynamically and freed after encoding
- File path format depends on your LVGL configuration

---

## How It Works

### Camera Frame Processing Flow

```
1. Camera captures frame → Queue
2. K10tobase64() receives from queue
3. Frame converted to JPEG (80% quality)
4. JPEG data encoded to Base64
5. Memory cleaned up
6. Base64 string returned
```

### File Image Processing Flow

```
1. Open image file with LVGL filesystem
2. Calculate file size
3. Allocate buffer memory
4. Read entire file into buffer
5. Encode buffer to Base64
6. Free buffer memory
7. Base64 string returned
```

## Advanced Usage

### Batch Processing Multiple Frames

```cpp
#include <k10_base64.h>

K10_base64 encoder;
const int BATCH_SIZE = 10;

void captureBatch() {
    String frames[BATCH_SIZE];
    
    for (int i = 0; i < BATCH_SIZE; i++) {
        frames[i] = encoder.K10tobase64();
        
        if (frames[i] != "NULL") {
            Serial.printf("Frame %d captured (%d bytes)\n", 
                         i + 1, frames[i].length());
        }
        
        delay(100);  // Small delay between captures
    }
    
    // Process batch...
}
```

### Image Gallery Conversion

```cpp
#include <k10_base64.h>

K10_base64 encoder;

void convertGallery() {
    String images[] = {
        "S:/photo1.jpg",
        "S:/photo2.jpg",
        "S:/photo3.jpg"
    };
    
    for (int i = 0; i < 3; i++) {
        String base64 = encoder.imgtobase64(images[i]);
        
        if (base64.length() > 0) {
            Serial.printf("Converted %s (%d bytes)\n", 
                         images[i].c_str(), base64.length());
            // Save or transmit base64 data...
        }
    }
}
```

### Real-time Streaming

```cpp
#include <k10_base64.h>

K10_base64 encoder;

void streamCamera() {
    while (true) {
        String frame = encoder.K10tobase64();
        
        if (frame != "NULL") {
            // Send frame over WiFi/Serial
            sendFrameOverNetwork(frame);
        }
        
        // Adjust FPS
        delay(33);  // ~30 FPS
    }
}

void sendFrameOverNetwork(String base64Data) {
    // Your network transmission code here
}
```

### Memory-Efficient Processing

```cpp
#include <k10_base64.h>

K10_base64 encoder;

void processWithMemoryCheck() {
    // Check available memory
    size_t freeMem = ESP.getFreeHeap();
    Serial.printf("Free memory: %d bytes\n", freeMem);
    
    if (freeMem > 100000) {  // Ensure sufficient memory
        String base64 = encoder.K10tobase64();
        
        if (base64 != "NULL") {
            // Process image
            processImage(base64);
        }
        
        // Clear string to free memory
        base64 = "";
    } else {
        Serial.println("Insufficient memory");
    }
}
```

## Error Handling

### Camera Frame Errors

```cpp
String captureWithRetry(int maxAttempts = 3) {
    for (int i = 0; i < maxAttempts; i++) {
        String frame = encoder.K10tobase64();
        
        if (frame != "NULL") {
            return frame;  // Success
        }
        
        Serial.printf("Capture attempt %d failed\n", i + 1);
        delay(500);
    }
    
    return "NULL";  // All attempts failed
}
```

### File Read Errors

```cpp
String loadImageSafely(String path) {
    String base64 = encoder.imgtobase64(path);
    
    if (base64.length() == 0) {
        Serial.println("Error: Could not read " + path);
        Serial.println("Possible causes:");
        Serial.println("  - File does not exist");
        Serial.println("  - Insufficient memory");
        Serial.println("  - File system not mounted");
        return "";
    }
    
    return base64;
}
```

## Best Practices

1. **Memory Management**
   - Monitor heap usage with `ESP.getFreeHeap()`
   - Clear Base64 strings after use
   - Consider PSRAM for large images

2. **Queue Configuration**
   - Ensure `xQueueCamer` is properly initialized
   - Set appropriate queue size for your frame rate
   - Handle queue overflow conditions

3. **JPEG Quality**
   - Current setting: 80% (good balance)
   - Lower quality = smaller Base64 strings
   - Modify in source if needed

4. **File Paths**
   - Use correct LVGL drive prefix (e.g., "S:/")
   - Verify file exists before conversion
   - Handle path errors gracefully

5. **Performance**
   - Add delays between captures to prevent overflow
   - Use batch processing for efficiency
   - Monitor CPU usage during conversion

## Troubleshooting

### Issue: K10tobase64() returns "NULL"

**Possible Causes:**
- Camera queue not initialized
- No frame available in queue
- JPEG conversion failed
- Insufficient memory

**Solutions:**
```cpp
// Check queue before capturing
if (xQueueCamer != NULL) {
    String frame = encoder.K10tobase64();
}

// Check free memory
Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
```

### Issue: imgtobase64() returns empty string

**Possible Causes:**
- File not found
- Incorrect file path
- File system not mounted
- Memory allocation failed

**Solutions:**
```cpp
// Verify file exists first
File file = SD.open("/image.jpg");
if (file) {
    file.close();
    String base64 = encoder.imgtobase64("S:/image.jpg");
}

// Check file system
lv_fs_res_t res = lv_fs_is_ready('S');
Serial.printf("FS ready: %d\n", res);
```

### Issue: Out of memory during conversion

**Solutions:**
- Use smaller images
- Add PSRAM to your ESP32
- Process images sequentially, not in batch
- Clear unused variables

```cpp
// Monitor memory during operation
size_t before = ESP.getFreeHeap();
String base64 = encoder.K10tobase64();
size_t after = ESP.getFreeHeap();
Serial.printf("Memory used: %d bytes\n", before - after);
```

## Technical Details

### JPEG Compression

- **Quality**: 80% (configurable in source)
- **Format**: JPEG baseline
- **Color Space**: YCbCr

### Base64 Encoding

- **Standard**: RFC 4648
- **Character Set**: A-Z, a-z, 0-9, +, /
- **Padding**: Standard (=)
- **Output Length**: ~133% of input size

### Memory Usage

Approximate memory requirements:

| Image Size | JPEG Buffer | Base64 String | Total |
|------------|-------------|---------------|-------|
| 320x240    | ~20 KB      | ~27 KB        | ~47 KB |
| 640x480    | ~60 KB      | ~80 KB        | ~140 KB |
| 1024x768   | ~120 KB     | ~160 KB       | ~280 KB |

## Examples

Check the `examples/` folder for complete working examples:

1. **SimpleCapture**: Basic camera frame capture
2. **FileConversion**: Convert stored images
3. **StreamingExample**: Real-time Base64 streaming
4. **BatchProcessing**: Process multiple images
5. **ErrorHandling**: Robust error handling

## Dependencies

This library requires:

- **base64**: For encoding functionality
- **unihiker_k10**: For K10 camera interface
- **LVGL**: For file system operations
- **ESP32 Camera**: For frame capture

## Compatibility

- ✅ ESP32
- ✅ ESP32-S2
- ✅ ESP32-S3
- ✅ ESP32-C3 (with camera support)
- ❌ ESP8266 (no camera support)
- ❌ Arduino Uno/Mega (insufficient resources)

## License

This library is released under the MIT License.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Support

For issues and questions:
- Open an issue on GitHub
- Check K10 camera documentation
- Review example code

## Changelog

### Version 1.0.0
- Initial release
- Camera frame to Base64 conversion
- Image file to Base64 conversion
- Queue-based frame handling
- Memory-efficient processing

## Credits

Developed for ESP32 platforms with K10 camera module integration.
