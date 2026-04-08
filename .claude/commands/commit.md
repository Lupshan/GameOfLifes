---
description: Stage and commit current changes with a Conventional Commits message
---

Create a git commit following the project's Conventional Commits convention.

Steps:
1. Run in parallel: `git status`, `git diff`, `git diff --staged`, `git log --oneline -10`.
2. Analyze ALL changes (staged + unstaged) and group them logically. If changes span unrelated concerns, ask the user whether to split into multiple commits.
3. Determine the type: `feat`, `fix`, `chore`, `docs`, `refactor`, `test`, `ci`, `build`, `perf`.
4. Determine the scope from the touched area: `engine`, `bytecode`, `backend`, `frontend`, `docs`, `ci`, `justfile`, or a submodule name.
5. Write a concise message: `<type>(<scope>): <imperative summary>` (≤ 72 chars). Add a body only if the *why* is non-obvious.
6. Stage explicitly by file (never `git add -A`/`.`). Skip any `.env*`, `*.key`, `*.pem`, `secrets/`. If you see one staged or untracked, STOP and warn the user.
7. Commit with a HEREDOC. Do NOT add Co-Authored-By unless the user asks.
8. Run `git status` to confirm.

Never push. Never amend unless explicitly asked. Never use `--no-verify`.
