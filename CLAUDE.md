# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

Claude Quest: a C + raylib RPG that teaches how to use Claude. There are ten regions, from prompt craft to building agents. Each region has five lesson pages, battles fought by answering multiple-choice questions, an ability modeled on a real Claude feature, and a boss that unlocks the next region. It follows the same conventions as the sibling games in `C_Games/` (Mahjong, Poker_Trainer).

Repo: https://github.com/smartino555/Claude_Quest (branch `main`).

## Commands

```bash
make              # build ./claude_quest (raylib via pkg-config or Homebrew)
make run          # build and run
make test         # build and run tests/test_main.c (~240,000 checks, must stay at 0 failed)
make shots        # render every screen to shots/ for checking layout
make clean
```

After any UI change, run `make shots` and look at the affected screens. After any content change, run `make test`.

## Architecture

- `src/content.c`: the curriculum. `REGIONS[10]`, each with 5 `Page`s, 48 `Question`s (`NQ`) (`q`, `opt[4]`, `ans`, `why`), 3 foes, a boss, an `Ability` and a theme color.
- `src/context.c`: `CONTEXT[r][i]`, the background note shown by the Context button for `REGIONS[r].qs[i]`. **It is a parallel array: keep it in the same order as the questions in content.c.** If you add, remove or reorder a question, update context.c to match.
- `src/game.c` / `game.h`: rules with no raylib, so the tests link it directly. Covers the player (level, HP, tokens, XP), the battle state machine (`BS_ASK`, `BS_FEEDBACK`, `BS_WON`, `BS_LOST`), abilities, question selection, and save/load (text format, header `claudequest 2`; version 1 saves still load).
- **No-repeat rule** (`nextQuestion` in game.c): `Player.seen[r][i]` counts how often each question was dealt. A battle in region R only deals one of the least-dealt questions in regions 0..R, so nothing repeats until everything open has been seen. Review share (30/40/50%) applies only while earlier regions have least-dealt questions left.
- `src/main.c`: the raylib front end. It draws on a 1600x900 virtual canvas scaled with Camera2D. The states are title, map, region, lesson, battle, codex and ending. Overlays (help, confirmFlee, showCtx) take over input from the screen underneath.
- `tests/test_main.c`: checks content integrity, progression, every ability, the question-mix statistics, a save round trip and a simulated playthrough.

## Curriculum coverage

Using Claude inside your own applications is taught mainly in two regions, with a little in a third:

- **Region 9, The API Spire (the Claude API):** request anatomy, stateless conversations, tool use, streaming/vision/thinking, and cost and scale (caching, batches, rate limits, SDKs). Its questions also cover structured outputs, the Files API, citations, parallel tool calls, error codes, pinning model versions and keeping API keys server-side.
- **Region 10, The Agent Summit (building agents):** workflow patterns, the agent loop and Agent SDK, context engineering, tool design and computer use, and evals and safe deployment.
- **Region 3, Artifact Forge:** a lesson on AI-powered artifacts, which call Claude while they run.

Known gaps, with at most a question or two each: choosing a model for an app, RAG and embeddings, Claude on Amazon Bedrock or Google Vertex AI, production concerns (guardrails, monitoring, prompt versioning, latency) and a worked end-to-end example of a feature. One idea is an 11th region between the API Spire and the Agent Summit, such as a "Production Foundry". Adding a region means changing `NREGIONS`, the map, the abilities (key row Q-P has 10 keys) and the tests.

## Gotchas

- **ASCII only.** The bundled Inter fonts are ASCII subsets, and the tests enforce this. Use plain quotes and hyphens, never smart quotes or em dashes.
- **Append new questions at the end of a region's list.** Saves store progress by question index, so inserting or reordering would attach old progress to the wrong question.
- **Text length limits are tested**, because each piece of text must fit its box: question < 170 chars, option < 120, explanation < 260, lesson page < 1100, context note 150-700.
- **Don't include raymath.h.** Its `Wrap` collides with the local text-wrap helper; use the local `Dist()` instead.
- **Keys in use:** 1-4 answer, Q-P abilities, C context, J codex, H help, Esc back/retreat, Enter/Space continue. Check for conflicts before adding a key.
- `--shots DIR` mode never touches the save file. raylib writes screenshots next to the executable, and the code moves them into DIR.
- Progress is saved to `save.txt` next to the executable. It is git-ignored, along with the binaries and `shots/`.
- The curriculum was written without checking current Anthropic docs. Fast-changing details (UI features, CLI flags, pricing such as the cache-read and batch discounts) may need updating against the docs.
