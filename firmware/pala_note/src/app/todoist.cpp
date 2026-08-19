#include "Arduino.h"
#include "HTTPClient.h"
#include "WiFiClientSecure.h"
#include "SD_MMC.h"
#include "esp_system.h"
#include "../../config.h"
#include "../../globals.h"
#include "../../secrets.h"
#include "notes.h"
#include "todoist.h"

static String jsonEscape(const String& input) {
  String out;
  out.reserve(input.length() + 24);
  for (size_t i = 0; i < input.length(); i++) {
    const char c = input[i];
    switch (c) {
      case '\\': out += "\\\\"; break;
      case '"':  out += "\\\""; break;
      case '\n': out += "\\n";  break;
      case '\r': out += "\\r";  break;
      case '\t': out += "\\t";  break;
      default:
        if ((uint8_t)c >= 0x20) out += c;
        break;
    }
  }
  return out;
}

static String formEncode(const String& input) {
  static const char HEX_DIGITS[] = "0123456789ABCDEF";
  String out;
  out.reserve(input.length() * 2);
  for (size_t i = 0; i < input.length(); i++) {
    const uint8_t c = (uint8_t)input[i];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      out += (char)c;
    } else {
      out += '%';
      out += HEX_DIGITS[c >> 4];
      out += HEX_DIGITS[c & 0x0F];
    }
  }
  return out;
}

static String notePath(int noteNum, const char* extension) {
  char path[64];
  snprintf(path, sizeof(path), "%s/note_%03d.%s", NOTES_DIR, noteNum, extension);
  return String(path);
}

static String readTranscript(int noteNum) {
  File file = SD_MMC.open(notePath(noteNum, "txt").c_str());
  if (!file) return "";
  String text;
  while (file.available() && text.length() < 4000) text += (char)file.read();
  file.close();
  text.trim();
  return text;
}

static String commandUuid(int noteNum) {
  const uint64_t mac = ESP.getEfuseMac();
  char uuid[37];
  snprintf(uuid, sizeof(uuid), "%08lx-%04x-4%03x-8%03x-%012llx",
           (unsigned long)(mac >> 16),
           (unsigned int)(mac & 0xFFFF),
           (unsigned int)(noteNum & 0x0FFF),
           (unsigned int)((noteNum >> 12) & 0x0FFF),
           (unsigned long long)(((mac << 16) ^ (uint64_t)noteNum) & 0xFFFFFFFFFFFFULL));
  return String(uuid);
}

static bool createTodoistTask(const NoteEntry& note) {
#ifndef TODOIST_API_KEY
  Serial.println("[Todoist] TODOIST_API_KEY is not configured");
  return false;
#else
  Serial.printf("[Todoist] note #%d: preparing task\n", note.num);
  String transcript = readTranscript(note.num);
  if (transcript.length() == 0) {
    Serial.printf("[Todoist] note #%d: transcript is missing or empty\n", note.num);
    return false;
  }

  String title = transcript;
  title.replace("\n", " ");
  title.replace("\r", " ");
  if (title.length() > 240) title = title.substring(0, 237) + "...";

  String description = "Pala Note #" + String(note.num);
  if (strlen(note.tag) > 0) description += "\nTag: " + String(note.tag);
  String created = noteCreatedUtc(note.num);
  if (created.length() > 0) description += "\nRecorded: " + created;
  description += "\n\n" + transcript;

  String labelsArg;
#if TODOIST_LABELS_ENABLED
  if (strlen(note.tag) > 0) {
    String todoistLabel = String(TODOIST_LABEL_PREFIX);
    todoistLabel.trim();
    if (todoistLabel.length() > 0 && !todoistLabel.endsWith("/")) todoistLabel += "/";
    todoistLabel += String(note.tag);
    labelsArg = ",\"labels\":[\"" + jsonEscape(todoistLabel) + "\"]";
  }
#endif

  const String uuid = commandUuid(note.num);
  String command = "[{\"type\":\"item_add\",\"temp_id\":\"" + uuid +
                   "\",\"uuid\":\"" + uuid + "\",\"args\":{\"content\":\"" +
                   jsonEscape(title) + "\",\"description\":\"" +
                   jsonEscape(description) + "\"" + labelsArg + "}}]";

  WiFiClientSecure client;
  client.setInsecure();  // TODO: install or pin Todoist's CA certificate.
  HTTPClient http;
  if (!http.begin(client, "https://api.todoist.com/api/v1/sync")) {
    Serial.printf("[Todoist] note #%d: could not initialize HTTPS request\n", note.num);
    return false;
  }
  http.setTimeout(30000);
  http.addHeader("Authorization", "Bearer " + String(TODOIST_API_KEY));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int status = http.POST("commands=" + formEncode(command));
  String response = http.getString();
  http.end();

  const bool accepted = status == 200 &&
                        response.indexOf("\"" + uuid + "\":\"ok\"") >= 0;
  if (!accepted) {
    Serial.printf("[Todoist] note #%d: rejected or unreadable response (HTTP %d, %u bytes)\n",
                  note.num, status, (unsigned)response.length());
    return false;
  }

  File marker = SD_MMC.open(notePath(note.num, "todoist").c_str(), FILE_WRITE);
  if (!marker) {
    Serial.printf("[Todoist] note #%d: task created but local marker write failed\n", note.num);
    return false;
  }
  marker.println("synced=1");
  marker.close();
  Serial.printf("[Todoist] note #%d: task created and marked synced\n", note.num);
  return true;
#endif
}

void syncTodoistAll() {
#ifndef TODOIST_API_KEY
  Serial.println("[Todoist] skipped; add TODOIST_API_KEY to the local secrets configuration");
#else
  int eligible = 0;
  int skipped = 0;
  int succeeded = 0;
  int failed = 0;
  for (int i = 0; i < (int)noteIndex.size(); i++) {
    const NoteEntry& note = noteIndex[i];
    if (!note.hasText) continue;
    if (SD_MMC.exists(notePath(note.num, "todoist").c_str())) skipped++;
    else eligible++;
  }
  Serial.printf("[Todoist] pass starting: %d eligible, %d already synced\n", eligible, skipped);
  for (int i = 0; i < (int)noteIndex.size(); i++) {
    const NoteEntry& note = noteIndex[i];
    if (!note.hasText) continue;
    if (SD_MMC.exists(notePath(note.num, "todoist").c_str())) continue;
    if (createTodoistTask(note)) succeeded++;
    else failed++;
  }
  Serial.printf("[Todoist] pass complete: %d succeeded, %d failed, %d previously synced\n",
                succeeded, failed, skipped);
#endif
}
