# ECG DSP MCU Patient Monitor

Real-time ECG signal processing and heart-rate monitoring using **Arduino UNO, Python, Proteus, and a 128×64 GLCD**.

This project demonstrates how an ECG signal can be supplied from a computer, transmitted to an Arduino UNO in real time, processed using embedded signal-processing techniques, and displayed on a graphical LCD.

The project is designed as an educational and engineering demonstration of **ECG digital signal processing (DSP), QRS detection, RR-interval measurement, heart-rate calculation, rhythm classification, and simple rhythm irregularity detection on a microcontroller**.

> **Project status:** The current implementation includes real-time ECG signal processing and QRS/heart-rate detection. A dedicated ECG digital filter stage such as a low-pass, high-pass, or band-pass filter is planned as the next DSP development stage.

---

## 1. Project Overview

The system uses prerecorded ECG datasets as the signal source.

A Python program:

1. Loads an ECG dataset from a text file.
2. Converts the ECG samples to an 8-bit range (`0–255`).
3. Sends the samples to the Arduino UNO through a serial connection.
4. Maintains an approximate sampling rate of **128 samples/second**.

The Arduino UNO then:

1. Receives ECG samples through the serial interface.
2. Processes the ECG signal in real time.
3. Performs discrete derivative-based QRS detection.
4. Measures the RR interval.
5. Calculates heart rate.
6. Classifies the rhythm as:

   * Bradycardia
   * Normal
   * Tachycardia
7. Performs a simple RR-interval irregularity analysis.
8. Displays the ECG waveform and monitoring information on a **128×64 GLCD**.

The system can also be simulated using **Proteus**.

---

## 2. System Architecture

```text
             ECG Dataset
                  │
                  ▼
        ┌───────────────────┐
        │   Python Program  │
        │                   │
        │ Load ECG          │
        │ Normalize 0–255   │
        │ Timing @ 128 Hz   │
        └─────────┬─────────┘
                  │
             Serial / COM
                  │
                  ▼
        ┌───────────────────┐
        │    Arduino UNO    │
        │                   │
        │ ECG Sample Input  │
        │       │           │
        │       ▼           │
        │ Discrete          │
        │ Derivative        │
        │       │           │
        │       ▼           │
        │ QRS Detection     │
        │       │           │
        │       ▼           │
        │ RR Interval       │
        │       │           │
        │       ▼           │
        │ Heart Rate        │
        │       │           │
        │       ▼           │
        │ Rhythm            │
        │ Classification    │
        │       │           │
        │       ▼           │
        │ RR Irregularity   │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │   128×64 GLCD     │
        │                   │
        │ ECG waveform      │
        │ HR                │
        │ RR                │
        │ Rhythm            │
        │ QRS indicator     │
        │ Arrhythmia status │
        └───────────────────┘
```

---

## 3. Current Signal-Processing Pipeline

The current implementation uses the following processing chain:

```text
ECG TXT File
     │
     ▼
Python ECG Loader
     │
     ▼
Normalization to 0–255
     │
     ▼
Serial Transmission @ 128 Hz
     │
     ▼
Arduino UNO
     │
     ▼
Discrete ECG Derivative
     │
     ▼
Slope / QRS Detection
     │
     ▼
Refractory Period
     │
     ▼
RR Interval
     │
     ▼
Heart Rate
     │
     ├──► Bradycardia
     ├──► Normal
     └──► Tachycardia
     │
     ▼
RR Irregularity Analysis
     │
     ▼
GLCD Display
```

### Important DSP note

The project already performs digital signal-processing operations, including:

* Discrete differentiation
* Slope calculation
* Adaptive signal-level estimation
* Event detection
* Sampling-rate-based RR measurement
* Heart-rate calculation
* RR-interval analysis

However, the current version does **not yet contain a dedicated ECG filter such as a low-pass, high-pass, or band-pass FIR/IIR filter**.

The planned DSP architecture is:

```text
ECG
 │
 ▼
Digital Filter
 │
 ▼
Derivative
 │
 ▼
QRS Detection
 │
 ▼
RR Interval
 │
 ▼
Heart Rate
 │
 ▼
Rhythm / Irregularity Analysis
```

This will make the DSP pipeline more representative of a conventional ECG processing system.

---

## 4. Hardware

### Main hardware

* Arduino UNO
* 128×64 GLCD
* Computer running Python
* USB/Serial connection
* Proteus simulation environment

### GLCD

The project uses the `U8glib` library and a KS0108-compatible 128×64 GLCD configuration.

The display is used to show:

* ECG waveform
* QRS detection indicator
* RR interval
* Heart rate
* Rhythm classification
* Arrhythmia/irregularity status

---

## 5. Software

### Python

Python is used as the ECG signal source and serial transmitter.

Main responsibilities:

* Read ECG datasets
* Prepare sample sequences
* Normalize samples
* Maintain approximately 128 Hz transmission
* Send samples through the serial port

### Arduino C++

Arduino is responsible for real-time embedded processing.

Main responsibilities:

* Serial ECG acquisition
* Digital signal processing
* QRS detection
* RR calculation
* Heart-rate calculation
* Rhythm classification
* RR irregularity analysis
* GLCD rendering

### Proteus

Proteus is used to simulate the Arduino-based patient monitor and its display hardware.

---

## 6. Repository Structure

```text
ECG-DSP-MCU-Patient-Monitor/
│
├── .gitignore
│
├── ECG_DSP_GitHub_Demo.mp4
│
├── ECG_DSP_MCU.pdsprj
│
├── main.ino
│
├── send_raw_ecg.py
│
├── ecg_normal_10s.txt
├── ecg_brady_10s.txt
└── ecg_tachy_10s.txt
```

### File descriptions

| File                      | Description                                                                                                       |
| ------------------------- | ----------------------------------------------------------------------------------------------------------------- |
| `main.ino`                | Main Arduino firmware for ECG acquisition, QRS detection, HR calculation, rhythm classification, and GLCD display |
| `send_raw_ecg.py`         | Python ECG loader and serial transmitter                                                                          |
| `ecg_normal_10s.txt`      | 10-second normal ECG test dataset                                                                                 |
| `ecg_brady_10s.txt`       | 10-second bradycardia test dataset                                                                                |
| `ecg_tachy_10s.txt`       | 10-second tachycardia test dataset                                                                                |
| `ECG_DSP_MCU.pdsprj`      | Proteus simulation project                                                                                        |
| `ECG_DSP_GitHub_Demo.mp4` | Project demonstration video                                                                                       |
| `.gitignore`              | Git files excluded from version control                                                                           |

---

## 7. ECG Test Data

The repository contains three ECG test datasets:

```text
ecg_normal_10s.txt
ecg_brady_10s.txt
ecg_tachy_10s.txt
```

Each test contains approximately:

```text
128 samples/second × 10 seconds
= 1280 samples
```

The Python transmitter expects a 10-second ECG sequence and prepares the data for transmission at approximately **128 Hz**.

The datasets are used to test different heart-rate conditions.

### Normal

Expected classification:

```text
NORMAL
```

### Bradycardia

Expected classification:

```text
BRADY
```

### Tachycardia

Expected classification:

```text
TACHY
```

---

## 8. Sampling Rate

The project uses:

```text
Sampling frequency = 128 Hz
```

Therefore:

```text
Sampling period = 1 / 128
                  ≈ 7.8125 ms
```

For a 10-second dataset:

```text
128 × 10 = 1280 samples
```

The Arduino firmware uses:

```cpp
#define FS 128
```

The Python transmitter also uses:

```python
FS = 128
```

Maintaining the same sampling frequency on both sides is important because RR interval and heart-rate calculations depend directly on the sampling rate.

---

## 9. ECG Sample Representation

The Python program reads ECG values from the text files and normalizes each dataset into the range:

```text
0–255
```

These values can then be transmitted as one-byte samples through the serial interface.

Conceptually:

```text
Original ECG
     │
     ▼
Min/Max normalization
     │
     ▼
0 ─────────────── 255
```

This representation is convenient for an Arduino UNO because each sample can be transmitted and processed as an 8-bit value.

### Important limitation

The current implementation does **not** transmit calibrated ECG values in physical units such as millivolts.

The Arduino therefore processes a normalized digital representation of the ECG waveform.

This is appropriate for the current signal-processing demonstration, but a future hardware implementation could use calibrated ADC measurements and preserve the physical amplitude scale.

---

## 10. QRS Detection

The current firmware uses a lightweight derivative-based QRS detection approach.

For each incoming sample:

```text
x[n]
```

the firmware calculates the discrete derivative:

```text
d[n] = x[n] - x[n-1]
```

The magnitude of the slope is then considered:

```text
|d[n]|
```

The detector uses several conditions including:

* Slope strength
* Derivative direction
* Adaptive signal level
* Refractory period
* Minimum sample count

A simplified representation is:

```text
ECG sample
    │
    ▼
Difference between samples
    │
    ▼
Absolute slope
    │
    ▼
Threshold / adaptive level
    │
    ▼
Refractory check
    │
    ▼
QRS detected
```

The refractory period prevents multiple detections of the same cardiac complex.

The current firmware uses approximately:

```text
51 samples
```

At 128 Hz this corresponds to approximately:

```text
51 / 128 ≈ 0.398 seconds
```

---

## 11. RR Interval Calculation

When a QRS complex is detected, the Arduino records its sample position.

For two consecutive QRS detections:

```text
RR samples = Current QRS sample - Previous QRS sample
```

The RR interval in seconds is:

```text
RR = RR_samples / FS
```

For example, if:

```text
RR_samples = 128
FS = 128 Hz
```

then:

```text
RR = 1 second
```

---

## 12. Heart Rate Calculation

Heart rate is calculated from the RR interval:

```text
HR = 60 / RR
```

Using sample counts:

```text
HR = (60 × FS) / RR_samples
```

For example:

```text
FS = 128 Hz
RR = 128 samples

HR = (60 × 128) / 128
   = 60 BPM
```

The firmware then classifies the rhythm according to the configured thresholds.

---

## 13. Rhythm Classification

The current implementation uses the following simple heart-rate thresholds:

|  Heart Rate | Classification |
| ----------: | -------------- |
|  `< 60 BPM` | Bradycardia    |
| `60–99 BPM` | Normal         |
| `≥ 100 BPM` | Tachycardia    |

These thresholds are implemented as a simple educational classification mechanism.

They should not be interpreted as a clinical diagnostic system.

---

## 14. RR Irregularity Detection

The firmware also maintains a short history of RR intervals.

The current implementation compares the current RR interval with the previous RR interval.

A difference greater than approximately:

```text
20%
```

is treated as an irregular RR change.

Multiple irregular changes can cause:

```text
ARRHYTHMIA
```

to be displayed.

This is a **simple algorithmic irregularity indicator**, not a clinical arrhythmia diagnostic algorithm.

It is intended to demonstrate how beat-to-beat timing can be analyzed on a microcontroller.

---

## 15. GLCD Display

The 128×64 GLCD displays the real-time monitoring information.

The interface includes:

```text
QRS indicator
RR interval
Heart rate
ECG waveform
Rhythm classification
Arrhythmia status
```

Conceptually:

```text
┌──────────────────────────────┐
│ QRS        RR: 0.82s         │
│ HR: 73 BPM                   │
│                              │
│       /\       /\            │
│      /  \     /  \           │
│ ____/    \___/    \____      │
│                              │
│ NORMAL                       │
└──────────────────────────────┘
```

---

## 16. Python Serial Transmitter

The Python script:

```text
send_raw_ecg.py
```

uses the serial connection to send ECG samples to the Arduino.

The current configuration is:

```python
PORT = "COM11"
BAUD = 115200
FS = 128
DURATION_SECONDS = 10
```

If the Arduino appears on another COM port, change:

```python
PORT = "COM11"
```

to the appropriate port.

For example:

```python
PORT = "COM5"
```

The baud rate must remain consistent with the Arduino firmware:

```cpp
#define SERIAL_BAUD 115200
```

---

## 17. Running the Project

### Step 1 — Install Python dependencies

Install PySerial:

```powershell
pip install pyserial
```

Verify Python:

```powershell
python --version
```

---

### Step 2 — Connect the Arduino

Connect the Arduino UNO to the computer using USB.

Find the assigned COM port in Windows Device Manager.

Update:

```python
PORT = "COM11"
```

in:

```text
send_raw_ecg.py
```

if necessary.

---

### Step 3 — Upload the Arduino firmware

Open:

```text
main.ino
```

in the Arduino IDE.

Install the required `U8glib` library if it is not already installed.

Select:

```text
Board: Arduino UNO
```

Select the correct COM port and upload the firmware.

---

### Step 4 — Run the Python transmitter

From the project directory:

```powershell
cd C:\ECG_DSP_MCU
```

Run:

```powershell
python send_raw_ecg.py
```

The script sends:

```text
NORMAL ECG
       ↓
BRADYCARDIA ECG
       ↓
TACHYCARDIA ECG
```

with a short delay between datasets.

---

## 18. Proteus Simulation

The Proteus project is provided as:

```text
ECG_DSP_MCU.pdsprj
```

The simulation is intended to demonstrate the embedded patient-monitor interface and GLCD behavior.

The Python program can be used as the ECG signal source in the overall development workflow, while Proteus provides the simulated hardware environment.

The exact serial configuration may depend on the local Proteus/virtual-COM setup.

---

## 19. Serial Output

The Arduino firmware also reports detected cardiac events through the serial interface.

A typical event has the form:

```text
QRS RR:0.82 HR:73 NORMAL ARR=NO
```

This provides a useful way to verify the embedded algorithm independently of the GLCD.

The serial output can be used during development to inspect:

* QRS detection
* RR interval
* Heart rate
* Rhythm classification
* RR irregularity status

---

## 20. DSP Development Roadmap

The current implementation provides the foundation for a more complete ECG DSP pipeline.

### Current

```text
ECG
 ↓
Derivative
 ↓
Slope-based QRS detection
 ↓
RR
 ↓
HR
 ↓
Rhythm classification
```

### Planned DSP pipeline

```text
ECG
 ↓
Digital filtering
 ↓
Baseline/noise reduction
 ↓
Derivative
 ↓
Squaring / energy estimation
 ↓
Moving-window integration
 ↓
Adaptive QRS threshold
 ↓
RR interval
 ↓
Heart rate
 ↓
Rhythm / irregularity analysis
```

Potential future DSP stages include:

* High-pass filtering
* Low-pass filtering
* Band-pass filtering
* FIR implementation
* IIR implementation
* Moving-window integration
* Adaptive thresholding
* Improved QRS detection
* Noise robustness
* Baseline-wander reduction
* 50/60 Hz interference rejection
* Fixed-point optimization for Arduino

---

## 21. MATLAB / GNU Octave DSP Development

MATLAB or GNU Octave can be used during development to analyze the ECG signal before implementing the final algorithm on the Arduino.

A useful workflow is:

```text
ECG Dataset
     │
     ▼
MATLAB / Octave
     │
     ├── Plot raw ECG
     ├── Analyze spectrum
     ├── Design filter
     ├── Test filter
     ├── Detect R-peaks
     └── Validate algorithm
     │
     ▼
Arduino implementation
```

This allows the DSP algorithm to be validated in a high-level environment before being converted into MCU-compatible code.

---

## 22. Why Python, MATLAB/Octave, Arduino, and Proteus?

Each tool has a different role.

| Tool            | Role                                                             |
| --------------- | ---------------------------------------------------------------- |
| Python          | ECG data source and real-time serial transmission                |
| MATLAB / Octave | DSP development, visualization, filtering and algorithm analysis |
| Arduino UNO     | Embedded real-time ECG processing                                |
| Proteus         | Hardware and GLCD simulation                                     |
| Git/GitHub      | Version control and project documentation                        |

This separation makes it possible to develop and validate the signal-processing algorithm before optimizing it for the microcontroller.

---

## 23. Resource Considerations

The target microcontroller is an **Arduino UNO**, based on the ATmega328P.

Because the MCU has limited:

* RAM
* Flash
* CPU performance

DSP algorithms should be selected with embedded constraints in mind.

For this reason, future filtering stages should consider:

* Number of coefficients
* Integer/fixed-point arithmetic
* Memory usage
* Processing time per sample
* Numerical stability
* Sampling frequency

At 128 Hz, the system has approximately:

```text
7.8125 ms
```

between consecutive samples.

The processing pipeline must therefore complete efficiently enough to maintain real-time operation.

---

## 24. Validation Strategy

The project can be validated in several stages.

### Stage 1 — Signal transmission

Verify that:

```text
Python → Serial → Arduino
```

is receiving ECG samples.

### Stage 2 — Waveform display

Verify that the GLCD displays a recognizable ECG waveform.

### Stage 3 — QRS detection

Verify that QRS events occur at expected cardiac complexes.

### Stage 4 — RR calculation

Compare measured RR intervals with the expected ECG beat spacing.

### Stage 5 — Heart-rate calculation

Verify the calculated BPM against the expected rhythm.

### Stage 6 — Rhythm classification

Test:

```text
Normal
Bradycardia
Tachycardia
```

datasets.

### Stage 7 — DSP filtering

After adding a dedicated filter, compare:

```text
Raw ECG
vs.
Filtered ECG
```

and evaluate its effect on QRS detection.

---

## 25. Limitations

This project is an **educational embedded DSP prototype** and is not a medical device.

Current limitations include:

* ECG samples are normalized to 8-bit values.
* The current input is prerecorded data rather than a physical ECG sensor.
* The current QRS detector is a lightweight custom algorithm.
* The current implementation does not yet contain a dedicated ECG band-pass filter.
* Rhythm classification is based primarily on heart rate.
* RR irregularity detection is intentionally simple.
* No clinical validation has been performed.
* No medical diagnostic claims should be made from the output.

The project is intended for learning, algorithm development, embedded DSP experimentation, and engineering demonstration.

---

## 26. Future Improvements

Possible future versions may include:

* Real ECG sensor input
* ADC-based ECG acquisition
* Digital band-pass filtering
* FIR filter implementation
* IIR filter implementation
* Pan-Tompkins-inspired QRS detection
* Improved adaptive thresholds
* Noise detection
* Baseline-wander removal
* Power-line interference rejection
* More robust RR analysis
* PVC detection
* AF-related irregularity analysis
* Signal-quality estimation
* More advanced GLCD interface
* Data logging
* UART/USB streaming
* Python real-time visualization
* MATLAB/Octave validation tools
* Fixed-point DSP optimization
* Additional ECG datasets

---

## 27. Development Philosophy

The project follows a progressive development approach:

```text
Understand the ECG signal
          ↓
Analyze the signal
          ↓
Develop DSP algorithm
          ↓
Validate in MATLAB/Octave
          ↓
Implement on Arduino
          ↓
Test with prerecorded ECG
          ↓
Validate real-time behavior
          ↓
Optimize for MCU
```

The goal is not simply to detect heart rate, but to demonstrate the complete path from an ECG dataset to a real-time embedded signal-processing system.

---

## 28. Project Status

### Implemented

* [x] ECG test datasets
* [x] Python ECG loader
* [x] ECG normalization
* [x] Serial ECG transmission
* [x] 128 Hz sample timing
* [x] Arduino ECG reception
* [x] Discrete derivative processing
* [x] QRS detection
* [x] RR interval calculation
* [x] Heart-rate calculation
* [x] Bradycardia classification
* [x] Normal rhythm classification
* [x] Tachycardia classification
* [x] Simple RR irregularity analysis
* [x] 128×64 GLCD waveform display
* [x] Proteus project
* [x] ECG test datasets in repository

### In development

* [ ] Dedicated digital ECG filter
* [ ] Improved DSP/QRS pipeline
* [ ] DSP validation in MATLAB/Octave
* [ ] Filter performance comparison
* [ ] MCU-oriented DSP optimization

---

## 29. Disclaimer

This project is intended for **educational and engineering purposes only**.

It is not intended for clinical diagnosis, patient monitoring in a medical setting, treatment decisions, or replacement of certified medical equipment.

Any ECG classification or arrhythmia indication produced by the software should be treated as a software demonstration rather than a medical conclusion.

---

## 30. Author / Repository

This repository contains the source code, ECG test datasets, Proteus simulation project, and demonstration material for the ECG DSP MCU Patient Monitor.

**Repository:**

`ECG-DSP-MCU-Patient-Monitor`

The project is maintained as an embedded systems and digital signal-processing development project focused on ECG analysis using a resource-constrained microcontroller.
