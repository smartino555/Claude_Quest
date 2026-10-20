// game.c - player progress, battle rules and saving
#include "game.h"
#include <stdio.h>
#include <string.h>

// ---------------------------------------------------------------- rng (xorshift64*)

unsigned rngNext(Rng *r) {
    if (!r->s) r->s = 0x9E3779B97F4A7C15ull;
    r->s ^= r->s >> 12;
    r->s ^= r->s << 25;
    r->s ^= r->s >> 27;
    return (unsigned)((r->s * 0x2545F4914F6CDD1Dull) >> 32);
}
int rngInt(Rng *r, int n) { return n > 0 ? (int)(rngNext(r) % (unsigned)n) : 0; }

// ---------------------------------------------------------------- player

int maxHp(const Player *p) { return 90 + 10 * p->level; }
int maxTokens(const Player *p) { return 20 + 4 * p->level; }
int xpToNext(int level) { return 60 + 40 * level; }
int baseDamage(const Player *p) { return 100 + 6 * (p->level - 1); }

void playerNew(Player *p) {
    memset(p, 0, sizeof *p);
    p->level = 1;
    p->hp = maxHp(p);
    p->tokens = maxTokens(p);
}

bool hasAbility(const Player *p, int a) { return a >= 0 && a < NREGIONS && p->pagesRead[a] >= NPAGES; }

bool bossOpen(const Player *p, int r, const char **why) {
    const char *w = NULL;
    if (p->pagesRead[r] < NPAGES) w = "Finish the mentor's lessons first.";
    else if (p->wins[r] < BOSS_WINS_NEEDED) w = "Win 2 battles in this region first.";
    if (why) *why = w;
    return w == NULL;
}

void readPage(Player *p, int r, int page) {
    if (page + 1 > p->pagesRead[r]) p->pagesRead[r] = page + 1;
}

void restAtInn(Player *p) {
    p->hp = maxHp(p);
    p->tokens = maxTokens(p);
}

static void gainXp(Player *p, int xp, int *levels) {
    p->xp += xp;
    while (p->xp >= xpToNext(p->level)) {
        p->xp -= xpToNext(p->level);
        p->level++;
        (*levels)++;
        p->hp = maxHp(p);
        p->tokens = maxTokens(p);
    }
}

// ---------------------------------------------------------------- question picking

// No repeats: a battle in region R draws on regions 0..R, and only ever deals one
// of the least-dealt questions there. So nothing comes back until every question
// you can be asked has been dealt as often. Among those, earlier regions come back
// as review (30% of normal battles, 40% of bosses, half of the final boss) while
// they have any left, and questions you got wrong are favoured.
static int reviewShare(const Battle *b) {
    if (b->region == 0) return 0;
    return b->boss ? (b->region == NREGIONS - 1 ? 50 : 40) : 30;
}

static void nextQuestion(Battle *b, Player *p) {
    int least = -1;
    for (int pass = 0; pass < 2 && least < 0; pass++) {
        if (pass == 1) {                                   // all asked this battle: start over,
            memset(b->used, 0, sizeof b->used);            // but not with the one just asked
            if (b->qr >= 0) b->used[b->qr][b->qi] = true;
        }
        for (int r = 0; r <= b->region; r++)
            for (int i = 0; i < NQ; i++)
                if (!b->used[r][i] && (least < 0 || p->seen[r][i] < least)) least = p->seen[r][i];
    }
    bool own = false, old = false;
    for (int r = 0; r <= b->region; r++)
        for (int i = 0; i < NQ; i++)
            if (!b->used[r][i] && p->seen[r][i] == least) { if (r == b->region) own = true; else old = true; }
    bool review = old && (!own || rngInt(&b->rng, 100) < reviewShare(b));

    int w[NREGIONS][NQ], tot = 0;
    for (int r = 0; r <= b->region; r++)
        for (int i = 0; i < NQ; i++) {
            w[r][i] = 0;
            if (b->used[r][i] || p->seen[r][i] != least || (r != b->region) != review) continue;
            int x = 3 + 4 * p->wrong[r][i] - (p->right[r][i] > 2 ? 2 : p->right[r][i]);
            w[r][i] = x < 1 ? 1 : x;
            tot += w[r][i];
        }
    int x = rngInt(&b->rng, tot), qr = 0, qi = 0;
    for (int r = 0; r <= b->region; r++)
        for (int i = 0; i < NQ; i++) {
            if (!w[r][i]) continue;
            if (x >= 0 && x < w[r][i]) { qr = r; qi = i; }
            x -= w[r][i];
        }
    b->qr = qr;
    b->qi = qi;
    b->used[qr][qi] = true;
    p->seen[qr][qi]++;
    for (int i = 0; i < 4; i++) { b->order[i] = i; b->hidden[i] = false; }
    for (int i = 3; i > 0; i--) {
        int j = rngInt(&b->rng, i + 1), t = b->order[i];
        b->order[i] = b->order[j];
        b->order[j] = t;
    }
    b->turns++;
    b->state = BS_ASK;
}

const Question *battleQuestion(const Battle *b) { return &REGIONS[b->qr].qs[b->qi]; }
bool slotIsCorrect(const Battle *b, int slot) { return b->order[slot] == battleQuestion(b)->ans; }

// ---------------------------------------------------------------- battle

void battleStart(Battle *b, Player *p, int region, bool boss, unsigned long long seed) {
    memset(b, 0, sizeof *b);
    b->rng.s = seed ? seed : 1;
    b->region = region;
    b->boss = boss;
    b->foe = boss ? 3 : rngInt(&b->rng, 3);
    b->maxhp = boss ? 700 + 60 * region : 300 + 30 * region;
    b->hp = b->maxhp;
    b->atk = boss ? 22 + 4 * region : 16 + 3 * region;
    b->qr = -1;
    b->qi = -1;
    p->battles++;
    nextQuestion(b, p);
}

int abilityCost(const Battle *b, int a) {
    int c = REGIONS[a].ability.cost;
    if (b->cacheOn) c = (c + 9) / 10;
    return c < 1 ? 1 : c;
}

static int visibleWrong(const Battle *b, int *slots) {
    int n = 0;
    for (int s = 0; s < 4; s++)
        if (!b->hidden[s] && !slotIsCorrect(b, s)) slots[n++] = s;
    return n;
}

static void hideWrong(Battle *b, int count) {
    int slots[4], n = visibleWrong(b, slots);
    for (int k = 0; k < count && n > 0; k++) {
        int j = rngInt(&b->rng, n);
        b->hidden[slots[j]] = true;
        slots[j] = slots[--n];
    }
}

bool battleUseAbility(Battle *b, Player *p, int a, char *msg, int n) {
    int slots[4];
    const char *no = NULL;
    if (b->state != BS_ASK) no = "Use abilities while a question is open.";
    else if (!hasAbility(p, a)) no = "You haven't learned that ability yet.";
    else if (p->tokens < abilityCost(b, a)) no = "Not enough tokens. Correct answers restore 3; the inn restores all.";
    else switch (a) {
    case AB_CLARIFY: no = visibleWrong(b, slots) < 2 ? "Too few wrong answers left to clear." : NULL; break;
    case AB_SEARCH: no = visibleWrong(b, slots) < 1 ? "Only the right answer is left." : NULL; break;
    case AB_THINK: no = b->thinkCharges ? "Extended Thinking is already active." : NULL; break;
    case AB_VERSION: no = p->hp >= maxHp(p) ? "HP is already full." : NULL; break;
    case AB_REWIND: no = b->rewindArmed ? "/rewind is already armed." : NULL; break;
    case AB_SKILL: no = b->skillCharges ? "The skill is already loaded." : NULL; break;
    case AB_HOOK: no = b->hookOn ? "The hook is already installed." : NULL; break;
    case AB_SUBAGENT: no = b->allyTurns ? "Your subagent is still working." : NULL; break;
    case AB_CACHE: no = b->cacheOn ? "The cache is already warm." : NULL; break;
    case AB_LOOP: no = b->loopCharges ? "The agent loop is already running." : NULL; break;
    }
    if (no) { snprintf(msg, n, "%s", no); return false; }

    p->tokens -= abilityCost(b, a);
    switch (a) {
    case AB_CLARIFY: hideWrong(b, 2); snprintf(msg, n, "Clarify: two wrong answers fade away."); break;
    case AB_SEARCH: hideWrong(b, 1); snprintf(msg, n, "Web Search: one wrong answer ruled out."); break;
    case AB_THINK: b->thinkCharges = 1; snprintf(msg, n, "Extended Thinking: your next correct answer deals double damage."); break;
    case AB_VERSION: {
        int h = maxHp(p) - p->hp;
        if (h > VERSION_HEAL) h = VERSION_HEAL;
        p->hp += h;
        snprintf(msg, n, "Version History: restored %d HP.", h);
        break;
    }
    case AB_REWIND: b->rewindArmed = true; snprintf(msg, n, "/rewind armed: a wrong answer will be rewound."); break;
    case AB_SKILL: b->skillCharges = 3; snprintf(msg, n, "Skill loaded: +50%% damage on your next 3 correct answers."); break;
    case AB_HOOK: b->hookOn = true; snprintf(msg, n, "PostToolUse hook installed: heal %d HP after every answer.", HOOK_HEAL); break;
    case AB_SUBAGENT: b->allyTurns = 3; snprintf(msg, n, "A subagent joins you. It strikes at the start of your next 3 turns."); break;
    case AB_CACHE: b->cacheOn = true; snprintf(msg, n, "Prompt Cache warmed: abilities now cost 10%%."); break;
    case AB_LOOP: b->loopCharges = 3; snprintf(msg, n, "Agent Loop running: your next 3 correct answers strike twice."); break;
    }
    return true;
}

void battleAnswer(Battle *b, Player *p, int slot) {
    if (b->state != BS_ASK || slot < 0 || slot > 3 || b->hidden[slot]) return;
    bool ok = slotIsCorrect(b, slot);
    b->lastSlot = slot;
    b->lastRight = ok;
    b->lastDmg = b->lastTaken = b->lastHeal = b->lastHits = 0;
    b->rewound = b->combo = false;
    b->allyDmg = 0;
    p->answered++;
    if (ok) {
        p->correct++;
        p->right[b->qr][b->qi]++;
        b->rights++;
        b->streak++;
        int d = baseDamage(p);
        if (b->streak >= 3) { d = d * 5 / 4; b->combo = true; }
        if (b->thinkCharges) { d *= 2; b->thinkCharges = 0; }
        if (b->skillCharges) { d = d * 3 / 2; b->skillCharges--; }
        b->lastHits = 1;
        if (b->loopCharges) { b->lastHits = 2; b->loopCharges--; }
        b->lastDmg = d * b->lastHits;
        b->hp -= b->lastDmg;
        if (b->hp < 0) b->hp = 0;
        p->tokens += 3;
        if (p->tokens > maxTokens(p)) p->tokens = maxTokens(p);
    } else {
        p->wrong[b->qr][b->qi]++;
        b->wrongs++;
        b->streak = 0;
        if (b->rewindArmed) {
            b->rewindArmed = false;
            b->rewound = true;
            b->hidden[slot] = true;
        } else {
            b->lastTaken = b->atk;
            p->hp -= b->atk;
            if (p->hp < 0) p->hp = 0;
        }
    }
    if (b->hookOn && p->hp > 0 && p->hp < maxHp(p)) {
        int h = maxHp(p) - p->hp;
        b->lastHeal = h < HOOK_HEAL ? h : HOOK_HEAL;
        p->hp += b->lastHeal;
    }
    b->state = BS_FEEDBACK;
}

static void finish(Battle *b, Player *p, bool won) {
    if (won) {
        b->state = BS_WON;
        b->xpGain = b->boss ? 120 + 30 * b->region : 30 + 10 * b->region;
        if (b->wrongs == 0) b->xpGain += b->xpGain / 4;        // flawless bonus
        p->wins[b->region]++;
        if (b->boss && !p->bossBeaten[b->region]) {
            p->bossBeaten[b->region] = true;
            b->firstClear = true;
            if (b->region + 1 < NREGIONS && p->unlocked < b->region + 1) {
                p->unlocked = b->region + 1;
                b->unlockedNew = true;
            }
        }
        gainXp(p, b->xpGain, &b->levelsGained);
    } else {
        b->state = BS_LOST;
        p->hp = maxHp(p) / 2;
    }
}

void battleContinue(Battle *b, Player *p) {
    if (b->state != BS_FEEDBACK) return;
    if (b->rewound) { b->state = BS_ASK; b->rewound = false; return; }
    if (b->hp <= 0) { finish(b, p, true); return; }
    if (p->hp <= 0) { finish(b, p, false); return; }
    if (b->allyTurns > 0) {
        b->allyTurns--;
        b->hp -= ALLY_DMG;
        if (b->hp <= 0) { b->hp = 0; b->allyDmg = ALLY_DMG; finish(b, p, true); return; }
        nextQuestion(b, p);
        b->allyDmg = ALLY_DMG;
        return;
    }
    nextQuestion(b, p);
}

// ---------------------------------------------------------------- save / load

bool saveGame(const Player *p, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    fprintf(f, "claudequest 2\n");
    fprintf(f, "player %d %d %d %d %d %d %d %d\n", p->level, p->xp, p->hp, p->tokens, p->unlocked,
            p->battles, p->answered, p->correct);
    for (int r = 0; r < NREGIONS; r++) {
        fprintf(f, "region %d %d %d %d\n", r, p->pagesRead[r], p->wins[r], p->bossBeaten[r] ? 1 : 0);
        for (int i = 0; i < NQ; i++)
            if (p->right[r][i] || p->wrong[r][i] || p->seen[r][i])
                fprintf(f, "q %d %d %d %d %d\n", r, i, p->right[r][i], p->wrong[r][i], p->seen[r][i]);
    }
    return fclose(f) == 0;
}

bool loadGame(Player *p, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return false;
    Player t;
    playerNew(&t);
    char line[200];
    int ver = 0;
    bool havePlayer = false;
    while (fgets(line, sizeof line, f)) {
        int a, b, c, d, e, g, h, k, n;
        if (sscanf(line, "claudequest %d", &a) == 1) ver = a;
        else if (sscanf(line, "player %d %d %d %d %d %d %d %d", &a, &b, &c, &d, &e, &g, &h, &k) == 8) {
            t.level = a < 1 ? 1 : a;
            t.xp = b; t.hp = c; t.tokens = d;
            t.unlocked = e < 0 ? 0 : e >= NREGIONS ? NREGIONS - 1 : e;
            t.battles = g; t.answered = h; t.correct = k;
            havePlayer = true;
        } else if (sscanf(line, "region %d %d %d %d", &a, &b, &c, &d) == 4) {
            if (a < 0 || a >= NREGIONS) continue;
            t.pagesRead[a] = b < 0 ? 0 : b > NPAGES ? NPAGES : b;
            t.wins[a] = c < 0 ? 0 : c;
            t.bossBeaten[a] = d != 0;
        } else if ((n = sscanf(line, "q %d %d %d %d %d", &a, &b, &c, &d, &e)) >= 4) {
            if (a < 0 || a >= NREGIONS || b < 0 || b >= NQ) continue;
            t.right[a][b] = c < 0 ? 0 : c;
            t.wrong[a][b] = d < 0 ? 0 : d;
            // version 1 saves had no dealt count; every answer was one deal
            t.seen[a][b] = n == 5 && e >= 0 ? e : t.right[a][b] + t.wrong[a][b];
        }
    }
    fclose(f);
    if ((ver != 1 && ver != 2) || !havePlayer) return false;
    if (t.hp < 1 || t.hp > maxHp(&t)) t.hp = maxHp(&t);
    if (t.tokens < 0 || t.tokens > maxTokens(&t)) t.tokens = maxTokens(&t);
    *p = t;
    return true;
}
