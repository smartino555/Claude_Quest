# Claude Quest (raylib / C)

An RPG that teaches you Claude, from writing your first good prompt to building agents. There are ten regions. Each one has a mentor who teaches a topic, battles where foes attack with questions, and a boss guarding the road to the next region. Every region builds on the ones before it, and battles keep pulling in review questions from earlier regions.

## Build & run (macOS)

```sh
brew install raylib          # one time
cd C_Games/Learn_Claude
make
./claude_quest
```

The game needs raylib 5.5 or newer. It looks for the `assets/` folder next to the executable. Progress is saved to `save.txt` next to the executable.

`make test` runs the test suite (`tests/test_main.c`, about 17,000 checks). It covers:
- **Curriculum integrity.** Every lesson, question and context note is present, ASCII-only (the font is an ASCII subset) and short enough to fit its box. Each question has 4 distinct options, a valid answer and an explanation, and no question appears twice.
- **Progression.** Lessons unlock abilities, and a boss opens after its lessons plus 2 wins. A boss win unlocks the next region; rematches don't.
- **Battle rules.** Damage, combos and every ability: stacking multipliers, rewind, cache pricing, hook healing and subagent strikes.
- **Question mix.** No repeats inside a battle, about 30% review questions in normal battles and 50% in the final boss, missed questions return more often, and answer positions are shuffled.
- **Save/load.** A save survives a round trip, and garbage files are rejected.
- **Full playthrough.** A simulated player who is right 75% of the time can finish the whole game. Currently that takes about 31 battles of about 6 questions each, ending at level 13.

`make shots` renders every screen to `shots/`, which is handy for checking the layout after a change.

## The ten regions

| # | Region | Topic | Ability you learn |
|---|---|---|---|
| 1 | Promptwood Village | Prompt craft: clarity, context, examples, XML tags, chaining | Clarify: remove two wrong answers |
| 2 | Conversation Citadel | The Claude apps: models, Projects, files, memory, Styles, search, Research | Extended Thinking: next hit deals double damage |
| 3 | Artifact Forge | Artifacts: types, iterating, versions, sharing, AI-powered apps | Version History: restore 35 HP |
| 4 | Connector Crossroads | Connectors, MCP, desktop extensions, Claude in Chrome, prompt injection | Web Search: remove one wrong answer |
| 5 | Terminal Caves | Claude Code basics: modes, @ and !, Esc and /rewind, /clear, /compact, /init | /rewind: undo your next wrong answer |
| 6 | Config Keep | CLAUDE.md, settings scopes, permission rules, slash commands, Skills | Load Skill: +50% damage for 3 hits |
| 7 | Hookfall Rapids | Hooks (events, matchers, exit code 2), headless `-p`, GitHub Actions | PostToolUse Hook: heal after every answer |
| 8 | The Hive | Subagents, MCP in Claude Code, plugins, worktrees, writer/reviewer | Spawn Subagent: an ally strikes for 3 turns |
| 9 | The API Spire | Messages API, stop reasons, tool use, streaming, caching, batches | Prompt Cache: abilities cost 10% |
| 10 | The Agent Summit | Workflows vs agents, Agent SDK, context engineering, tool design, evals | Agent Loop: next 3 hits strike twice |

Each ability works like the feature it is named after, which reinforces the lesson. For example, Prompt Cache makes repeated ability use cost about 10%, just as cache reads do.

## How a region works

1. **Lessons.** The mentor has five lesson pages. Reading all five teaches you the region's ability.
2. **Battles.** Each correct answer hits the foe, and three in a row is a combo. A wrong answer lets the foe hit you, then shows the correct answer and why. Battles in later regions mix in review questions from earlier ones: 30% in normal battles, 40% against bosses and half against the final boss.
3. **Boss.** It opens once you've read the lessons and won 2 battles. Beating it unlocks the next region.
4. **Inn.** Resting restores your HP and tokens. Abilities cost tokens, and each correct answer gives back 3.

**Context.** Stuck on a question? Press C, or the Context button at the bottom of the battle screen, for background on the ideas the question uses: what the terms mean and when you would use them in real work. For example, the question about `<contract>` tags explains what XML tags are for, with a sample prompt. It costs nothing, and it works before or after you answer.

Losing a battle sends you back to the region with half your HP. Questions you miss come back more often until you get them right.

## Codex (J)

- **Lessons:** every lesson page you have read, by region.
- **Mistakes:** every question you have missed, most-missed first, with the correct answer and explanation.
- **Progress:** lessons, wins, bosses, questions seen and accuracy per region.

## Controls

| Key | Action |
|---|---|
| 1 - 4 | Answer |
| Q W E R T Y U I O P | Abilities (one per region, in order) |
| C | Context for the current question |
| Enter / Space | Confirm, continue |
| Arrow keys | Move on the map, turn lesson pages, scroll |
| J | Codex |
| H | Help |
| Esc | Back; in battle, retreat (no XP) |

The mouse works everywhere too.

## Code layout

- `src/content.c`: the whole curriculum (50 lesson pages, 120 questions, foes, abilities). Edit here to add or change material.
- `src/context.c`: the Context note for each question. `CONTEXT[r][i]` belongs to question `i` of region `r`, so keep the two files in the same order.
- `src/game.c`: progression, battle rules, question selection and save/load. It doesn't use raylib, so the tests link it directly.
- `src/main.c`: the raylib front end (screens, drawing, input).
