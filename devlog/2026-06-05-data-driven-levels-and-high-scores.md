# Devlog — Data-Driven Levels & Persistent High Scores

*2026-06-05*

This update is mostly an under-the-hood pass: all level data now lives in CSV
files instead of being hard-coded in C++, and the player's best score is now
saved between sessions. No new gameplay, but the game is now far easier to
tweak and expand — and your high score finally sticks around.

## What changed

### Levels are now defined in CSV

Previously every level was described by big `const` arrays sitting at the top of
`CGame.cpp` — start positions, goals, timers, trigger zones, blocked paths and
enemy placements were all baked into the code. Changing a single spawn point
meant editing C++ and recompiling the whole project.

All of that now lives in `Data/CSV/`, loaded at startup by a new `CDataReader`
class:

| File | Holds |
|------|-------|
| `levelData.csv`   | Per-level start position, goal position/size, and timer |
| `enemySpawns.csv` | Every enemy spawn position, by level |
| `blockedPaths.csv`| The barriers that seal rooms during fights (position + scale) |
| `triggers.csv`    | The trigger volumes that start each encounter and the barriers they raise |
| `enemyGroups.csv` | Which enemy types make up each wave, and how many |

Each file is a plain table with a `LEVEL_ID` column, so the loaders just bucket
rows by level. Want to add an enemy to the second wave of level 2? Add one line:

```
LEVEL_ID,GROUP,TYPE,COUNT
1,1,ROBO,8
1,2,SPIDER,4   <-- e.g. bump this number, save, run
```

No rebuild required — the game reads the files fresh on launch.

### Enemy waves are data, not code

The enemy composition of each room used to be a literal list of
`new CSpider`, `new CRobo`, `new CBoss` objects. That's now expressed as
archetypes in `enemyGroups.csv`. `CDataReader` returns the *types* (it stays
free of any concrete enemy class), and the game constructs the actual objects
from them. Spawn order is preserved exactly, so every enemy still appears at the
same position it did before — this refactor is behavior-for-behavior identical,
just editable.

### High score is saved locally

The result screen has always shown a "HIGHEST SCORE", but it reset every time
you closed the game. It now persists to a small `highscore.dat` file:

- The saved score loads once at startup.
- Whenever you beat it, the new record is written to disk immediately.
- Beat your previous best and you'll still get the **NEW HIGH SCORE!** banner —
  except now it's a record you can actually chase across play sessions.

## Why bother?

Separating data from code is the kind of change that pays off every day after
it lands. Balancing a level, moving a spawn, retiming a room, or building an
entirely new stage is now a spreadsheet edit instead of a code change and a
rebuild. It also keeps `CGame.cpp` ~150 lines lighter and removes a wall of
magic numbers.

## Notes

- The CSV files are pure ASCII so they stay safe to hand-edit on any machine.
- `highscore.dat` is per-machine save data and is git-ignored; it's created
  automatically the first time you set a score.

---

*Built with DirectX 9/11. Thanks for playing — and good luck topping the
leaderboard.*
