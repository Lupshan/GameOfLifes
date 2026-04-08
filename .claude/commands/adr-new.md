---
description: Create a new numbered ADR in docs/adr/
---

Create a new ADR file in `docs/adr/`.

Steps:
1. List existing ADRs with Glob `docs/adr/*.md` to find the next number (zero-padded to 3 digits).
2. Ask the user for the title if not provided in `$ARGUMENTS`, otherwise use `$ARGUMENTS` as the title.
3. Slugify the title (kebab-case, lowercase, ASCII).
4. Create `docs/adr/NNN-<slug>.md` with this template:

```markdown
# NNN — <Title>

- Status: Proposed
- Date: <today's date YYYY-MM-DD>

## Context

<what situation triggers this decision>

## Decision

<the chosen approach>

## Consequences

<positive, negative, neutral consequences>

## Alternatives considered

<other options and why rejected>
```

5. Report the new file path to the user. Do NOT commit.
