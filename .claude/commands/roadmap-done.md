---
description: Verify a roadmap step's Definition of Done and mark it complete
---

Mark a roadmap step as done. Argument: step slug or number (`$ARGUMENTS`).

Steps:
1. Locate the step file in `docs/roadmap/` matching the argument (Glob `docs/roadmap/*$ARGUMENTS*.md`). If ambiguous, ask the user.
2. Read the file and locate the **Definition of Done** checklist.
3. For EACH unchecked item, verify it is genuinely satisfied. Do not tick items you have not actually verified — read the relevant code/tests/files. If a check cannot be verified, report it and STOP.
4. Run the relevant test suites for the brick(s) the step touched (`just test-engine`, `just test-bytecode`, `just test-backend`, or `just test`). All must be green.
5. If everything passes: tick all DoD checkboxes in the file and append a `## Completion` section with the date and the latest commit SHA (`git rev-parse HEAD`).
6. Update `docs/roadmap/README.md` to reflect the step as done if it tracks status.
7. Report a short summary. Do NOT auto-commit unless the user asks.

If anything fails: STOP, do not modify the file, and report what's missing.
