# Pala Note device guide

**Version: 1.1.2**

This is a current-state guide. Features discussed under
[Known limitations](#known-limitations) are planned or incomplete and should
not be mistaken for available behavior.

## Contents

- [Getting started](#getting-started)
- [Controls and navigation](#controls-and-navigation)
- [Record and tag a note](#record-and-tag-a-note)
- [Browse and manage notes](#browse-and-manage-notes)
- [Manual Sync](#manual-sync)
- [Transfer mode](#transfer-mode)
- [Settings and device information](#settings-and-device-information)
- [Power, time, and storage](#power-time-and-storage)
- [Troubleshooting](#troubleshooting)
- [Known limitations](#known-limitations)

## Getting started

### What you need

- An assembled Pala Note with its ESP32-S3, 1.54-inch 200 x 200 e-paper
  display, two buttons, microphone, speaker, RTC, battery, and SD-card slot.
- A working microSD card installed in the device. The firmware mounts it in
  one-bit SD-MMC mode and stores its data under `/notes`.
- A data-capable USB cable and a computer running Arduino IDE.
- A 2.4 GHz Wi-Fi network for Sync and Transfer. ESP32-S3 Wi-Fi does not join
  5 GHz-only networks.
- An OpenAI API key for transcription. A Todoist API token is optional.

The purchased construction guide and any private hardware material are not
included in this repository. Use those materials for assembly-specific details.

### Configure and upload the firmware

1. Install **esp32 by Espressif Systems** in Arduino Boards Manager.
2. Open `firmware/pala_note/pala_note.ino`.
3. Select **ESP32S3 Dev Module** as the board.
4. Enable **USB CDC On Boot** so startup and Sync diagnostics reach the Serial
   Monitor.
5. Use the library and partition settings required by the hardware build and
   your installed ESP32 Arduino core.
6. Use the documented `secrets.example.h` definitions as a template for a
   local `secrets.h`. Enter your Wi-Fi SSID, Wi-Fi password, and OpenAI API key
   in that ignored local file. Never put real credentials in the example file
   or commit them.
7. If desired, follow [Todoist sync setup](../TODOIST_SETUP.md) to add Todoist.
8. Verify and upload the sketch.

Do not post, share, or commit the local credentials file. This guide does not
require opening it after configuration.

### First boot and Serial Monitor

Open Serial Monitor at **115200 baud**, then start or reset the device. A normal
startup prints a banner containing the firmware version, initializes the
display, audio, RTC, and SD card, creates `/notes` if needed, loads tags and the
note index, and displays the **ready** screen.

On a new card, the firmware creates the default tags `TODO`, `Buy`, `Idea`,
`Private`, and `Work`. Firmware v1.1.2 also performs its one-time default-tag
migration while preserving custom tags.

If the SD card cannot be mounted, the device displays **SD ERR** and remains
there. Correct the card or hardware problem and restart it.

## Controls and navigation

The two controls are vertically arranged on the device's right side:

- **Upper button — REC:** record, select, play, confirm, or go back, depending
  on the screen.
- **Lower button — power/menu:** open or advance menus, move to the next item,
  cancel confirmations, or advance through note text.

The firmware recognizes these REC gestures:

- **Single press:** select the highlighted item. In note detail it plays audio.
- **Long press** (about 600 ms): go back on most menus; in note detail it opens
  deletion confirmation; after recording it opens discard confirmation.
- **Double press** (two presses within about 200 ms): go back on most screens.
  In the note list it moves to the previous note, and in Transfer it exits.

The power/menu button is handled as a single press during normal operation.
Holding it does not provide a separate navigation action. On the ready screen,
pressing it opens the main menu; inside menus it advances to the next item.

### Startup and wake behavior

- On a cold boot, the device opens the **ready** screen after initialization.
- From deep sleep, press the **upper REC button** to wake directly into a new
  recording.
- From deep sleep, press the **lower power/menu button** to wake directly into
  the main menu.
- A wake without either button still held falls back to the ready screen.

E-paper refreshes are slower than phone or computer displays. Wait for a screen
to finish drawing before assuming a press was missed.

## Record and tag a note

1. From **ready**, press and hold REC for about 350 ms. The recording screen is
   black with a white recording dot.
2. Keep REC held while speaking. Recording continues while REC is down and is
   guaranteed to run for at least roughly half a second.
3. Release REC to stop. The device writes a mono, 16-bit, 16 kHz WAV file,
   briefly shows **saved** and its note number, then opens **choose tag**.
4. Press the lower power/menu button to cycle through the available tags.
5. Single-press REC to apply the displayed tag and save the note to the index.
   The current v1.1.2 flow then enters deep sleep.

### Discarding Notes

On **choose tag**, follow the **hold REC: discard** cue. Long-press REC to open
the **DISCARD** screen, then press REC to confirm or the lower power/menu button
to cancel. Confirming permanently removes the recording and returns to ready;
cancelling returns to tag selection.

If recording cannot create or write the WAV, the device displays **REC FAIL**,
returns to ready, and does not add the note to its index.

### Note numbers and files

The next note number is one greater than the highest number in `index.csv`.
Numbers are displayed with three digits, such as `#001`, and filenames use the
same pattern. Deleted highest-numbered notes can therefore allow that number to
be reused; gaps elsewhere are not renumbered.

Saving a tagged recording creates:

- `note_NNN.wav` — original recording.
- `note_NNN.meta` — UTC creation time when available, tag, and transcription
  state.
- An entry in `index.csv` — note number, tag, and whether text exists.

Sync may later add:

- `note_NNN.txt` — OpenAI transcript.
- `note_NNN.todoist` — marker written only after Todoist accepts the task.

## Browse and manage notes

### Browse every note

1. From ready, press the lower power/menu button.
2. Leave **Notes** highlighted and single-press REC.
3. Press the lower button to move to the next note. Double-press the lower
   button to move to the previous note.
4. Single-press REC to open the highlighted note.
5. Long-press or double-press REC to return to the main menu.

The list shows newest notes first, three per page. Each card shows its number,
tag, recorded time when known, and transcript preview or **not synced**. A long
active preview scrolls automatically.

### Filter by tag

1. Open the main menu and move to **Tags** with the lower button.
2. Single-press REC.
3. Cycle through tags with the lower button. The screen shows how many notes
   use each tag.
4. Single-press REC to open that tag's filtered note list.
5. Long-press or double-press REC to return to the main menu.

### Read, scroll, and play

In note detail, a transcript is shown seven lines at a time. If no transcript
exists, the screen says **not synced**.

- Press the lower button to advance to the next text page. After the last page,
  it returns to page one of the next note.
- Single-press REC to play the WAV through the speaker. Press REC during
  playback to stop it.
- Double-press REC to return to the note list.

### Delete a note

1. Open the note detail screen.
2. Long-press REC to open **DELETE**.
3. Single-press REC to confirm, or press the lower button to cancel. A double
   or long REC press also cancels.

Deletion permanently removes the note's WAV, transcript, metadata, and local
Todoist marker and removes it from the index. It does not delete any task
already created in Todoist. Back up important files before deleting them.

## Manual Sync

Sync is manual in v1.1.2:

1. Open the main menu, move to **Sync**, and single-press REC.
2. The device attempts Wi-Fi connection up to 20 times at half-second intervals.
3. After connecting, it requests UTC time from internet time servers and writes
   it to the hardware RTC when successful.
4. Each indexed note without a transcript is uploaded to OpenAI using the
   `whisper-1` transcription model. Successful text is saved beside the WAV and
   the index and metadata are marked as transcribed.
5. Each transcribed note without a `.todoist` marker is offered to Todoist, if
   configured. Accepted tasks receive a local marker so later Sync runs skip
   them.
6. Wi-Fi disconnects, the device shows **all done**, and it returns to the
   screen from which Sync was started.

The Todoist task title uses up to the first 240 transcript characters. Its
description includes the Pala Note number, tag, recorded UTC time when known,
and full transcript. Optional labels use the configured prefix, such as
`pala/Idea`. See [Todoist sync setup](../TODOIST_SETUP.md) for configuration.

### Partial and failed Sync behavior

- A Wi-Fi failure stops the Sync before transcription and displays **NO WIFI**.
- Each failed OpenAI upload is retried up to three times with a fixed
  three-second delay. A failed note keeps its WAV and remains pending.
- A successful transcript remains saved even if a later note fails.
- Todoist processes every available transcript independently. A failure writes
  no marker, so that note is retried on the next Sync.
- If Todoist creates a task but the device cannot write its marker, the next
  Sync may create a duplicate task.
- **all done** means the bounded passes ended; consult Serial Monitor to confirm
  whether individual notes failed.

Serial Monitor reports Wi-Fi attempts and IP address, clock update, each upload
and response, transcription totals, Todoist eligible/success/failure totals,
and Wi-Fi shutdown. API credentials are sent only to their configured services;
do not share Serial logs if future diagnostics ever add sensitive content.

## Transfer mode

Transfer starts a web portal on the Pala Note itself. It is available only to
devices that can reach Pala Note on the same local network; it is not a cloud
service and does not provide authentication or HTTPS.

1. Open **Menu > Settings > Transfer** and single-press REC.
2. Wait while the device joins Wi-Fi and starts the portal.
3. Read the numeric IP address displayed under **open browser**.
4. On a phone or computer on the same network, open `http://IP-ADDRESS/`, using
   the exact address on the device.
5. When finished, double-press or long-press REC. The server stops, Wi-Fi turns
   off, and the device returns to Settings.

The portal lists newest notes first and supports:

- Filtering note cards by tag.
- Reading transcripts and playing available WAV audio in the browser.
- Downloading an individual TXT or WAV.
- Downloading one combined text export for all notes or the active tag. The
  on-device export is truncated if it grows beyond roughly 55 KB.
- Permanently deleting a note after a browser confirmation.
- Opening **tags** to add a unique custom tag (maximum 31 characters and 20
  total tags) or delete a tag.

Deleting a tag that is in use preserves its notes and moves them to
`Untagged`. `Untagged` itself cannot be deleted. Tag changes become available
on the device after the portal reloads its stored tag list.

Keep Transfer active only while using it. Anyone who can reach the portal on
the local network can read, download, or delete notes.

## Settings and device information

Open **Menu > Settings**. Press the lower button to cycle and single-press REC
to choose. Long-press or double-press REC to return to the main menu.

- **Sounds:** toggles interface sounds on or off. Recording and playback manage
  the audio path separately. The setting is not persisted across a restart.
- **Transfer:** starts the local portal described above.
- **Device:** shows firmware version, board name, indexed note count, sound
  state, and whether the RTC contains a valid time. Any button returns to
  Settings.

The Settings screen and deep-sleep screen also show the firmware version.

### Battery display

The ready screen draws a circular charge estimate. The firmware samples the
battery every 30 seconds while it is not recording or playing. At 15% or lower,
it shows a 2.5-second low-battery warning once; the warning is armed again after
the estimate rises above 20%. Battery percentage is an approximate voltage
mapping in 5% steps, not a fuel-gauge measurement.

## Power, time, and storage

### Deep sleep

After two minutes without a recognized action, the device shows its sleep
screen and firmware version, turns off Wi-Fi and audio output, and enters deep
sleep. The timeout is suspended during recording and Transfer mode. The upper
REC and lower power/menu buttons are the only configured wake sources; there is
no timer or daily automatic Sync wake in v1.1.2.

### Timezone behavior

The RTC and metadata store UTC. Sync refreshes the clock from internet time
servers. On-device note-list timestamps use a fixed offset of UTC-8 (Pacific
Standard Time). The firmware does not automatically apply Pacific daylight
saving time, so displayed device times can be one hour behind during PDT. The
web portal asks the browser to display stored UTC timestamps in the browser's
local timezone.

Notes created before the RTC has a valid time show **time not set** and may have
an empty creation timestamp. A later Sync sets the RTC but does not reconstruct
missing historical recording times.

### SD-card layout and backup

All user note data is under `/notes`:

```text
/notes/
  index.csv
  tags.txt
  tags.defaults
  note_NNN.wav
  note_NNN.txt
  note_NNN.meta
  note_NNN.todoist
```

Temporary `index.tmp` or `tags.tmp` files may briefly exist during a safe
rewrite. Back up the entire `/notes` directory with the device powered down or
sleeping and the card removed safely. The WAV, TXT, and metadata files contain
the recoverable note content; the index, tag list, defaults revision, and
Todoist markers preserve application state.

The firmware does not rebuild `index.csv` by scanning orphaned WAV or TXT
files. Restoring individual note files without a matching valid index entry may
leave them invisible on the device. Prefer restoring the entire consistent
directory. Because `.todoist` markers prevent repeat delivery, removing them
from a backup can cause tasks to be created again.

## Troubleshooting

### Upload fails or the device behaves incorrectly

- Confirm **ESP32S3 Dev Module** is selected, not a similarly named ESP32 board.
- Confirm the required libraries and board support package are installed.
- Recheck the hardware build's partition and upload settings.
- Use a data-capable USB cable and select the correct serial port.

### Serial Monitor is blank

- Set it to **115200 baud**.
- Enable **USB CDC On Boot**, upload again, then reopen Serial Monitor.
- Reset or wake the device after opening the monitor; deep sleep stops normal
  output until a button wakes it.

### Wi-Fi or Transfer reports NO WIFI

- Verify the configured SSID and password without sharing them.
- Confirm the network offers 2.4 GHz and that the device is in range.
- Avoid captive portals and client-isolated guest networks. Isolation can also
  prevent another device from opening the Transfer IP.
- Retry; both Sync and Transfer use bounded connection attempts.

### OpenAI transcription fails

- Watch `[Whisper]` lines in Serial Monitor for connection failures or an HTTP
  status.
- HTTP 401 generally indicates an invalid or missing API key.
- HTTP 429 can indicate a short-term rate limit or exhausted API quota/billing.
  Firmware v1.1.2 does not distinguish those cases and uses the same fixed
  three-second retry, so check the OpenAI account before retrying Sync.
- Keep the WAV file. A note without a saved transcript remains pending and can
  be retried later.
- The current response parser and memory limits may reject unusually large or
  unexpected responses.

### Todoist delivery fails

- Follow [Todoist sync setup](../TODOIST_SETUP.md) and confirm the token remains
  valid.
- Watch `[Todoist]` lines for the note number, HTTP status, and pass totals.
- A transcript with no `.todoist` marker remains pending. Retry Sync after the
  account or network problem is corrected.
- Do not manually delete markers unless you accept the risk of duplicate tasks.

### SD ERR, REC FAIL, missing notes, or playback failure

- Power down before reseating the SD card and verify it is healthy and writable.
- Back up `/notes` before attempting repair.
- **REC FAIL** can also indicate memory allocation or an audio/SD write failure;
  restart and watch Serial Monitor for `[Rec]` output.
- A WAV shorter than its 44-byte header is not playable. The device also stops
  playback if the file cannot be opened or playback memory cannot be allocated.
- If files exist but notes are missing from the UI, restore a matching
  `index.csv`; the firmware does not automatically discover orphaned files.

## Known limitations

- Sync is manual. Optional unattended daily Sync is planned in
  [issue #3](https://github.com/Lord0fBytes/PalaNote/issues/3) but is not
  available.
- OpenAI retry handling uses three fixed-delay attempts and does not parse
  `Retry-After` or distinguish quota failures. Improvements are tracked in
  [issue #2](https://github.com/Lord0fBytes/PalaNote/issues/2).
- Saving a tag currently puts the device directly to sleep. Returning to ready
  for rapid consecutive notes is tracked in
  [issue #7](https://github.com/Lord0fBytes/PalaNote/issues/7).
- Transfer has no authentication or TLS. OpenAI and Todoist HTTPS clients also
  currently skip certificate validation.
- The on-device timezone is a fixed UTC-8 offset and does not adjust for DST.
- Sound preference does not persist across restarts.
- There is no automatic index reconstruction or undelete function.
- Physical-device verification of this v1.1.2 guide is required before release.
