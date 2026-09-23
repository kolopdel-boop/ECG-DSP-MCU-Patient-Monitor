import serial
import time
from pathlib import Path


# ======================================================
# SERIAL SETTINGS
# ======================================================

PORT = "COM11"
BAUD = 115200

FS = 128
SAMPLE_PERIOD = 1.0 / FS

DURATION_SECONDS = 10
EXPECTED_SAMPLES = FS * DURATION_SECONDS


# ======================================================
# ECG FILES
# ======================================================

ECG_FILES = [
    (
        "NORMAL",
        Path(r"C:\ECG_DSP_MCU\ecg_normal_10s.txt")
    ),

    (
        "BRADYCARDIA",
        Path(r"C:\ECG_DSP_MCU\ecg_brady_10s.txt")
    ),

    (
        "TACHYCARDIA",
        Path(r"C:\ECG_DSP_MCU\ecg_tachy_10s.txt")
    )
]


# ======================================================
# LOAD ECG
# ======================================================

def load_ecg(filename):

    print()
    print("Loading ECG:")
    print(filename)

    if not filename.exists():
        raise FileNotFoundError(
            f"\nECG file not found:\n{filename}"
        )

    values = []

    with open(filename, "r") as file:

        for line in file:

            line = line.strip()

            if not line:
                continue

            try:
                values.append(float(line))

            except ValueError:
                print(
                    "Invalid value skipped:",
                    line
                )

    if len(values) == 0:
        raise ValueError(
            "No ECG samples found."
        )

    print("Samples:", len(values))
    print("Minimum:", min(values))
    print("Maximum:", max(values))

    return values


# ======================================================
# NORMALIZE ECG
# ======================================================

def normalize_ecg(values):

    minimum = min(values)
    maximum = max(values)

    print()
    print("Normalization")
    print("----------------------------------------")
    print("Minimum:", minimum)
    print("Maximum:", maximum)

    if maximum == minimum:

        print(
            "WARNING: ECG has no amplitude variation."
        )

        return bytearray(
            [128] * len(values)
        )


    result = bytearray()

    for value in values:

        normalized = (
            (value - minimum)
            /
            (maximum - minimum)
        )

        sample = int(
            normalized * 255
        )

        sample = max(
            0,
            min(255, sample)
        )

        result.append(sample)


    print("Normalized range: 0 ... 255")

    return result


# ======================================================
# PREPARE ECG
# ======================================================

def prepare_ecg(filename):

    values = load_ecg(filename)


    if len(values) < EXPECTED_SAMPLES:

        print()
        print(
            "WARNING: file contains only",
            len(values),
            "samples."
        )

        print(
            "Repeating ECG to reach",
            EXPECTED_SAMPLES,
            "samples."
        )

        repeated = []

        while len(repeated) < EXPECTED_SAMPLES:

            repeated.extend(values)

        values = repeated[:EXPECTED_SAMPLES]


    elif len(values) > EXPECTED_SAMPLES:

        print()
        print(
            "Using first",
            EXPECTED_SAMPLES,
            "samples."
        )

        values = values[:EXPECTED_SAMPLES]


    return normalize_ecg(values)


# ======================================================
# SEND ONE ECG
# ======================================================

def send_ecg(
    ser,
    samples,
    label
):

    print()
    print("========================================")
    print("START:", label)
    print("========================================")

    print(
        "Samples:",
        len(samples)
    )

    print(
        "FS:",
        FS
    )

    print(
        "Duration:",
        len(samples) / FS,
        "seconds"
    )

    print()


    start_time = time.perf_counter()


    for index, sample in enumerate(samples):

        ser.write(
            bytes([sample])
        )


        target_time = (
            start_time
            +
            (index + 1)
            *
            SAMPLE_PERIOD
        )


        while True:

            remaining = (
                target_time
                -
                time.perf_counter()
            )

            if remaining <= 0:
                break


            if remaining > 0.001:

                time.sleep(
                    remaining - 0.0005
                )


    elapsed = (
        time.perf_counter()
        -
        start_time
    )


    actual_fs = (
        len(samples)
        /
        elapsed
    )


    print()
    print("----------------------------------------")
    print("Transmission complete:", label)
    print("Elapsed:", round(elapsed, 3), "seconds")
    print(
        "Actual FS:",
        round(actual_fs, 2),
        "Hz"
    )
    print("----------------------------------------")


# ======================================================
# MAIN
# ======================================================

def main():

    print()
    print("========================================")
    print(" THREE ECG TEST TRANSMITTER")
    print("========================================")

    print("Port:", PORT)
    print("Baud:", BAUD)
    print("Sampling rate:", FS)
    print("Duration per ECG:", DURATION_SECONDS)
    print("========================================")


    # --------------------------------------------------
    # Open serial
    # --------------------------------------------------

    print()
    print("Opening", PORT)

    ser = serial.Serial(
        PORT,
        BAUD,
        timeout=1
    )


    time.sleep(2)

    print("Connected.")


    try:

        # ==============================================
        # SEND ALL THREE ECGs
        # ==============================================

        for label, filename in ECG_FILES:

            print()
            print()
            print("########################################")
            print("#", label)
            print("########################################")


            samples = prepare_ecg(
                filename
            )


            send_ecg(
                ser,
                samples,
                label
            )


            # ------------------------------------------
            # Pause between ECGs
            # ------------------------------------------

            print()
            print(
                "Waiting 2 seconds before next ECG..."
            )

            time.sleep(2)


        # ==============================================
        # FINISHED
        # ==============================================

        print()
        print()
        print("========================================")
        print("ALL THREE ECG TESTS COMPLETE")
        print("========================================")


    except KeyboardInterrupt:

        print()
        print(
            "Transmission stopped by user."
        )


    finally:

        ser.close()

        print()
        print(
            "Serial closed:",
            PORT
        )


# ======================================================
# RUN
# ======================================================

if __name__ == "__main__":
    main()