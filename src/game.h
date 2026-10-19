// game.h - player progress, battle rules and saving (no raylib, so tests can link it)
#ifndef GAME_H
#define GAME_H

#include "content.h"
#include <stdbool.h>

// ability index == the region that teaches it
enum { AB_CLARIFY, AB_THINK, AB_VERSION, AB_SEARCH, AB_REWIND, AB_SKILL, AB_HOOK, AB_SUBAGENT, AB_CACHE, AB_LOOP };

enum { BS_ASK, BS_FEEDBACK, BS_WON, BS_LOST };

#define BOSS_WINS_NEEDED 2
#define ALLY_DMG 40
#define HOOK_HEAL 6
#define VERSION_HEAL 35

typedef struct { unsigned long long s; } Rng;
unsigned rngNext(Rng *r);
int rngInt(Rng *r, int n);                // 0..n-1

typedef struct {
    int level, xp, hp, tokens;
    int unlocked;                         // highest region index the player may enter
    int pagesRead[NREGIONS];              // lesson pages read, 0..NPAGES
    int wins[NREGIONS];
    bool bossBeaten[NREGIONS];
    int right[NREGIONS][NQ], wrong[NREGIONS][NQ];
    int battles, answered, correct;       // lifetime totals
} Player;

void playerNew(Player *p);
int maxHp(const Player *p);
int maxTokens(const Player *p);
int xpToNext(int level);
int baseDamage(const Player *p);
bool hasAbility(const Player *p, int a);
bool bossOpen(const Player *p, int r, const char **why);
void readPage(Player *p, int r, int page);   // marks pages 0..page as read
void restAtInn(Player *p);

typedef struct {
    int region, foe;                      // foe 0..2, or 3 for the boss
    bool boss;
    int hp, maxhp, atk;
    // the question on screen
    int qr, qi;                           // its region and index
    int order[4];                         // display slot -> option index in the content
    bool hidden[4];                       // slots removed by Clarify / Web Search / rewind
    bool used[NREGIONS][NQ];
    // ability effects
    int thinkCharges, skillCharges, loopCharges, allyTurns;
    bool rewindArmed, hookOn, cacheOn;
    // bookkeeping
    int state, streak, turns, rights, wrongs;
    // what the last answer did (for the feedback panel)
    int lastSlot, lastDmg, lastTaken, lastHeal, lastHits;
    bool lastRight, rewound, combo;
    int allyDmg;                          // ally strike before the current question
    // results
    int xpGain, levelsGained;
    bool unlockedNew, firstClear;        // firstClear: this win beat the boss for the first time
    Rng rng;
} Battle;

void battleStart(Battle *b, Player *p, int region, bool boss, unsigned long long seed);
int abilityCost(const Battle *b, int a);
// returns false (and explains why in msg) when the ability can't be used now
bool battleUseAbility(Battle *b, Player *p, int a, char *msg, int n);
void battleAnswer(Battle *b, Player *p, int slot);
void battleContinue(Battle *b, Player *p);
bool slotIsCorrect(const Battle *b, int slot);
const Question *battleQuestion(const Battle *b);

bool saveGame(const Player *p, const char *path);
bool loadGame(Player *p, const char *path);

#endif
