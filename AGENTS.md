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
- When starting work for a new release, immediately bump the firmware version shown on-device to the target release version so test builds can be identified on the hardware.
- Add `Co-authored-by: Relay <relay@users.noreply.github.com>` to commits created during Relay-assisted sessions.

## GitHub issue conventions

- Prefix every new issue title with a lowercase type in brackets: `[bug]` for incorrect existing behavior or `[feat]` for new or expanded behavior.
- Follow the prefix with a short, sentence-style description, for example `[bug] Sleep wake up menu` or `[feat] Add optional unattended daily sync`.
- Apply the `bug` label to bug issues.
- Apply the `minor` label to small, localized feature work and the `major` label to substantial or cross-cutting feature work.
- Use only the label that best matches the issue under this scheme unless the user explicitly requests additional labels.
- Assign the issue to its target release milestone when that release is known; otherwise leave the milestone unset.
- Include enough context, expected behavior, implementation notes, and acceptance criteria for the issue to be actionable without changing the user's intent.

## Working on a GitHub issue

When the user says `work on issue #N`, follow this checklist.

### Review and scope

- Read the GitHub issue, including its title, body, labels, milestone, comments, and current state.
- Identify dependencies and related issues.
- Confirm the expected behavior is sufficiently clear; ask before implementing when ambiguity would materially change the result.
- Keep the work limited to the issue's stated intent.

### Protect the workspace

- Follow the secrets restrictions above for every operation.
- Check the current branch and working-tree status, and preserve unrelated user changes.
- Do not merge from the community reference repository unless explicitly requested.

### Select the working branch

- Use the existing branch for the issue's milestone when one exists, such as `hotfix-v1.1.2`.
- Otherwise create a focused branch using the issue type and number.
- When beginning a new release branch, immediately bump the on-device firmware version.
- Do not merge, tag, or release merely because implementation is complete.

### Understand and implement

- Trace the relevant code before editing it, including affected controls, UI states, storage, and network behavior.
- Check whether persisted SD-card state contributes to the reported behavior.
- Summarize the cause and proposed change before making a substantial modification.
- Implement the smallest complete change that satisfies the acceptance criteria.
- Preserve behavior outside the issue's scope and add useful Serial diagnostics for hardware, network, storage, or background failures.

### Verify and commit

- Check formatting, malformed diffs, stale references, and unintended changes.
- Compile when an appropriate local toolchain is available; otherwise clearly hand off Arduino compilation and hardware testing to the user.
- Confirm that no sensitive configuration was staged or exposed.
- Create a concise commit and include the required Relay co-author trailer.
- Update the issue with the implementation commit and mark it as awaiting device verification.
- Do not claim the issue is fixed until the user confirms the on-device test.

### Device-test handoff

- Tell the user which branch or sketch to upload, the expected result, regression checks, and any Serial output to watch.
- Do not merge or release while waiting for the test result.

### After successful testing

- Add an issue comment recording the successful device test and planned release.
- Keep the issue open until the release is actually merged and published.
- Continue collecting requested fixes on the milestone branch.

### Release only when explicitly requested

- Review all issues assigned to the milestone and confirm the included changes were tested.
- Perform a final sensitive-data and repository sanity check.
- Update release notes and remove unreleased wording.
- Merge the release branch into `main`, create and push the version tag, and push `main`.
- Close released issues, update the milestone, and report the merge commit, tag, and GitHub links.
