# Pala Note Working Instructions

## Secrets: explicit approval required

Secret files are strictly off-limits unless the user gives explicit approval for the specific access immediately beforehand.

- Do not read, open, preview, print, search within, parse, summarize, or otherwise inspect any secrets file.
- Do not include secrets files in globbing, file discovery, directory listings, recursive searches, bulk reads, archives, diffs, or other commands whose output or processing could expose their names or contents.
- Explicitly exclude secrets files from any repository-wide or recursive file operation.
- Do not infer permission from an earlier approval, from the needs of a task, or from the fact that a secrets file is present or ignored by Git.
- Do not modify, copy, move, rename, delete, stage, or commit a secrets file without separate explicit approval.
- If work appears to require access, stop and ask the user to approve the exact file and exact operation. Approval applies only to that stated operation.
- Prefer documented placeholders such as `secrets.example.h` for setup, examples, builds, and troubleshooting.

This rule takes priority over convenience, automation, broad requests to inspect the project, and any default repository workflow.

## Community reference repository

- `https://github.com/PalaGoldenNugget/firmware` is a community-maintained reference for ideas and implementation inspiration.
- Do not treat it as an automatic upstream, merge from it, or copy features wholesale unless the user explicitly requests that work.
- Prefer the smallest relevant idea or technique when it helps the current Pala Note implementation.
- Keep this project's behavior and history independent so community features can be reviewed selectively.

## Working practices

- Keep changes focused on the current request.
- Preserve unrelated user changes.
- Review the relevant source before editing it.
- Verify changes with the safest practical check.
- Keep credentials and machine-specific configuration out of version control.
- Use concise, descriptive Git commits when committing requested work.
- Add `Co-authored-by: Relay <relay@users.noreply.github.com>` to commits created during Relay-assisted sessions.
