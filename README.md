# Pala Note

Personal firmware workspace for the Pala Note ESP32-S3 e-paper voice recorder.
The device records voice notes to an SD card, transcribes them through OpenAI,
and provides an on-device note browser and local transfer portal.

Firmware version on this branch: **v1.1.2 (unreleased)**

For setup, controls, recording, Sync, Transfer, storage, and troubleshooting,
see the complete [Pala Note device guide](docs/DEVICE_GUIDE.md). Todoist-specific
configuration is documented in [Todoist sync setup](TODOIST_SETUP.md).

## Features

- Two-button voice-note recording workflow
- 200 x 200 e-paper interface
- Local WAV, transcript, metadata, and tag storage on SD card
- OpenAI Whisper transcription during Sync
- Todoist Inbox task delivery with `pala/` labels for newly transcribed notes
- Local browser portal for browsing, filtering, downloading, and deleting notes
- Audio playback, battery monitoring, RTC timestamps, and deep sleep
- Detailed Serial Monitor diagnostics for Wi-Fi, transcription, and Todoist sync

## Repository layout

- `firmware/pala_note/` - ESP32-S3 Arduino firmware
- `case V1.0/` - printable enclosure files and STEP assembly
- `TODOIST_SETUP.md` - Todoist integration setup and behavior
- `AGENTS.md` - project working and credential-safety instructions

The purchased build guide is intentionally kept local and is not redistributed
through this repository.

## Arduino setup

1. Install **esp32 by Espressif Systems** through Arduino Boards Manager.
2. Open `firmware/pala_note/pala_note.ino`.
3. Select **ESP32S3 Dev Module**.
4. Set **USB CDC On Boot** to **Enabled** for Serial Monitor output.
5. Use a Serial Monitor speed of **115200 baud**.
6. Verify and upload the sketch.

The exact library and partition settings may depend on the original Pala Note
build guide and your installed ESP32 Arduino core.

## Local configuration

Copy the definitions shown in `secrets.example.h` into your own local
configuration and provide the required Wi-Fi and API credentials. The real
configuration file is ignored by Git and must never be committed.

For Todoist setup, see [Todoist sync setup](TODOIST_SETUP.md). For the complete
first-boot and device workflow, see the [device guide](docs/DEVICE_GUIDE.md).

## Sync behavior

When Sync is selected, the firmware:

1. Connects to Wi-Fi and updates the clock.
2. Uploads pending WAV files to OpenAI for transcription.
3. Stores returned transcripts locally on the SD card.
4. Creates Todoist Inbox tasks for transcripts not previously delivered.
5. Writes a local marker after Todoist confirms each task.

## Development

The `main` branch contains the current tested release, `v1.1.1`. The
`hotfix-v1.1.2` branch collects device-tested fixes for the next patch release.
Release `v1.1.1` added a
one-time migration for updated default tags while preserving custom tags.
Release `v1.1.0` added Todoist delivery, expanded sync diagnostics,
Pacific-time configuration, and version display on the Settings and deep-sleep
screens.

The community-maintained
[Golden Nugget firmware](https://github.com/PalaGoldenNugget/firmware) may be
used for selective inspiration, but it is not an automatic upstream for this
repository.

## Security notes

- Never commit Wi-Fi passwords or API tokens.
- API requests currently use an insecure TLS client mode; certificate
  validation should be hardened before treating the firmware as production
  software.
- OpenAI and Todoist usage may incur account charges.

## Attribution and licensing

Pala Note and the original project materials are by Paul Lagier. This repository
contains a personal working copy with local modifications. Vendor-derived,
third-party, firmware, and hardware files may retain their original terms.
Confirm those terms before redistributing or manufacturing from this repository.
