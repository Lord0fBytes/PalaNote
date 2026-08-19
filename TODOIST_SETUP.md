# Todoist sync setup

Pala Note can create a Todoist Inbox task for each transcribed voice note when
the device's **Sync** action runs.

## Configure the token

1. In Todoist, open **Settings -> Integrations -> Developer** and copy the
   personal API token.
2. Manually add this definition to your local `secrets.h` file:

   ```cpp
   #define TODOIST_API_KEY "paste-your-personal-api-token-here"
   ```

The local secrets file is ignored by Git. Do not place the real token in
`secrets.example.h` or any committed source file.

## Configure labels

Todoist labels are controlled by two settings in `config.h`:

```cpp
#define TODOIST_LABELS_ENABLED 1
#define TODOIST_LABEL_PREFIX   "pala"
```

Set `TODOIST_LABELS_ENABLED` to `0` to sync tasks without labels. Change
`TODOIST_LABEL_PREFIX` to use another label namespace. The separator is added
automatically, so a prefix of `pala` and a tag of `TODO` produce `pala/TODO`.

## Behavior

- Sync first transcribes any pending recordings through OpenAI.
- Each transcribed note that has not reached Todoist becomes an Inbox task.
- The transcript's opening text is used as the task title.
- The full transcript, Pala Note number, tag, and recording time are included
  in the task description.
- When enabled, the note's current tag is also added as a Todoist label using
  the configured prefix, such as `pala/TODO` or `pala/Idea`. Local tags remain
  unchanged.
- After Todoist confirms the task, the device writes a local `.todoist` marker
  beside the note. Future syncs skip notes with this marker.
- If a request fails, no marker is written and the note is retried during the
  next Sync.
- Deleting the note on Pala Note removes its local sync marker but does not
  delete the task from Todoist.

Tasks go to the Todoist Inbox by default, so no project ID is required.
