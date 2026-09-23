#include <Arduino.h>
#include <U8glib.h>

// ======================================================
// GLCD KS0108 128x64
// ======================================================

U8GLIB_KS0108_128 display(
  2, 3, 4, 5,       // DB0 - DB3
  10, 11, 12, 13,   // DB4 - DB7
  6,                 // E
  A0, A1,            // CS1, CS2
  7,                 // DI
  8                  // RW
);

// ======================================================
// ECG SETTINGS
// ======================================================

#define ECG_SAMPLES 128

#define GRAPH_TOP     12
#define GRAPH_BOTTOM  56

#define SERIAL_BAUD 115200
#define FS 128

// ======================================================
// DISPLAY TIME SCALE
// ======================================================
//
// 1 = 1 second across screen
// 2 = 2 seconds across screen
// 3 = 3 seconds across screen
//
// Acquisition remains 128 Hz.
// Only DISPLAY is slowed.
//

#define DISPLAY_SAMPLES_PER_PIXEL 2


// ======================================================
// ECG DISPLAY BUFFER
// ======================================================

uint8_t displayBuffer[ECG_SAMPLES];

uint8_t displayX = 0;

uint8_t displaySampleCounter = 0;

uint16_t displaySampleSum = 0;


// ======================================================
// QRS / RR / HR
// ======================================================

uint32_t sampleCounter = 0;
uint32_t lastQrsSample = 0;

uint16_t rrSamples = 0;
uint16_t heartRate = 0;

bool havePreviousQRS = false;
bool qrsDetected = false;


// ======================================================
// QRS DETECTOR
// ======================================================

int16_t previousSample = 128;
int16_t previousDerivative = 0;

uint16_t signalLevel = 0;
uint16_t refractoryCounter = 0;


// ======================================================
// DISPLAY TEXT
// ======================================================

char rrText[12];
char hrText[12];

const char* rhythmText = "---";

uint8_t qrsIndicator = 0;


// ======================================================
// ARRHYTHMIA DETECTION
// ======================================================

uint16_t rrHistory[4];

uint8_t rrCount = 0;

bool arrhythmiaDetected = false;

uint8_t irregularCount = 0;


// ======================================================
// RESET DISPLAY SWEEP
// ======================================================

void resetDisplaySweep()
{
    displayX = 0;

    displaySampleCounter = 0;

    displaySampleSum = 0;


    for (uint8_t i = 0; i < ECG_SAMPLES; i++)
    {
        displayBuffer[i] = 128;
    }
}


// ======================================================
// ADD SAMPLE TO DISPLAY
// ======================================================
//
// Important:
//
// This function is ONLY for the GLCD.
//
// QRS detection still receives every
// 128 Hz sample.
//

void addDisplaySample(uint8_t sample)
{
    displaySampleSum += sample;

    displaySampleCounter++;


    if (
        displaySampleCounter
        >= DISPLAY_SAMPLES_PER_PIXEL
    )
    {
        uint8_t averagedSample =
            displaySampleSum /
            DISPLAY_SAMPLES_PER_PIXEL;


        displayBuffer[displayX] =
            averagedSample;


        displayX++;


        // ------------------------------------------
        // Sweep reached right edge
        // ------------------------------------------

        if (displayX >= ECG_SAMPLES)
        {
            displayX = 0;


            // Clear display data so the new sweep
            // starts on a clean screen.

            for (
                uint8_t i = 0;
                i < ECG_SAMPLES;
                i++
            )
            {
                displayBuffer[i] = 128;
            }
        }


        displaySampleCounter = 0;

        displaySampleSum = 0;
    }
}


// ======================================================
// CLASSIFY BASIC RHYTHM
// ======================================================

void classifyRhythm()
{
    if (heartRate < 60)
    {
        rhythmText = "BRADY";
    }
    else if (heartRate < 100)
    {
        rhythmText = "NORMAL";
    }
    else
    {
        rhythmText = "TACHY";
    }
}


// ======================================================
// ARRHYTHMIA ANALYSIS
// ======================================================

void analyzeRR(uint16_t currentRR)
{
    // ----------------------------------------------
    // First RR
    // ----------------------------------------------

    if (rrCount == 0)
    {
        rrHistory[0] = currentRR;

        rrCount = 1;

        arrhythmiaDetected = false;

        irregularCount = 0;

        return;
    }


    // ----------------------------------------------
    // Previous RR
    // ----------------------------------------------

    uint16_t previousRR =
        rrHistory[rrCount - 1];


    uint16_t difference;


    if (currentRR > previousRR)
    {
        difference =
            currentRR - previousRR;
    }
    else
    {
        difference =
            previousRR - currentRR;
    }


    // ----------------------------------------------
    // 20% threshold
    // ----------------------------------------------

    uint16_t threshold =
        previousRR / 5;


    bool irregular =
        difference > threshold;


    // ----------------------------------------------
    // Require repeated irregularity
    // ----------------------------------------------

    if (irregular)
    {
        if (irregularCount < 10)
        {
            irregularCount++;
        }
    }
    else
    {
        if (irregularCount > 0)
        {
            irregularCount--;
        }
    }


    // ----------------------------------------------
    // ARRHYTHMIA decision
    // ----------------------------------------------

    if (irregularCount >= 2)
    {
        arrhythmiaDetected = true;
    }
    else if (irregularCount == 0)
    {
        arrhythmiaDetected = false;
    }


    // ----------------------------------------------
    // Store RR
    // ----------------------------------------------

    if (rrCount < 4)
    {
        rrHistory[rrCount] =
            currentRR;

        rrCount++;
    }
    else
    {
        rrHistory[0] =
            rrHistory[1];

        rrHistory[1] =
            rrHistory[2];

        rrHistory[2] =
            rrHistory[3];

        rrHistory[3] =
            currentRR;
    }
}


// ======================================================
// QRS DETECTION
// ======================================================

void processQRS(uint8_t sample)
{
    sampleCounter++;


    int16_t currentSample =
        sample;


    int16_t derivative =
        currentSample
        -
        previousSample;


    previousSample =
        currentSample;


    uint16_t slope =
        abs(derivative);


    // ----------------------------------------------
    // Adaptive signal level
    // ----------------------------------------------

    if (slope > signalLevel)
    {
        signalLevel +=
            (slope - signalLevel) >> 3;
    }
    else
    {
        signalLevel -=
            (signalLevel - slope) >> 5;
    }


    // ----------------------------------------------
    // Refractory
    // ----------------------------------------------

    if (refractoryCounter > 0)
    {
        refractoryCounter--;
    }


    bool strongSlope =
        slope > 12;


    bool slopePeak =
        derivative > previousDerivative;


    previousDerivative =
        derivative;


    // ----------------------------------------------
    // QRS detected
    // ----------------------------------------------

    if (
        strongSlope &&
        slopePeak &&
        refractoryCounter == 0 &&
        signalLevel > 8 &&
        sampleCounter > 20
    )
    {
        qrsDetected = true;

        qrsIndicator = 8;


        // ------------------------------------------
        // Refractory = approximately 400 ms
        // ------------------------------------------

        refractoryCounter = 51;


        // ------------------------------------------
        // RR
        // ------------------------------------------

        if (havePreviousQRS)
        {
            rrSamples =
                sampleCounter
                -
                lastQrsSample;


            if (
                rrSamples >= 40 &&
                rrSamples <= 256
            )
            {
                // ----------------------------------
                // HR
                // ----------------------------------

                heartRate =
                    (60UL * FS)
                    /
                    rrSamples;


                // ----------------------------------
                // RR text
                // ----------------------------------

                uint16_t rr100 =
                    (rrSamples * 100UL)
                    /
                    FS;


                uint16_t rrSeconds =
                    rr100 / 100;


                uint16_t rrFraction =
                    rr100 % 100;


                snprintf(
                    rrText,
                    sizeof(rrText),
                    "RR:%u.%02us",
                    rrSeconds,
                    rrFraction
                );


                // ----------------------------------
                // HR text
                // ----------------------------------

                snprintf(
                    hrText,
                    sizeof(hrText),
                    "HR:%u",
                    heartRate
                );


                // ----------------------------------
                // Basic rhythm
                // ----------------------------------

                classifyRhythm();


                // ----------------------------------
                // RR variability
                // ----------------------------------

                analyzeRR(
                    rrSamples
                );


                // ----------------------------------
                // Serial output
                // ----------------------------------

                Serial.print("QRS  ");

                Serial.print(
                    rrText
                );

                Serial.print("  ");

                Serial.print(
                    hrText
                );

                Serial.print("  ");

                Serial.print(
                    rhythmText
                );

                Serial.print("  ARR=");

                if (arrhythmiaDetected)
                {
                    Serial.println("YES");
                }
                else
                {
                    Serial.println("NO");
                }
            }
        }
        else
        {
            strcpy(
                rrText,
                "RR:---"
            );

            strcpy(
                hrText,
                "HR:---"
            );

            rhythmText = "---";

            havePreviousQRS = true;
        }


        lastQrsSample =
            sampleCounter;
    }
}


// ======================================================
// DRAW ECG SWEEP
// ======================================================

void drawECG()
{
    display.firstPage();


    do
    {
        display.setFont(
            u8g_font_5x7
        );


        // ------------------------------------------
        // QRS
        // ------------------------------------------

        display.drawStr(
            1,
            8,
            "QRS:"
        );


        if (qrsIndicator > 0)
        {
            display.drawDisc(
                21,
                5,
                2
            );
        }


        // ------------------------------------------
        // RR / HR
        // ------------------------------------------

        display.drawStr(
            30,
            8,
            rrText
        );


        display.drawStr(
            76,
            8,
            hrText
        );


        // ------------------------------------------
        // ECG frame
        // ------------------------------------------

        display.drawFrame(
            0,
            GRAPH_TOP,
            128,
            GRAPH_BOTTOM - GRAPH_TOP + 1
        );


        // ------------------------------------------
        // Draw waveform
        // ------------------------------------------

        for (
            uint8_t i = 1;
            i < ECG_SAMPLES;
            i++
        )
        {
            int y1 = map(
                displayBuffer[i - 1],
                0,
                255,
                GRAPH_BOTTOM - 1,
                GRAPH_TOP + 1
            );


            int y2 = map(
                displayBuffer[i],
                0,
                255,
                GRAPH_BOTTOM - 1,
                GRAPH_TOP + 1
            );


            y1 = constrain(
                y1,
                GRAPH_TOP + 1,
                GRAPH_BOTTOM - 1
            );


            y2 = constrain(
                y2,
                GRAPH_TOP + 1,
                GRAPH_BOTTOM - 1
            );


            display.drawLine(
                i - 1,
                y1,
                i,
                y2
            );
        }


        // ------------------------------------------
        // Sweep cursor
        // ------------------------------------------

        uint8_t cursorX = displayX;


        display.drawVLine(
            cursorX,
            GRAPH_TOP + 1,
            GRAPH_BOTTOM - GRAPH_TOP - 1
        );


        // ------------------------------------------
        // Bottom status
        // ------------------------------------------

        if (arrhythmiaDetected)
        {
            display.drawStr(
                2,
                63,
                "ARRHYTHMIA"
            );
        }
        else
        {
            display.drawStr(
                2,
                63,
                rhythmText
            );
        }

    }
    while (
        display.nextPage()
    );
}


// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(
        SERIAL_BAUD
    );


    // ----------------------------------------------
    // Display buffer
    // ----------------------------------------------

    resetDisplaySweep();


    // ----------------------------------------------
    // Text
    // ----------------------------------------------

    strcpy(
        rrText,
        "RR:---"
    );


    strcpy(
        hrText,
        "HR:---"
    );


    // ----------------------------------------------
    // GLCD
    // ----------------------------------------------

    display.setContrast(
        60
    );


    drawECG();
}


// ======================================================
// LOOP
// ======================================================

void loop()
{
    // ==================================================
    // RECEIVE ECG
    // ==================================================

    while (
        Serial.available() > 0
    )
    {
        uint8_t sample =
            (uint8_t)Serial.read();


        // ----------------------------------------------
        // IMPORTANT:
        //
        // QRS gets EVERY sample at 128 Hz
        // ----------------------------------------------

        processQRS(
            sample
        );


        // ----------------------------------------------
        // Display gets its own time scale
        // ----------------------------------------------

        addDisplaySample(
            sample
        );
    }


    // ==================================================
    // QRS INDICATOR
    // ==================================================

    if (qrsIndicator > 0)
    {
        qrsIndicator--;
    }


    // ==================================================
    // GLCD REFRESH
    // ==================================================

    static unsigned long lastDisplay = 0;


    if (
        millis() - lastDisplay >= 40
    )
    {
        lastDisplay =
            millis();


        drawECG();
    }
}