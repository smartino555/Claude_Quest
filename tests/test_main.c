// test_main.c - curriculum integrity and game rules (no raylib needed)
#include "../src/game.h"
#include <stdio.h>
#include <string.h>

static int checks = 0, fails = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { fails++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

static bool asciiOnly(const char *s) {
    for (; *s; s++)
        if ((*s < 32 || *s > 126) && *s != '\n') return false;
    return true;
}

static void testContent(void) {
    int pos[4] = { 0 };
    for (int r = 0; r < NREGIONS; r++) {
        const Region *R = &REGIONS[r];
        CHECK(R->name && R->topic && R->mentor && R->greeting, "region %d missing text", r);
        CHECK(asciiOnly(R->name) && asciiOnly(R->topic) && asciiOnly(R->mentor) && asciiOnly(R->greeting), "region %d non-ASCII", r);
        CHECK(R->ability.name && R->ability.desc && R->ability.cost > 0, "region %d ability", r);
        CHECK(R->boss.name && R->boss.taunt, "region %d boss", r);
        for (int f = 0; f < 3; f++) CHECK(R->foes[f].name && R->foes[f].taunt && asciiOnly(R->foes[f].taunt), "region %d foe %d", r, f);
        for (int i = 0; i < NPAGES; i++) {
            const Page *p = &R->pages[i];
            CHECK(p->title && p->body && strlen(p->body) > 150, "region %d page %d too short or missing", r, i);
            CHECK(p->title && p->body && asciiOnly(p->title) && asciiOnly(p->body), "region %d page %d non-ASCII", r, i);
            CHECK(p->body && strlen(p->body) < 1100, "region %d page %d too long for one card (%zu)", r, i, strlen(p->body));
        }
        for (int i = 0; i < NQ; i++) {
            const Question *q = &R->qs[i];
            CHECK(q->q && q->why && strlen(q->why) > 20, "r%d q%d missing text", r, i);
            CHECK(q->ans >= 0 && q->ans < 4, "r%d q%d answer index %d", r, i, q->ans);
            CHECK(asciiOnly(q->q) && asciiOnly(q->why), "r%d q%d non-ASCII", r, i);
            CHECK(strlen(q->q) < 170, "r%d q%d question too long (%zu)", r, i, strlen(q->q));
            for (int o = 0; o < 4; o++) {
                CHECK(q->opt[o] && q->opt[o][0] && asciiOnly(q->opt[o]), "r%d q%d option %d", r, i, o);
                CHECK(q->opt[o] && strlen(q->opt[o]) < 120, "r%d q%d option %d too long", r, i, o);
                for (int o2 = 0; o2 < o; o2++) CHECK(strcmp(q->opt[o], q->opt[o2]) != 0, "r%d q%d duplicate options", r, i);
            }
            CHECK(strlen(q->why) < 260, "r%d q%d explanation too long (%zu)", r, i, strlen(q->why));
            pos[q->ans]++;
            for (int r2 = 0; r2 <= r; r2++)
                for (int j = 0; j < (r2 == r ? i : NQ); j++) CHECK(strcmp(q->q, REGIONS[r2].qs[j].q) != 0, "duplicate question r%d q%d", r, i);
            // the Context panel's background text
            const char *c = CONTEXT[r][i];
            CHECK(c && strlen(c) > 150 && asciiOnly(c), "r%d q%d context missing, short or non-ASCII", r, i);
            CHECK(c && strlen(c) < 700, "r%d q%d context too long for its panel (%zu)", r, i, c ? strlen(c) : 0);
            for (int r2 = 0; r2 <= r; r2++)
                for (int j = 0; j < (r2 == r ? i : NQ); j++) CHECK(c != CONTEXT[r2][j] && (!c || strcmp(c, CONTEXT[r2][j]) != 0), "duplicate context r%d q%d", r, i);
        }
    }
    printf("  authored answer positions: %d %d %d %d (shuffled at play time)\n", pos[0], pos[1], pos[2], pos[3]);
}

static void learnAll(Player *p, int upto) {
    for (int r = 0; r <= upto; r++) readPage(p, r, NPAGES - 1);
}

static int correctSlot(const Battle *b) {
    for (int s = 0; s < 4; s++) if (slotIsCorrect(b, s)) return s;
    return -1;
}
static int wrongSlot(const Battle *b) {
    for (int s = 0; s < 4; s++) if (!slotIsCorrect(b, s) && !b->hidden[s]) return s;
    return -1;
}

static void testProgress(void) {
    Player p;
    playerNew(&p);
    CHECK(p.level == 1 && p.hp == maxHp(&p) && p.tokens == maxTokens(&p) && p.unlocked == 0, "new player");
    const char *why;
    CHECK(!bossOpen(&p, 0, &why) && why, "boss locked at start");
    readPage(&p, 0, 2);
    CHECK(p.pagesRead[0] == 3 && !hasAbility(&p, 0), "pages 0-2 read, no ability yet");
    readPage(&p, 0, 0);
    CHECK(p.pagesRead[0] == 3, "reading an earlier page doesn't go backwards");
    readPage(&p, 0, NPAGES - 1);
    CHECK(hasAbility(&p, 0), "ability after last page");
    CHECK(!bossOpen(&p, 0, NULL), "boss needs wins");
    p.wins[0] = BOSS_WINS_NEEDED;
    CHECK(bossOpen(&p, 0, NULL), "boss open");
}

static void testBattle(void) {
    Player p;
    playerNew(&p);
    Battle b;
    battleStart(&b, &p, 0, false, 42);
    CHECK(b.state == BS_ASK && b.qr == 0, "region 0 asks region 0");
    int hp0 = b.hp;
    battleAnswer(&b, &p, correctSlot(&b));
    CHECK(b.lastRight && b.hp == hp0 - baseDamage(&p), "correct hit %d -> %d", hp0, b.hp);
    battleContinue(&b, &p);
    int php = p.hp;
    battleAnswer(&b, &p, wrongSlot(&b));
    CHECK(!b.lastRight && p.hp == php - b.atk && b.streak == 0, "wrong answer hurts");
    battleContinue(&b, &p);

    char msg[160];
    CHECK(!battleUseAbility(&b, &p, AB_CLARIFY, msg, sizeof msg), "ability not learned");
    readPage(&p, 0, NPAGES - 1);
    int tok = p.tokens;
    CHECK(battleUseAbility(&b, &p, AB_CLARIFY, msg, sizeof msg), "clarify: %s", msg);
    int hidden = 0;
    for (int s = 0; s < 4; s++) { hidden += b.hidden[s]; CHECK(!(b.hidden[s] && slotIsCorrect(&b, s)), "clarify hid the right answer"); }
    CHECK(hidden == 2 && p.tokens == tok - REGIONS[0].ability.cost, "clarify hid %d", hidden);
    CHECK(!battleUseAbility(&b, &p, AB_CLARIFY, msg, sizeof msg), "only one wrong left: %s", msg);
    battleAnswer(&b, &p, 0 + (b.hidden[0] ? (b.hidden[1] ? (b.hidden[2] ? 3 : 2) : 1) : 0));   // a visible slot
    CHECK(b.state == BS_FEEDBACK, "answer accepted");
    battleAnswer(&b, &p, 0);
    CHECK(b.state == BS_FEEDBACK, "can't answer twice");

    // rewind: no damage, wrong slot crossed out, same question again
    playerNew(&p);
    learnAll(&p, 9);
    battleStart(&b, &p, 4, false, 7);
    int q = b.qi, qr = b.qr;
    CHECK(battleUseAbility(&b, &p, AB_REWIND, msg, sizeof msg), "rewind: %s", msg);
    php = p.hp;
    int ws = wrongSlot(&b);
    battleAnswer(&b, &p, ws);
    CHECK(b.rewound && p.hp == php && b.hidden[ws], "rewound");
    battleContinue(&b, &p);
    CHECK(b.state == BS_ASK && b.qi == q && b.qr == qr, "same question after rewind");

    // damage modifiers stack: thinking x2, skill x1.5, loop 2 hits
    playerNew(&p);
    learnAll(&p, 9);
    p.tokens = 999;
    battleStart(&b, &p, 9, true, 9);
    battleUseAbility(&b, &p, AB_THINK, msg, sizeof msg);
    battleUseAbility(&b, &p, AB_SKILL, msg, sizeof msg);
    battleUseAbility(&b, &p, AB_LOOP, msg, sizeof msg);
    hp0 = b.hp;
    battleAnswer(&b, &p, correctSlot(&b));
    int d = baseDamage(&p) * 2 * 3 / 2 * 2;
    CHECK(hp0 - b.hp == d && b.lastHits == 2, "stacked damage %d, want %d", hp0 - b.hp, d);
    CHECK(b.thinkCharges == 0 && b.skillCharges == 2 && b.loopCharges == 2, "charges consumed");
    battleContinue(&b, &p);

    // cache makes abilities cheap; hook heals; subagent strikes on later turns
    CHECK(battleUseAbility(&b, &p, AB_CACHE, msg, sizeof msg), "cache");
    for (int a = 0; a < NREGIONS; a++) CHECK(abilityCost(&b, a) == (REGIONS[a].ability.cost + 9) / 10, "cached cost %d", a);
    CHECK(battleUseAbility(&b, &p, AB_HOOK, msg, sizeof msg), "hook");
    p.hp = 80;
    battleAnswer(&b, &p, wrongSlot(&b));
    CHECK(p.hp == 80 - b.atk + HOOK_HEAL && b.lastHeal == HOOK_HEAL, "hook heal after hit: %d", p.hp);
    battleContinue(&b, &p);
    CHECK(battleUseAbility(&b, &p, AB_SUBAGENT, msg, sizeof msg), "subagent");
    hp0 = b.hp;
    p.hp = maxHp(&p);
    battleAnswer(&b, &p, wrongSlot(&b));
    battleContinue(&b, &p);
    CHECK(b.hp == hp0 - ALLY_DMG && b.allyTurns == 2, "ally strike %d", hp0 - b.hp);
    p.hp = maxHp(&p);
    CHECK(!battleUseAbility(&b, &p, AB_VERSION, msg, sizeof msg), "version history refuses at full HP");
}

static void testWinLose(void) {
    Player p;
    playerNew(&p);
    learnAll(&p, 0);
    p.wins[0] = 2;
    Battle b;
    battleStart(&b, &p, 0, true, 3);
    while (b.state == BS_ASK) { battleAnswer(&b, &p, correctSlot(&b)); battleContinue(&b, &p); }
    CHECK(b.state == BS_WON && b.firstClear && b.unlockedNew && p.unlocked == 1 && p.bossBeaten[0], "boss win unlocks region 2");
    CHECK(b.wrongs == 0 && b.xpGain == 120 + 120 / 4, "flawless boss xp %d", b.xpGain);
    CHECK(p.level == 2, "levelled up to %d", p.level);
    battleStart(&b, &p, 0, true, 4);
    while (b.state == BS_ASK) { battleAnswer(&b, &p, correctSlot(&b)); battleContinue(&b, &p); }
    CHECK(!b.firstClear && !b.unlockedNew && p.unlocked == 1, "rematch doesn't re-unlock");

    battleStart(&b, &p, 0, false, 5);
    while (b.state == BS_ASK) { battleAnswer(&b, &p, wrongSlot(&b)); battleContinue(&b, &p); }
    CHECK(b.state == BS_LOST && p.hp == maxHp(&p) / 2, "loss sends you back with half HP");

    // final boss: no region after it
    playerNew(&p);
    learnAll(&p, 9);
    p.unlocked = 9;
    battleStart(&b, &p, 9, true, 6);
    while (b.state == BS_ASK) { battleAnswer(&b, &p, correctSlot(&b)); battleContinue(&b, &p); }
    CHECK(b.state == BS_WON && b.firstClear && !b.unlockedNew && p.unlocked == 9, "final boss");
}

// the least-dealt count among questions a battle in region reg could deal next (-1 if none)
static int leastDealt(const Player *p, const Battle *b, int reg) {
    int m = -1;
    for (int r = 0; r <= reg; r++)
        for (int i = 0; i < NQ; i++)
            if (!(b && b->used[r][i]) && (m < 0 || p->seen[r][i] < m)) m = p->seen[r][i];
    return m;
}

static void testQuestionMix(void) {
    Player p;
    playerNew(&p);
    Battle b;
    // no repeats across battles: every region-0 question is dealt once before any comes back,
    // including ones left unanswered by leaving a battle early
    int dealt = 0;
    for (int k = 0; dealt < NQ && k < 100; k++) {
        battleStart(&b, &p, 0, false, 11 + k);
        b.hp = 1 << 30;
        for (int t = 0;; t++) {
            dealt++;
            CHECK(p.seen[0][b.qi] == 1, "question %d dealt twice before all %d were dealt (deal %d)", b.qi, NQ, dealt);
            if (t == 4 || dealt == NQ) break;              // leave mid-battle
            battleAnswer(&b, &p, correctSlot(&b));
            battleContinue(&b, &p);
        }
    }
    for (int i = 0; i < NQ; i++) CHECK(p.seen[0][i] == 1, "question %d dealt %d times in the first round", i, p.seen[0][i]);
    battleStart(&b, &p, 0, false, 500);
    CHECK(p.seen[0][b.qi] == 2, "second round starts once all are dealt");

    // review only draws on earlier regions while they have undealt questions left
    playerNew(&p);
    int review = 0;
    for (int k = 0; k < 200; k++) {
        memset(p.seen, 0, sizeof p.seen);
        for (int r = 0; r < 5; r++) for (int i = 0; i < NQ; i++) p.seen[r][i] = 1;
        battleStart(&b, &p, 5, false, 3000 + k);
        review += b.qr != 5;
    }
    CHECK(review == 0, "dealt review questions while region 5 had new ones (%d)", review);
    review = 0;
    for (int k = 0; k < 200; k++) {
        memset(p.seen, 0, sizeof p.seen);
        for (int i = 0; i < NQ; i++) p.seen[5][i] = 1;
        battleStart(&b, &p, 5, false, 4000 + k);
        review += b.qr != 5;
    }
    CHECK(review == 200, "dealt a region 5 repeat while review questions were new (%d of 200 review)", review);

    // review share when everything is equally fresh, and mistakes come back more often
    int total = 0, missed = 0;
    review = 0;
    p.wrong[5][3] = 6;
    for (int k = 0; k < 400; k++) {
        memset(p.seen, 0, sizeof p.seen);
        battleStart(&b, &p, 5, false, 1000 + k);
        total++;
        review += b.qr != 5;
        missed += b.qr == 5 && b.qi == 3;
    }
    CHECK(review > total * 20 / 100 && review < total * 40 / 100, "normal battles ~30%% review, got %d/%d", review, total);
    CHECK(missed > (total - review) / NQ * 2, "missed question favoured: %d of %d", missed, total - review);
    review = total = 0;
    for (int k = 0; k < 400; k++) {
        memset(p.seen, 0, sizeof p.seen);
        battleStart(&b, &p, 9, true, 5000 + k);
        total++;
        review += b.qr != 9;
    }
    CHECK(review > total * 40 / 100 && review < total * 60 / 100, "final boss ~50%% review, got %d/%d", review, total);
    // options are shuffled
    int at[4] = { 0 };
    for (int k = 0; k < 400; k++) { battleStart(&b, &p, 0, false, 9000 + k); at[correctSlot(&b)]++; }
    for (int s = 0; s < 4; s++) CHECK(at[s] > 50, "right answer lands in slot %d only %d times", s, at[s]);
}

static void testSave(void) {
    Player p, q;
    playerNew(&p);
    p.level = 7; p.xp = 33; p.hp = 50; p.tokens = 12; p.unlocked = 4;
    p.pagesRead[2] = 5; p.wins[3] = 4; p.bossBeaten[1] = true;
    p.right[3][7] = 2; p.wrong[3][7] = 5; p.battles = 20; p.answered = 99; p.correct = 70;
    p.seen[3][7] = 9; p.seen[9][NQ - 1] = 1;
    const char *path = "test_save.txt";
    CHECK(saveGame(&p, path), "save");
    CHECK(loadGame(&q, path), "load");
    CHECK(memcmp(&p, &q, sizeof p) == 0, "round trip");
    // a version 1 save (12 questions a region, no dealt counts) still loads
    FILE *f = fopen(path, "w");
    fputs("claudequest 1\nplayer 4 10 80 20 2 9 50 40\nregion 0 5 3 1\nq 0 11 2 1\n", f);
    fclose(f);
    CHECK(loadGame(&q, path) && q.level == 4 && q.unlocked == 2 && q.right[0][11] == 2 && q.wrong[0][11] == 1, "v1 save loads");
    CHECK(q.seen[0][11] == 3 && q.seen[0][12] == 0, "v1 answers count as deals; new questions start undealt");
    f = fopen(path, "w");
    fputs("not a save file\n", f);
    fclose(f);
    q.level = 99;
    CHECK(!loadGame(&q, path) && q.level == 99, "garbage rejected and player untouched");
    remove(path);
    CHECK(!loadGame(&q, "no_such_file.txt"), "missing file");
}

// a 75%-accurate player using no abilities works through the whole game
static void testPlaythrough(void) {
    Player p;
    playerNew(&p);
    Rng r = { 77 };
    int battles = 0, turns = 0, losses = 0, repeats = 0;
    for (int reg = 0; reg < NREGIONS; reg++) {
        learnAll(&p, reg);
        while (!p.bossBeaten[reg] && battles < 2000) {
            Battle b;
            bool boss = bossOpen(&p, reg, NULL);
            // every deal must be one of the least-dealt questions still available
            int least = leastDealt(&p, NULL, reg);
            battleStart(&b, &p, reg, boss, 1234 + battles);
            CHECK(p.seen[b.qr][b.qi] - 1 == least, "battle opened with a repeat while fresher questions remained");
            repeats += p.seen[b.qr][b.qi] > 1;
            int guard = 0;
            while (b.state == BS_ASK && guard++ < 200) {
                battleAnswer(&b, &p, rngInt(&r, 100) < 75 ? correctSlot(&b) : wrongSlot(&b));
                int t0 = b.turns;
                least = leastDealt(&p, &b, reg);
                battleContinue(&b, &p);
                if (b.turns > t0 && least >= 0) {
                    CHECK(p.seen[b.qr][b.qi] - 1 == least, "dealt a repeat while fresher questions remained");
                    repeats += p.seen[b.qr][b.qi] > 1;
                }
            }
            CHECK(guard < 200, "battle never ended");
            battles++;
            turns += b.turns;
            if (b.state == BS_LOST) losses++;
            restAtInn(&p);
        }
    }
    CHECK(p.bossBeaten[NREGIONS - 1] && p.unlocked == NREGIONS - 1, "game completable");
    printf("  playthrough at 75%% accuracy: %d battles, %.1f questions each, %d losses, final level %d, %d repeated questions\n",
           battles, (double)turns / battles, losses, p.level, repeats);
}

int main(void) {
    printf("content\n");   testContent();
    printf("progress\n");  testProgress();
    printf("battle\n");    testBattle();
    printf("win/lose\n");  testWinLose();
    printf("questions\n"); testQuestionMix();
    printf("save\n");      testSave();
    printf("playthrough\n"); testPlaythrough();
    printf("%d checks, %d failed\n", checks, fails);
    return fails ? 1 : 0;
}
