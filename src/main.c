// main.c - raylib front end for Claude Quest
#include "raylib.h"

#include "game.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VW 1600          // virtual canvas
#define VH 900
#define HUD_H 56

enum { ST_TITLE, ST_MAP, ST_REGION, ST_LESSON, ST_BATTLE, ST_CODEX, ST_ENDING };

// ---------------------------------------------------------------- globals

static Font fBold, fReg;
static bool hasUI = false;
static Camera2D cam;
static Vector2 mouse;
static bool clickNow = false;

static Player pl;
static Battle bt;
static int state = ST_TITLE, codexReturn = ST_MAP;
static int cur = 0;                       // selected region on the map
static int menuSel = 0, titleSel = 0;
static int lessonPage = 0;
static bool lessonHadAbility = false;    // so the "ability learned" toast shows once
static bool showHelp = false, confirmNew = false, confirmFlee = false, haveSave = false;
static bool showCtx = false;               // the Context panel in battle
static char savePath[1024];
static bool noSave = false;               // screenshot mode never touches the save
static Vector2 heroMapPos;

static float animT = 0, flashT = 0;
static char flashMsg[200];
static float monHurtT = 0, heroHurtT = 0, boltT = 0, allyT = 0, lungeT = 0;

typedef struct { float x, y, t; char s[40]; Color c; } Floater;
static Floater floaters[8];

static int codexTab = 0, codexRegion = 0;
static float codexScroll = 0, codexMax = 0;

// palette
static const Color C_BG     = { 15, 17, 27, 255 };
static const Color C_PANEL  = { 27, 30, 45, 255 };
static const Color C_PANEL2 = { 38, 42, 62, 255 };
static const Color C_PANEL3 = { 52, 57, 84, 255 };
static const Color C_TEXT   = { 234, 230, 220, 255 };
static const Color C_DIM    = { 148, 150, 172, 255 };
static const Color C_GOLD   = { 238, 190, 92, 255 };
static const Color C_RED    = { 226, 86, 76, 255 };
static const Color C_GREEN  = { 98, 198, 124, 255 };
static const Color C_BLUE   = { 102, 156, 236, 255 };
static const Color C_PARCH  = { 241, 232, 210, 255 };
static const Color C_INK    = { 50, 42, 36, 255 };
static const Color C_INK2   = { 112, 98, 84, 255 };
static const Color C_SKIN   = { 236, 200, 162, 255 };

static float Dist(Vector2 a, Vector2 b) { return hypotf(a.x - b.x, a.y - b.y); }
static Color RegionCol(int r) { return (Color){ REGIONS[r].col[0], REGIONS[r].col[1], REGIONS[r].col[2], 255 }; }
static Color Darken(Color c, float k) { return (Color){ (unsigned char)(c.r * k), (unsigned char)(c.g * k), (unsigned char)(c.b * k), c.a }; }

// ---------------------------------------------------------------- text & widgets

static void Txt(Font f, const char *s, float x, float y, float sz, Color c) {
    DrawTextEx(f, s, (Vector2){ x, y }, sz, hasUI ? 0.0f : sz / 10.0f, c);
}
static float TxtW(Font f, const char *s, float sz) { return MeasureTextEx(f, s, sz, hasUI ? 0.0f : sz / 10.0f).x; }
static void TxtC(Font f, const char *s, float cx, float y, float sz, Color c) { Txt(f, s, cx - TxtW(f, s, sz) / 2, y, sz, c); }
static void TxtR(Font f, const char *s, float rx, float y, float sz, Color c) { Txt(f, s, rx - TxtW(f, s, sz), y, sz, c); }
static void Flash(const char *msg) { snprintf(flashMsg, sizeof flashMsg, "%s", msg); flashT = 3.2f; }

// word-wrapped text; a blank line is a short paragraph gap. Returns the height used.
static float Wrap(Font f, const char *s, float x, float y, float w, float sz, float lh, Color c, bool draw) {
    char line[700] = "", test[800];
    float yy = y;
    const char *p = s;
    while (*p) {
        if (*p == '\n') {
            if (line[0]) { if (draw) Txt(f, line, x, yy, sz, c); yy += lh; line[0] = 0; }
            else yy += lh * 0.55f;
            p++;
            continue;
        }
        const char *q = p;
        while (*q && *q != ' ' && *q != '\n') q++;
        int wl = (int)(q - p);
        snprintf(test, sizeof test, "%s%s%.*s", line, line[0] ? " " : "", wl, p);
        if (line[0] && TxtW(f, test, sz) > w) {
            if (draw) Txt(f, line, x, yy, sz, c);
            yy += lh;
            snprintf(line, sizeof line, "%.*s", wl, p);
        } else snprintf(line, sizeof line, "%s", test);
        p = q;
        while (*p == ' ') p++;
    }
    if (line[0]) { if (draw) Txt(f, line, x, yy, sz, c); yy += lh; }
    return yy - y;
}

// largest size (down to min) at which the text fits in maxH
static float FitSize(Font f, const char *s, float w, float maxH, float sz, float minSz, float lhK) {
    while (sz > minSz && Wrap(f, s, 0, 0, w, sz, sz * lhK, WHITE, false) > maxH) sz -= 1;
    return sz;
}

static bool Hover(Rectangle r) { return CheckCollisionPointRec(mouse, r); }
static bool Clicked(Rectangle r) { return clickNow && Hover(r); }

static bool Button(Rectangle r, const char *label, float sz, bool enabled, bool selected) {
    bool hov = enabled && Hover(r);
    Color bg = !enabled ? C_PANEL : (hov || selected) ? C_PANEL3 : C_PANEL2;
    DrawRectangleRounded(r, 0.3f, 8, bg);
    if (enabled && (hov || selected)) DrawRectangleRoundedLinesEx(r, 0.3f, 8, 2, C_GOLD);
    TxtC(fBold, label, r.x + r.width / 2, r.y + (r.height - sz) / 2, sz, enabled ? C_TEXT : C_DIM);
    return hov && clickNow;
}

static void Bar(float x, float y, float w, float h, float frac, Color fill, Color back) {
    if (frac < 0) frac = 0;
    if (frac > 1) frac = 1;
    DrawRectangleRounded((Rectangle){ x, y, w, h }, 0.5f, 6, back);
    if (frac > 0) DrawRectangleRounded((Rectangle){ x, y, fmaxf(w * frac, h), h }, 0.5f, 6, fill);
}

static void AddFloater(float x, float y, const char *s, Color c) {
    for (int i = 0; i < 8; i++)
        if (floaters[i].t <= 0) {
            floaters[i] = (Floater){ x, y, 1.4f, "", c };
            snprintf(floaters[i].s, sizeof floaters[i].s, "%s", s);
            return;
        }
}

static void Save(void) {
    if (noSave) return;
    if (saveGame(&pl, savePath)) haveSave = true;
}

// ---------------------------------------------------------------- shapes & characters

// raylib culls one winding; draw both so callers needn't care about vertex order
static void Tri(Vector2 a, Vector2 b, Vector2 c, Color col) {
    DrawTriangle(a, b, c, col);
    DrawTriangle(a, c, b, col);
}

typedef struct { Color robe, hat, orb; bool beard; } Look;

static void DrawFigure(float x, float y, float s, float face, Look lk, float hurt) {
    float bob = sinf(animT * 2.2f) * 3 * s;
    if (hurt > 0) x += sinf(animT * 70) * 7 * hurt;
    DrawEllipse((int)x, (int)y, 36 * s, 9 * s, Fade(BLACK, 0.35f));
    Color robe = hurt > 0 ? ColorLerp(lk.robe, C_RED, hurt * 0.7f) : lk.robe;
    DrawRectangleRec((Rectangle){ x - 14 * s, y - 26 * s, 9 * s, 26 * s }, Darken(robe, 0.45f));
    DrawRectangleRec((Rectangle){ x + 5 * s, y - 26 * s, 9 * s, 26 * s }, Darken(robe, 0.45f));
    Vector2 tl = { x - 19 * s, y - 92 * s + bob }, tr = { x + 19 * s, y - 92 * s + bob };
    Vector2 bl = { x - 32 * s, y - 20 * s }, br = { x + 32 * s, y - 20 * s };
    Tri(tl, tr, br, robe);
    Tri(tl, br, bl, robe);
    DrawLineEx((Vector2){ x - 24 * s, y - 52 * s + bob * 0.5f }, (Vector2){ x + 24 * s, y - 52 * s + bob * 0.5f }, 4 * s, Darken(robe, 0.6f));
    Vector2 head = { x, y - 106 * s + bob };
    DrawCircleV(head, 17 * s, C_SKIN);
    DrawCircleV((Vector2){ x + 7 * s * face, head.y - 1 * s }, 2.6f * s, C_INK);
    if (lk.beard) Tri((Vector2){ x - 13 * s, head.y + 4 * s }, (Vector2){ x + 13 * s, head.y + 4 * s }, (Vector2){ x + 3 * s * face, head.y + 42 * s }, (Color){ 236, 236, 240, 255 });
    DrawRectangleRec((Rectangle){ x - 27 * s, head.y - 15 * s, 54 * s, 6 * s }, lk.hat);
    Tri((Vector2){ x - 19 * s, head.y - 12 * s }, (Vector2){ x + 19 * s, head.y - 12 * s }, (Vector2){ x - 12 * s * face, head.y - 66 * s }, lk.hat);
    DrawCircleV((Vector2){ x - 2 * s * face, head.y - 34 * s }, 3.5f * s, C_GOLD);
    Vector2 st0 = { x + 34 * s * face, y - 4 * s }, st1 = { x + 42 * s * face, y - 146 * s + bob };
    DrawLineEx(st0, st1, 5 * s, (Color){ 120, 84, 52, 255 });
    float glow = 0.5f + 0.5f * sinf(animT * 3);
    DrawCircleGradient(st1, 22 * s, Fade(lk.orb, 0.25f + 0.25f * glow), Fade(lk.orb, 0));
    DrawCircleV(st1, 7 * s, lk.orb);
}

static Look HeroLook(void) { return (Look){ (Color){ 204, 112, 70, 255 }, (Color){ 64, 70, 112, 255 }, C_GOLD, false }; }
static Look MentorLook(int r) { Color c = RegionCol(r); return (Look){ Darken(c, 0.8f), Darken(c, 0.5f), ColorLerp(c, WHITE, 0.4f), true }; }

static Color FoeCol(int region, int foe) {
    Vector3 h = ColorToHSV(RegionCol(region));
    float hue = fmodf(h.x + (foe == 3 ? 0 : (foe - 1) * 28.0f) + 360, 360);
    return ColorFromHSV(hue, fminf(1, h.y * (foe == 3 ? 1.2f : 1.0f)), foe == 3 ? h.z * 0.8f : h.z);
}

static void DrawMonster(float cx, float cy, float R, int seed, Color col, bool boss, float hurt) {
    float t = animT;
    if (hurt > 0) cx += sinf(t * 80) * 9 * hurt;
    cy += sinf(t * 1.7f + seed) * 5;
    DrawEllipse((int)cx, (int)(cy + R * 0.95f), R * 0.95f, R * 0.18f, Fade(BLACK, 0.35f));
    if (boss) DrawCircleGradient((Vector2){ cx, cy }, R * 1.75f, Fade(col, 0.35f + 0.1f * sinf(t * 2)), Fade(col, 0));
    Color dark = Darken(col, 0.55f);
    int spikes = boss ? 5 : 2 + seed % 4;
    for (int i = 0; i < spikes; i++) {
        float a = -PI * (0.18f + 0.64f * (spikes == 1 ? 0.5f : (float)i / (spikes - 1)));
        float w = boss ? 0.16f : 0.2f, len = boss ? (i % 2 ? 1.45f : 1.75f) : 1.4f;
        Vector2 b1 = { cx + cosf(a - w) * R * 0.9f, cy + sinf(a - w) * R * 0.85f };
        Vector2 b2 = { cx + cosf(a + w) * R * 0.9f, cy + sinf(a + w) * R * 0.85f };
        Vector2 tip = { cx + cosf(a) * R * len, cy + sinf(a) * R * len * 0.95f };
        Tri(b1, b2, tip, boss ? C_GOLD : dark);
    }
    Color body = hurt > 0 ? ColorLerp(col, WHITE, hurt * 0.8f) : col;
    const int N = 56;
    Vector2 c = { cx, cy };
    for (int i = 0; i < N; i++) {
        float a0 = 2 * PI * i / N, a1 = 2 * PI * (i + 1) / N;
        float r0 = R * (1 + 0.06f * sinf(3 * a0 + t * 2 + seed) + 0.04f * sinf(5 * a0 - t * 1.3f));
        float r1 = R * (1 + 0.06f * sinf(3 * a1 + t * 2 + seed) + 0.04f * sinf(5 * a1 - t * 1.3f));
        Tri(c, (Vector2){ cx + cosf(a0) * r0, cy + sinf(a0) * r0 * 0.88f }, (Vector2){ cx + cosf(a1) * r1, cy + sinf(a1) * r1 * 0.88f }, body);
    }
    DrawEllipse((int)cx, (int)(cy + R * 0.3f), R * 0.55f, R * 0.4f, Fade(WHITE, 0.12f));
    int eyes = boss ? 3 : 1 + seed % 3;
    float er = eyes == 1 ? R * 0.3f : R * 0.2f;
    bool blink = fmodf(t + seed * 0.7f, 4.0f) < 0.13f;
    for (int i = 0; i < eyes; i++) {
        float ex = cx + (eyes == 1 ? 0 : (i - (eyes - 1) / 2.0f) * R * 0.5f);
        float ey = cy - R * 0.22f - (eyes == 3 && i == 1 ? R * 0.12f : 0);
        if (blink) { DrawLineEx((Vector2){ ex - er, ey }, (Vector2){ ex + er, ey }, 4, C_INK); continue; }
        DrawCircleV((Vector2){ ex, ey }, er, (Color){ 250, 248, 240, 255 });
        DrawCircleV((Vector2){ ex - er * 0.35f, ey + er * 0.1f }, er * 0.45f, boss ? (Color){ 180, 30, 30, 255 } : C_INK);
        DrawCircleV((Vector2){ ex - er * 0.5f, ey - er * 0.1f }, er * 0.14f, WHITE);
    }
    float my = cy + R * 0.32f, mw = R * (boss ? 0.5f : 0.36f);
    DrawEllipse((int)cx, (int)my, mw, R * 0.13f, (Color){ 40, 16, 24, 255 });
    if (boss || seed % 2) {
        int teeth = boss ? 6 : 3;
        for (int i = 0; i < teeth; i++) {
            float tx = cx - mw * 0.8f + (2 * mw * 0.8f) * (i + 0.5f) / teeth;
            Tri((Vector2){ tx - 7, my - R * 0.1f }, (Vector2){ tx + 7, my - R * 0.1f }, (Vector2){ tx, my + R * 0.04f }, WHITE);
        }
    }
}

static void Stars(int count, float x0, float y0, float w, float h, unsigned seed) {
    Rng r = { seed };
    for (int i = 0; i < count; i++) {
        float x = x0 + rngInt(&r, (int)w), y = y0 + rngInt(&r, (int)h);
        float tw = 0.4f + 0.6f * (0.5f + 0.5f * sinf(animT * (0.6f + (i % 5) * 0.3f) + i));
        DrawCircleV((Vector2){ x, y }, 1 + (i % 3) * 0.6f, Fade(C_TEXT, 0.25f * tw));
    }
}

static void Mountains(float baseY, Color c, unsigned seed, float hMax) {
    Rng r = { seed };
    float x = -60;
    while (x < VW) {
        float w = 160 + rngInt(&r, 220), h = hMax * (0.45f + rngInt(&r, 55) / 100.0f);
        Tri((Vector2){ x, baseY }, (Vector2){ x + w, baseY }, (Vector2){ x + w / 2, baseY - h }, c);
        x += w * 0.6f;
    }
}

// ---------------------------------------------------------------- HUD

static void DrawHud(void) {
    DrawRectangle(0, 0, VW, HUD_H, C_PANEL);
    DrawLine(0, HUD_H, VW, HUD_H, C_PANEL3);
    Txt(fBold, "CLAUDE QUEST", 24, 16, 22, C_GOLD);
    char s[64];
    snprintf(s, sizeof s, "Level %d", pl.level);
    Txt(fBold, s, 230, 16, 22, C_TEXT);
    float x = 350;
    Txt(fReg, "HP", x, 18, 18, C_DIM);
    Bar(x + 32, 21, 170, 14, (float)pl.hp / maxHp(&pl), C_RED, C_PANEL3);
    snprintf(s, sizeof s, "%d/%d", pl.hp, maxHp(&pl));
    Txt(fReg, s, x + 212, 18, 18, C_TEXT);
    x = 650;
    Txt(fReg, "Tokens", x, 18, 18, C_DIM);
    Bar(x + 62, 21, 140, 14, (float)pl.tokens / maxTokens(&pl), C_BLUE, C_PANEL3);
    snprintf(s, sizeof s, "%d/%d", pl.tokens, maxTokens(&pl));
    Txt(fReg, s, x + 212, 18, 18, C_TEXT);
    x = 950;
    Txt(fReg, "XP", x, 18, 18, C_DIM);
    Bar(x + 32, 21, 140, 14, (float)pl.xp / xpToNext(pl.level), C_GOLD, C_PANEL3);
    snprintf(s, sizeof s, "%d/%d", pl.xp, xpToNext(pl.level));
    Txt(fReg, s, x + 182, 18, 18, C_TEXT);
    TxtR(fReg, "J  Codex      H  Help      Esc  Back", VW - 24, 18, 18, C_DIM);
}

// ---------------------------------------------------------------- title

static const char *TITLE_ITEMS[4] = { "Continue", "New game", "How to play", "Quit" };

static void StartNewGame(void) {
    playerNew(&pl);
    cur = 0;
    state = ST_REGION;
    menuSel = 0;
    Save();
}

static void DrawTitle(bool *quit) {
    DrawRectangleGradientV(0, 0, VW, VH, (Color){ 22, 20, 40, 255 }, C_BG);
    Stars(160, 0, 0, VW, 560, 7);
    Mountains(VH, (Color){ 30, 30, 52, 255 }, 3, 340);
    Mountains(VH, (Color){ 22, 22, 38, 255 }, 11, 220);
    TxtC(fBold, "CLAUDE QUEST", VW / 2, 150, 104, C_GOLD);
    TxtC(fReg, "Ten regions. Every Claude feature. One apprentice.", VW / 2, 272, 28, C_TEXT);
    DrawFigure(VW / 2 - 360, 700, 1.6f, 1, HeroLook(), 0);
    DrawMonster(VW / 2 + 380, 590, 80, 9, FoeCol(9, 3), true, 0);

    if (confirmNew) {
        Rectangle p = { VW / 2 - 330, 380, 660, 230 };
        DrawRectangleRounded(p, 0.12f, 8, C_PANEL);
        DrawRectangleRoundedLinesEx(p, 0.12f, 8, 2, C_RED);
        TxtC(fBold, "Start over?", VW / 2, p.y + 30, 32, C_TEXT);
        TxtC(fReg, "This erases your saved progress.", VW / 2, p.y + 80, 22, C_DIM);
        bool yes = Button((Rectangle){ p.x + 90, p.y + 140, 220, 56 }, "Erase and start", 22, true, false);
        bool no = Button((Rectangle){ p.x + 350, p.y + 140, 220, 56 }, "Keep my save", 22, true, false);
        if (yes || IsKeyPressed(KEY_Y)) { confirmNew = false; StartNewGame(); }
        else if (no || IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) confirmNew = false;
        return;
    }

    int first = haveSave ? 0 : 1;
    if (titleSel < first) titleSel = first;
    if (IsKeyPressed(KEY_DOWN)) titleSel = titleSel >= 3 ? first : titleSel + 1;
    if (IsKeyPressed(KEY_UP)) titleSel = titleSel <= first ? 3 : titleSel - 1;
    int act = -1;
    for (int i = first; i < 4; i++) {
        Rectangle r = { VW / 2 - 170, 380 + (i - first) * 74, 340, 60 };
        if (Hover(r)) titleSel = i;
        if (Button(r, TITLE_ITEMS[i], 26, true, titleSel == i)) act = i;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) act = titleSel;
    if (act == 0) { state = ST_MAP; cur = pl.unlocked; heroMapPos = (Vector2){ 0, 0 }; }
    else if (act == 1) { if (haveSave) confirmNew = true; else StartNewGame(); }
    else if (act == 2) showHelp = true;
    else if (act == 3) *quit = true;
    TxtC(fReg, "Arrow keys and Enter, or the mouse", VW / 2, VH - 44, 18, C_DIM);
}

// ---------------------------------------------------------------- map

static const Vector2 NODES[NREGIONS] = {
    { 140, 770 }, { 360, 690 }, { 230, 520 }, { 430, 380 }, { 690, 470 },
    { 650, 700 }, { 900, 780 }, { 960, 560 }, { 800, 290 }, { 1010, 150 },
};

static void DrawLock(float x, float y, Color c) {
    DrawRing((Vector2){ x, y - 6 }, 7, 10, 180, 360, 12, c);
    DrawRectangleRounded((Rectangle){ x - 12, y - 6, 24, 18 }, 0.3f, 4, c);
}

static void DrawMap(void) {
    DrawRectangleGradientV(0, HUD_H, VW, VH - HUD_H, (Color){ 20, 26, 40, 255 }, (Color){ 14, 18, 26, 255 });
    Stars(120, 0, HUD_H, 1120, VH - HUD_H, 21);
    for (int i = 0; i < NREGIONS; i++) {
        Color c = RegionCol(i);
        DrawCircleGradient(NODES[i], 150, Fade(c, i <= pl.unlocked ? 0.16f : 0.05f), Fade(c, 0));
    }
    for (int i = 0; i + 1 < NREGIONS; i++) {
        Vector2 a = NODES[i], b = NODES[i + 1];
        bool open = i + 1 <= pl.unlocked;
        if (open) DrawLineEx(a, b, 6, Fade(C_GOLD, 0.7f));
        else {
            float len = Dist(a, b);
            for (float d = 0; d < len; d += 22) {
                float t0 = d / len, t1 = fminf(1, (d + 11) / len);
                DrawLineEx((Vector2){ a.x + (b.x - a.x) * t0, a.y + (b.y - a.y) * t0 },
                           (Vector2){ a.x + (b.x - a.x) * t1, a.y + (b.y - a.y) * t1 }, 4, Fade(C_DIM, 0.35f));
            }
        }
    }

    // input
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN)) { if (cur < pl.unlocked) cur++; }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_UP)) { if (cur > 0) cur--; }
    bool enter = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);

    for (int i = 0; i < NREGIONS; i++) {
        Vector2 p = NODES[i];
        bool open = i <= pl.unlocked, sel = i == cur;
        Color c = RegionCol(i);
        if (sel) DrawCircleV(p, 44 + 3 * sinf(animT * 3), Fade(C_GOLD, 0.35f));
        DrawCircleV(p, 36, open ? c : (Color){ 46, 50, 66, 255 });
        DrawRing(p, 34, 38, 0, 360, 40, pl.bossBeaten[i] ? C_GOLD : open ? Darken(c, 0.6f) : C_PANEL3);
        if (open) {
            char n[4];
            snprintf(n, sizeof n, "%d", i + 1);
            TxtC(fBold, n, p.x, p.y - 17, 32, (Color){ 20, 18, 26, 255 });
        } else DrawLock(p.x, p.y + 2, C_DIM);
        TxtC(fBold, REGIONS[i].name, p.x, p.y + 46, 19, open ? C_TEXT : C_DIM);
        if (open && clickNow && Dist(mouse, p) < 40) {
            if (cur == i) enter = true;
            cur = i;
        }
    }
    if (heroMapPos.x == 0 && heroMapPos.y == 0) heroMapPos = NODES[cur];
    Vector2 tgt = NODES[cur];
    float k = fminf(1, GetFrameTime() * 8);
    heroMapPos.x += (tgt.x - heroMapPos.x) * k;
    heroMapPos.y += (tgt.y - heroMapPos.y) * k;
    DrawFigure(heroMapPos.x + 30, heroMapPos.y - 30, 0.55f, 1, HeroLook(), 0);

    // side panel
    const Region *R = &REGIONS[cur];
    Rectangle pn = { 1140, 84, 432, 790 };
    DrawRectangleRounded(pn, 0.05f, 8, C_PANEL);
    DrawRectangle((int)pn.x + 20, (int)pn.y + 22, 6, 74, RegionCol(cur));
    char s[200];
    snprintf(s, sizeof s, "REGION %d OF %d", cur + 1, NREGIONS);
    Txt(fBold, s, pn.x + 42, pn.y + 22, 16, C_DIM);
    Wrap(fBold, R->name, pn.x + 42, pn.y + 44, 360, 32, 36, C_TEXT, true);
    float y = pn.y + 128;
    Txt(fBold, R->topic, pn.x + 28, y, 24, RegionCol(cur));
    y += 44;
    snprintf(s, sizeof s, "Mentor: %s", R->mentor);
    Txt(fReg, s, pn.x + 28, y, 20, C_TEXT);
    y += 46;
    const char *lbl[4] = { "Lessons", "Battles won", "Boss", "Ability" };
    for (int i = 0; i < 4; i++) {
        Txt(fReg, lbl[i], pn.x + 28, y, 19, C_DIM);
        Color vc = C_TEXT;
        switch (i) {
        case 0: snprintf(s, sizeof s, "%d of %d pages", pl.pagesRead[cur], NPAGES); if (pl.pagesRead[cur] >= NPAGES) vc = C_GREEN; break;
        case 1: snprintf(s, sizeof s, "%d", pl.wins[cur]); break;
        case 2: snprintf(s, sizeof s, "%s", pl.bossBeaten[cur] ? "Defeated" : "Undefeated"); vc = pl.bossBeaten[cur] ? C_GREEN : C_GOLD; break;
        case 3: snprintf(s, sizeof s, "%s", R->ability.name); vc = hasAbility(&pl, cur) ? C_GREEN : C_TEXT; break;
        }
        Txt(fBold, s, pn.x + 160, y, 19, vc);
        y += 36;
    }
    y += 8;
    Txt(fReg, hasAbility(&pl, cur) ? "Learned. Use it in battle with the key shown." : "Learn it by finishing the lessons here.", pn.x + 28, y, 17, C_DIM);
    y += 28;
    Wrap(fReg, R->ability.desc, pn.x + 28, y, 380, 19, 26, C_TEXT, true);
    y += 70;
    DrawLine((int)pn.x + 28, (int)y, (int)(pn.x + pn.width - 28), (int)y, C_PANEL3);
    y += 22;
    snprintf(s, sizeof s, "Boss: %s", R->boss.name);
    Wrap(fBold, s, pn.x + 28, y, 380, 20, 26, C_TEXT, true);
    y += 60;
    if (cur > 0 && cur < NREGIONS) {
        snprintf(s, sizeof s, "Builds on: %s", REGIONS[cur - 1].topic);
        Txt(fReg, s, pn.x + 28, y, 18, C_DIM);
    }
    if (Button((Rectangle){ pn.x + 28, pn.y + pn.height - 92, pn.width - 56, 64 }, "Enter region", 26, true, true)) enter = true;
    if (enter) { state = ST_REGION; menuSel = 0; }

    // legend
    int done = 0;
    for (int i = 0; i < NREGIONS; i++) done += pl.bossBeaten[i];
    snprintf(s, sizeof s, "Bosses defeated: %d of %d", done, NREGIONS);
    Txt(fBold, s, 32, 84, 20, C_GOLD);
    Txt(fReg, "Left / Right to travel, Enter to visit", 32, 112, 18, C_DIM);
}

// ---------------------------------------------------------------- region

static void DrawScene(int r, float x0, float w) {
    Color c = RegionCol(r);
    DrawRectangleGradientV((int)x0, HUD_H, (int)w, VH - HUD_H, Darken(c, 0.3f), Darken(c, 0.12f));
    Stars(60, x0, HUD_H, w, 360, 100 + r);
    BeginScissorMode((int)(cam.offset.x + x0 * cam.zoom), (int)(cam.offset.y + HUD_H * cam.zoom), (int)(w * cam.zoom), (int)((VH - HUD_H) * cam.zoom));
    Mountains(720, Darken(c, 0.22f), 40 + r, 300);
    Mountains(760, Darken(c, 0.16f), 80 + r, 180);
    EndScissorMode();
    DrawRectangle((int)x0, 740, (int)w, VH - 740, Darken(c, 0.1f));
}

static void Bubble(Rectangle r, const char *text, float sz, float tailX, bool tailDown) {
    float h = Wrap(fReg, text, 0, 0, r.width - 44, sz, sz * 1.4f, WHITE, false) + 36;
    r.height = h;
    DrawRectangleRounded(r, 0.2f, 8, C_PARCH);
    Vector2 a = { tailX - 14, tailDown ? r.y + h - 1 : r.y + 1 }, b = { tailX + 14, a.y }, t = { tailX, tailDown ? a.y + 26 : a.y - 26 };
    Tri(a, b, t, C_PARCH);
    Wrap(fReg, text, r.x + 22, r.y + 18, r.width - 44, sz, sz * 1.4f, C_INK, true);
}

static void StartBattle(bool boss) {
    battleStart(&bt, &pl, cur, boss, ((unsigned long long)(GetTime() * 1e6)) ^ (unsigned long long)rand() << 20 ^ 0xC1A0DEull);
    state = ST_BATTLE;
    confirmFlee = showCtx = false;
    monHurtT = heroHurtT = boltT = allyT = lungeT = 0;
    for (int i = 0; i < 8; i++) floaters[i].t = 0;
}

static void DrawRegion(void) {
    const Region *R = &REGIONS[cur];
    DrawScene(cur, 0, 900);
    DrawFigure(250, 760, 1.7f, 1, MentorLook(cur), 0);
    DrawFigure(640, 770, 1.5f, -1, HeroLook(), 0);
    Txt(fBold, R->mentor, 150, 790, 24, C_TEXT);
    Bubble((Rectangle){ 60, 110, 780, 0 }, R->greeting, 24, 260, true);

    Rectangle pn = { 900, 56, 700, VH - 56 };
    DrawRectangleRec(pn, C_PANEL);
    char s[200];
    snprintf(s, sizeof s, "REGION %d  |  %s", cur + 1, R->topic);
    Txt(fBold, s, 970, 92, 18, RegionCol(cur));
    Txt(fBold, R->name, 970, 118, 40, C_TEXT);

    const char *why = NULL;
    bool bossOk = bossOpen(&pl, cur, &why);
    const char *title[5], *sub[5];
    char t0[96], s0[160], s1[160], t2[96], s2[160];
    snprintf(t0, sizeof t0, "Lessons with %s", R->mentor);
    if (pl.pagesRead[cur] >= NPAGES) snprintf(s0, sizeof s0, "All %d pages read. Review them any time.", NPAGES);
    else snprintf(s0, sizeof s0, "%d of %d pages read. Finish them to learn %s.", pl.pagesRead[cur], NPAGES, R->ability.name);
    snprintf(s1, sizeof s1, "Won %d. Questions from this region%s.", pl.wins[cur], cur ? ", plus review of earlier ones" : "");
    snprintf(t2, sizeof t2, "Challenge %s", R->boss.name);
    if (!bossOk) snprintf(s2, sizeof s2, "Locked. %s", why);
    else if (pl.bossBeaten[cur]) snprintf(s2, sizeof s2, "Defeated. Rematch for extra XP.");
    else if (cur + 1 < NREGIONS) snprintf(s2, sizeof s2, "Defeat it to open the road to %s.", REGIONS[cur + 1].name);
    else snprintf(s2, sizeof s2, "The final battle. It draws on every region.");
    title[0] = t0; sub[0] = s0;
    title[1] = "Seek a battle"; sub[1] = s1;
    title[2] = t2; sub[2] = s2;
    title[3] = "Rest at the inn"; sub[3] = "Restore all HP and tokens.";
    title[4] = "Back to the map"; sub[4] = "Travel to another region.";

    if (IsKeyPressed(KEY_DOWN)) menuSel = (menuSel + 1) % 5;
    if (IsKeyPressed(KEY_UP)) menuSel = (menuSel + 4) % 5;
    int act = -1;
    for (int i = 0; i < 5; i++) if (IsKeyPressed(KEY_ONE + i)) act = i;
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) act = menuSel;
    for (int i = 0; i < 5; i++) {
        Rectangle r = { 970, 200 + i * 118, 590, 104 };
        bool en = i != 2 || bossOk;
        bool sel = menuSel == i;
        if (Hover(r)) menuSel = i;
        DrawRectangleRounded(r, 0.18f, 8, sel ? C_PANEL3 : C_PANEL2);
        if (sel) DrawRectangleRoundedLinesEx(r, 0.18f, 8, 2, en ? C_GOLD : C_DIM);
        char k[4];
        snprintf(k, sizeof k, "%d", i + 1);
        DrawCircleV((Vector2){ r.x + 36, r.y + 36 }, 17, en ? RegionCol(cur) : C_PANEL3);
        TxtC(fBold, k, r.x + 36, r.y + 24, 22, C_BG);
        Txt(fBold, title[i], r.x + 70, r.y + 20, 25, en ? C_TEXT : C_DIM);
        Wrap(fReg, sub[i], r.x + 70, r.y + 56, r.width - 90, 18, 23, i == 2 && !bossOk ? C_GOLD : C_DIM, true);
        if (Clicked(r)) act = i;
    }
    switch (act) {
    case 0: state = ST_LESSON; lessonHadAbility = hasAbility(&pl, cur); lessonPage = lessonHadAbility ? 0 : pl.pagesRead[cur]; break;
    case 1: StartBattle(false); break;
    case 2: if (bossOk) StartBattle(true); else Flash(why); break;
    case 3: restAtInn(&pl); Save(); Flash("You rest at the inn. HP and tokens restored."); break;
    case 4: state = ST_MAP; break;
    }
}

// ---------------------------------------------------------------- lessons

static void DrawLesson(void) {
    const Region *R = &REGIONS[cur];
    DrawScene(cur, 0, VW);
    Rectangle card = { 200, 76, 1200, 806 };
    DrawRectangleRounded(card, 0.04f, 8, C_PARCH);
    DrawRectangleRoundedLinesEx(card, 0.04f, 8, 3, Darken(RegionCol(cur), 0.8f));
    DrawCircleV((Vector2){ card.x + 64, card.y + 58 }, 26, Darken(RegionCol(cur), 0.8f));
    DrawCircleV((Vector2){ card.x + 64, card.y + 62 }, 13, C_SKIN);
    Tri((Vector2){ card.x + 48, card.y + 52 }, (Vector2){ card.x + 80, card.y + 52 }, (Vector2){ card.x + 60, card.y + 22 }, Darken(RegionCol(cur), 0.5f));
    char s[160];
    snprintf(s, sizeof s, "%s  |  %s  |  Lesson %d of %d", R->mentor, R->topic, lessonPage + 1, NPAGES);
    Txt(fBold, s, card.x + 108, card.y + 46, 20, C_INK2);
    const Page *pg = &R->pages[lessonPage];
    Txt(fBold, pg->title, card.x + 60, card.y + 100, 42, C_INK);
    float bw = card.width - 120, avail = 560;
    float sz = FitSize(fReg, pg->body, bw, avail, 26, 19, 1.48f);
    Wrap(fReg, pg->body, card.x + 60, card.y + 168, bw, sz, sz * 1.48f, C_INK, true);

    readPage(&pl, cur, lessonPage);
    for (int i = 0; i < NPAGES; i++)
        DrawCircleV((Vector2){ card.x + card.width / 2 - 48 + i * 24, card.y + card.height - 48 }, i == lessonPage ? 7 : 5, i == lessonPage ? C_INK : C_INK2);
    bool last = lessonPage == NPAGES - 1;
    bool prev = lessonPage > 0 && (Button((Rectangle){ card.x + 40, card.y + card.height - 76, 200, 56 }, "Previous", 22, true, false) || IsKeyPressed(KEY_LEFT));
    bool next = Button((Rectangle){ card.x + card.width - 240, card.y + card.height - 76, 200, 56 }, last ? "Finish" : "Next", 22, true, true)
             || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    if (prev) lessonPage--;
    else if (next) {
        if (!last) lessonPage++;
        else {
            bool firstTime = !lessonHadAbility;
            readPage(&pl, cur, NPAGES - 1);
            state = ST_REGION;
            menuSel = 1;
            snprintf(s, sizeof s, "Ability learned: %s. %s", R->ability.name, R->ability.desc);
            Flash(firstTime ? s : "Lessons reviewed.");
        }
        Save();
    }
}

// ---------------------------------------------------------------- battle

static const int AB_KEYS[NREGIONS] = { KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P };
static const char *AB_KEYNAMES = "QWERTYUIOP";

#define HERO_X 300
#define HERO_Y 440
#define MON_X 1180
#define MON_Y 300

static void UseAbility(int a) {
    char msg[160];
    int hpBefore = pl.hp;
    if (battleUseAbility(&bt, &pl, a, msg, sizeof msg)) {
        Flash(msg);
        if (pl.hp > hpBefore) { char f[24]; snprintf(f, sizeof f, "+%d HP", pl.hp - hpBefore); AddFloater(HERO_X, HERO_Y - 200, f, C_GREEN); }
        if (a == AB_SUBAGENT) allyT = 1.2f;
    } else Flash(msg);
}

static void Answer(int slot) {
    if (bt.state != BS_ASK || bt.hidden[slot]) return;
    battleAnswer(&bt, &pl, slot);
    char f[40];
    if (bt.lastRight) {
        boltT = 0.45f;
        monHurtT = 0.6f;
        snprintf(f, sizeof f, "-%d", bt.lastDmg);
        AddFloater(MON_X - 170, MON_Y - 40, f, C_GOLD);
    } else if (bt.rewound) {
        AddFloater(HERO_X, HERO_Y - 200, "Rewound!", C_BLUE);
    } else {
        lungeT = 0.5f;
        heroHurtT = 0.6f;
        snprintf(f, sizeof f, "-%d HP", bt.lastTaken);
        AddFloater(HERO_X, HERO_Y - 200, f, C_RED);
    }
    if (bt.lastHeal) { snprintf(f, sizeof f, "+%d HP", bt.lastHeal); AddFloater(HERO_X + 60, HERO_Y - 160, f, C_GREEN); }
}

static void Continue(void) {
    int before = bt.hp;
    battleContinue(&bt, &pl);
    if (bt.hp < before && bt.allyDmg) {
        char f[24];
        snprintf(f, sizeof f, "-%d ally", bt.allyDmg);
        AddFloater(MON_X - 170, MON_Y + 10, f, C_BLUE);
        allyT = 0.8f;
        monHurtT = 0.4f;
    }
    if (bt.state == BS_WON || bt.state == BS_LOST) Save();
}

static void EffectChips(float x, float y) {
    char s[8][40];
    Color c[8];
    int n = 0;
    if (bt.thinkCharges) { snprintf(s[n], 40, "Thinking x2"); c[n++] = C_GOLD; }
    if (bt.skillCharges) { snprintf(s[n], 40, "Skill +50%% (%d)", bt.skillCharges); c[n++] = C_GOLD; }
    if (bt.loopCharges) { snprintf(s[n], 40, "Agent Loop (%d)", bt.loopCharges); c[n++] = C_GOLD; }
    if (bt.allyTurns) { snprintf(s[n], 40, "Subagent (%d)", bt.allyTurns); c[n++] = C_BLUE; }
    if (bt.rewindArmed) { snprintf(s[n], 40, "/rewind armed"); c[n++] = C_BLUE; }
    if (bt.hookOn) { snprintf(s[n], 40, "Hook +%d HP", HOOK_HEAL); c[n++] = C_GREEN; }
    if (bt.cacheOn) { snprintf(s[n], 40, "Cache warm"); c[n++] = C_GREEN; }
    if (bt.streak >= 3) { snprintf(s[n], 40, "Combo x1.25"); c[n++] = C_GOLD; }
    for (int i = 0; i < n; i++) {
        float w = TxtW(fBold, s[i], 15) + 20;
        if (x + w > 880) { x = 40; y += 32; }
        DrawRectangleRounded((Rectangle){ x, y, w, 26 }, 0.5f, 6, Fade(c[i], 0.2f));
        DrawRectangleRoundedLinesEx((Rectangle){ x, y, w, 26 }, 0.5f, 6, 1.5f, c[i]);
        Txt(fBold, s[i], x + 10, y + 5, 15, c[i]);
        x += w + 8;
    }
}

static void DrawBattleOverlay(void) {
    DrawRectangle(0, 0, VW, VH, Fade(BLACK, 0.6f));
    bool won = bt.state == BS_WON;
    const Foe *foe = bt.boss ? &REGIONS[bt.region].boss : &REGIONS[bt.region].foes[bt.foe];
    float ph = won ? 400 + 42 * (bt.levelsGained > 0) + 42 * bt.unlockedNew : 470;
    Rectangle p = { VW / 2 - 400, VH / 2 - ph / 2, 800, ph };
    DrawRectangleRounded(p, 0.08f, 8, C_PANEL);
    DrawRectangleRoundedLinesEx(p, 0.08f, 8, 3, won ? C_GOLD : C_RED);
    TxtC(fBold, won ? "Victory" : "Defeated", VW / 2, p.y + 40, 56, won ? C_GOLD : C_RED);
    char s[240];
    float y = p.y + 128;
    if (won) {
        snprintf(s, sizeof s, "%s is defeated.", foe->name);
        TxtC(fReg, s, VW / 2, y, 26, C_TEXT); y += 52;
        snprintf(s, sizeof s, "+%d XP%s", bt.xpGain, bt.wrongs == 0 ? "  (flawless bonus included)" : "");
        TxtC(fBold, s, VW / 2, y, 28, C_GOLD); y += 46;
        if (bt.levelsGained) {
            snprintf(s, sizeof s, "Level up! You are now level %d. HP and tokens restored.", pl.level);
            TxtC(fBold, s, VW / 2, y, 24, C_GREEN); y += 42;
        }
        if (bt.unlockedNew) {
            snprintf(s, sizeof s, "The road is open: %s (%s)", REGIONS[bt.region + 1].name, REGIONS[bt.region + 1].topic);
            TxtC(fBold, s, VW / 2, y, 24, C_BLUE); y += 42;
        }
    } else {
        Wrap(fReg, "You retreat to the inn with half your HP. Review this region's lessons in the Codex (J); the Mistakes tab lists every question you missed, with the right answer.",
             p.x + 70, y, p.width - 140, 24, 34, C_TEXT, true);
        y += 150;
    }
    snprintf(s, sizeof s, "%d correct, %d missed", bt.rights, bt.wrongs);
    TxtC(fReg, s, VW / 2, p.y + p.height - 150, 22, C_DIM);
    if (Button((Rectangle){ VW / 2 - 140, p.y + p.height - 96, 280, 62 }, "Continue", 26, true, true) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (won && bt.firstClear && bt.region == NREGIONS - 1) state = ST_ENDING;   // first final-boss win
        else { state = ST_REGION; menuSel = won ? 1 : 0; }
    }
}

// background for the question on screen; free, and open while asking or after answering
static void DrawContext(bool input) {
    const Question *q = battleQuestion(&bt);
    const char *ctx = CONTEXT[bt.qr][bt.qi];
    DrawRectangle(0, 0, VW, VH, Fade(BLACK, 0.65f));
    float w = 1160, tw = w - 120;
    float qh = Wrap(fBold, q->q, 0, 0, tw, 22, 22 * 1.3f, WHITE, false);
    float ch = Wrap(fReg, ctx, 0, 0, tw, 23, 23 * 1.45f, WHITE, false);
    float h = 96 + qh + 34 + ch + 84;
    Rectangle p = { VW / 2 - w / 2, VH / 2 - h / 2, w, h };
    DrawRectangleRounded(p, 0.05f, 8, C_PANEL);
    DrawRectangleRoundedLinesEx(p, 0.05f, 8, 2, C_BLUE);
    Txt(fBold, "Context", p.x + 60, p.y + 32, 34, C_BLUE);
    char s[200];
    snprintf(s, sizeof s, "Region %d  |  %s", bt.qr + 1, REGIONS[bt.qr].topic);
    TxtR(fBold, s, p.x + p.width - 60, p.y + 44, 17, RegionCol(bt.qr));
    float y = p.y + 96;
    Wrap(fBold, q->q, p.x + 60, y, tw, 22, 22 * 1.3f, C_DIM, true);
    y += qh + 16;
    DrawRectangle((int)p.x + 60, (int)y, (int)tw, 2, C_PANEL3);
    Wrap(fReg, ctx, p.x + 60, y + 18, tw, 23, 23 * 1.45f, C_TEXT, true);
    TxtC(fReg, "Press C, Esc or click to close", VW / 2, p.y + p.height - 46, 17, C_DIM);
    if (input && (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_ESCAPE) || clickNow)) showCtx = false;
}

// a tile like the ability tiles; returns true when clicked
static bool ContextTile(Rectangle r, bool enabled) {
    bool hov = enabled && Hover(r);
    DrawRectangleRounded(r, 0.18f, 8, hov ? C_PANEL3 : C_PANEL2);
    DrawRectangleRounded((Rectangle){ r.x, r.y, r.width, 6 }, 0.5f, 4, C_BLUE);
    if (hov) DrawRectangleRoundedLinesEx(r, 0.18f, 8, 2, C_GOLD);
    TxtC(fBold, "Context", r.x + r.width / 2, r.y + 24, 18, C_TEXT);
    TxtC(fReg, "C  |  free", r.x + r.width / 2, r.y + 60, 16, C_BLUE);
    return hov && clickNow;
}

static void DrawBattle(void) {
    const Region *R = &REGIONS[bt.region];
    const Foe *foe = bt.boss ? &R->boss : &R->foes[bt.foe];
    Color rc = RegionCol(bt.region);
    float dt = GetFrameTime();

    // the Context panel and help swallow the battle's input; the panel reads the real click itself
    bool blocked = showCtx || showHelp, ctxWasOpen = showCtx, realClick = clickNow;
    Vector2 realMouse = mouse;
    if (showCtx) { clickNow = false; mouse = (Vector2){ -100, -100 }; }

    // arena
    DrawRectangleGradientV(0, HUD_H, VW, 410, Darken(rc, 0.3f), Darken(rc, 0.14f));
    Stars(50, 0, HUD_H, VW, 200, 300 + bt.region);
    BeginScissorMode((int)cam.offset.x, (int)(cam.offset.y + HUD_H * cam.zoom), (int)(VW * cam.zoom), (int)(410 * cam.zoom));
    Mountains(430, Darken(rc, 0.2f), 500 + bt.region, 200);
    EndScissorMode();
    DrawRectangle(0, 430, VW, 36, Darken(rc, 0.1f));

    float lunge = lungeT > 0 ? sinf((0.5f - lungeT) / 0.5f * PI) * 120 : 0;
    float R0 = bt.boss ? 112 : 86;
    DrawMonster(MON_X - lunge, MON_Y + (bt.boss ? 10 : 30), R0, bt.region * 4 + bt.foe, FoeCol(bt.region, bt.foe), bt.boss, monHurtT / 0.6f);
    DrawFigure(HERO_X, HERO_Y, 1.25f, 1, HeroLook(), heroHurtT / 0.6f);
    if (allyT > 0 || bt.allyTurns) {
        float a = allyT > 0 ? 1 : 0.85f;
        DrawFigure(HERO_X + 150, HERO_Y + 10, 0.8f, 1, (Look){ Fade(C_BLUE, a), Fade((Color){ 40, 60, 120, 255 }, a), C_BLUE, false }, 0);
    }
    if (boltT > 0) {
        Vector2 a = { HERO_X + 54, HERO_Y - 182 }, b = { MON_X - lunge, MON_Y + 10 };
        float k = 1 - boltT / 0.45f;
        Vector2 hd = { a.x + (b.x - a.x) * fminf(1, k * 1.6f), a.y + (b.y - a.y) * fminf(1, k * 1.6f) };
        DrawLineEx(a, hd, 10, Fade(C_GOLD, 0.35f));
        DrawLineEx(a, hd, 4, C_GOLD);
        DrawCircleV(hd, 14, Fade(WHITE, 0.8f));
        if (bt.lastHits == 2) DrawLineEx((Vector2){ a.x, a.y + 30 }, (Vector2){ hd.x, hd.y + 30 }, 4, C_GOLD);
    }

    // status boxes
    char s[200];
    Rectangle pb = { 30, 72, 440, 96 };
    DrawRectangleRounded(pb, 0.2f, 8, Fade(C_PANEL, 0.92f));
    snprintf(s, sizeof s, "You  |  Level %d", pl.level);
    Txt(fBold, s, pb.x + 18, pb.y + 12, 20, C_TEXT);
    Txt(fReg, "HP", pb.x + 18, pb.y + 44, 16, C_DIM);
    Bar(pb.x + 76, pb.y + 46, 250, 14, (float)pl.hp / maxHp(&pl), C_RED, C_PANEL3);
    snprintf(s, sizeof s, "%d/%d", pl.hp, maxHp(&pl));
    Txt(fReg, s, pb.x + 336, pb.y + 43, 17, C_TEXT);
    Txt(fReg, "Tokens", pb.x + 18, pb.y + 68, 16, C_DIM);
    Bar(pb.x + 76, pb.y + 70, 250, 14, (float)pl.tokens / maxTokens(&pl), C_BLUE, C_PANEL3);
    snprintf(s, sizeof s, "%d/%d", pl.tokens, maxTokens(&pl));
    Txt(fReg, s, pb.x + 336, pb.y + 67, 17, C_TEXT);
    EffectChips(40, 178);

    Rectangle mb = { VW - 470, 72, 440, 76 };
    DrawRectangleRounded(mb, 0.2f, 8, Fade(C_PANEL, 0.92f));
    Txt(fBold, foe->name, mb.x + 18, mb.y + 12, 21, bt.boss ? C_GOLD : C_TEXT);
    if (bt.boss) TxtR(fBold, "BOSS", mb.x + mb.width - 18, mb.y + 15, 16, C_GOLD);
    Bar(mb.x + 18, mb.y + 46, 300, 14, (float)bt.hp / bt.maxhp, C_GOLD, C_PANEL3);
    snprintf(s, sizeof s, "%d/%d", bt.hp, bt.maxhp);
    TxtR(fReg, s, mb.x + mb.width - 18, mb.y + 43, 17, C_TEXT);
    if (bt.turns == 1 && bt.state == BS_ASK) Bubble((Rectangle){ 700, 92, 400, 0 }, foe->taunt, 20, 1040, true);

    for (int i = 0; i < 8; i++) {
        Floater *f = &floaters[i];
        if (f->t <= 0) continue;
        f->t -= dt;
        float a = fminf(1, f->t * 2);
        TxtC(fBold, f->s, f->x, f->y - (1.4f - f->t) * 60, 34, Fade(f->c, a));
    }

    // question
    const Question *q = battleQuestion(&bt);
    Rectangle qc = { 30, 476, VW - 60, 112 };
    DrawRectangleRounded(qc, 0.14f, 8, C_PANEL);
    bool review = bt.qr != bt.region;
    if (review) snprintf(s, sizeof s, "REVIEW  |  Region %d: %s  |  %s", bt.qr + 1, REGIONS[bt.qr].name, REGIONS[bt.qr].topic);
    else snprintf(s, sizeof s, "REGION %d  |  %s", bt.qr + 1, REGIONS[bt.qr].topic);
    Txt(fBold, s, qc.x + 24, qc.y + 14, 16, review ? C_GOLD : RegionCol(bt.qr));
    float qs = FitSize(fBold, q->q, qc.width - 48, 72, 26, 19, 1.3f);
    Wrap(fBold, q->q, qc.x + 24, qc.y + 40, qc.width - 48, qs, qs * 1.3f, C_TEXT, true);

    bool ask = bt.state == BS_ASK && !confirmFlee && !blocked;
    for (int i = 0; i < 4; i++) {
        Rectangle r = { 30 + (i % 2) * ((VW - 60) / 2 + 8), 598 + (i / 2) * 82, (VW - 60) / 2 - 8, 74 };
        const char *o = q->opt[bt.order[i]];
        bool hid = bt.hidden[i], hov = ask && !hid && Hover(r);
        Color bg = hov ? C_PANEL3 : C_PANEL2, edge = hov ? C_GOLD : BLANK, tc = C_TEXT;
        if (bt.state == BS_FEEDBACK && !bt.rewound) {
            if (slotIsCorrect(&bt, i)) { bg = Fade(C_GREEN, 0.25f); edge = C_GREEN; }
            else if (i == bt.lastSlot) { bg = Fade(C_RED, 0.22f); edge = C_RED; }
            else tc = C_DIM;
        }
        if (hid) { bg = C_PANEL; tc = Fade(C_DIM, 0.5f); edge = BLANK; }
        DrawRectangleRounded(r, 0.2f, 8, bg);
        if (edge.a) DrawRectangleRoundedLinesEx(r, 0.2f, 8, 2, edge);
        char k[4];
        snprintf(k, sizeof k, "%d", i + 1);
        DrawRectangleRounded((Rectangle){ r.x + 14, r.y + 21, 32, 32 }, 0.3f, 6, hid ? C_PANEL2 : C_PANEL3);
        TxtC(fBold, k, r.x + 30, r.y + 26, 20, hid ? C_DIM : C_GOLD);
        float os = FitSize(fReg, o, r.width - 80, 56, 21, 16, 1.25f);
        float h = Wrap(fReg, o, 0, 0, r.width - 80, os, os * 1.25f, WHITE, false);
        Wrap(fReg, o, r.x + 62, r.y + (r.height - h) / 2 + 2, r.width - 80, os, os * 1.25f, tc, true);
        if (hid) DrawLineEx((Vector2){ r.x + 60, r.y + r.height / 2 }, (Vector2){ r.x + r.width - 20, r.y + r.height / 2 }, 2, Fade(C_DIM, 0.5f));
        if (ask && !hid && Clicked(r)) Answer(i);
    }
    for (int i = 0; i < 4 && ask; i++)
        if (IsKeyPressed(KEY_ONE + i) || IsKeyPressed(KEY_KP_1 + i)) Answer(i);

    // bottom: abilities or feedback
    Rectangle bb = { 30, 768, VW - 60, 118 };
    if (bt.state == BS_FEEDBACK) {
        Color c = bt.lastRight ? C_GREEN : bt.rewound ? C_BLUE : C_RED;
        DrawRectangleRounded(bb, 0.14f, 8, C_PANEL);
        DrawRectangleRounded((Rectangle){ bb.x, bb.y, 8, bb.height }, 0.5f, 4, c);
        char head[220];
        if (bt.lastRight) {
            snprintf(head, sizeof head, "Correct! %d damage%s%s%s", bt.lastDmg, bt.combo ? "  |  combo" : "",
                     bt.lastHits == 2 ? "  |  Agent Loop: 2 hits" : "", bt.lastHeal ? "  |  hook healed you" : "");
        } else if (bt.rewound) {
            snprintf(head, sizeof head, "Rewound! No damage. That answer is crossed out; try again.");
        } else {
            snprintf(head, sizeof head, "Not quite. %s hits you for %d.%s", foe->name, bt.lastTaken, bt.lastHeal ? " The hook heals a little." : "");
        }
        Txt(fBold, head, bb.x + 28, bb.y + 14, 22, c);
        float tw = bb.width - 330;
        if (!bt.rewound) {
            char why[700];
            if (bt.lastRight) snprintf(why, sizeof why, "%s", q->why);
            else snprintf(why, sizeof why, "Answer: %s.  %s", q->opt[q->ans], q->why);
            float ws = FitSize(fReg, why, tw, 66, 20, 15, 1.3f);
            Wrap(fReg, why, bb.x + 28, bb.y + 46, tw, ws, ws * 1.3f, C_TEXT, true);
        }
        bool cont = Button((Rectangle){ bb.x + bb.width - 270, bb.y + 12, 246, 54 }, "Continue  (Enter)", 21, true, true)
                 || (!blocked && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)));
        bool ctx = Button((Rectangle){ bb.x + bb.width - 270, bb.y + 74, 246, 36 }, "Context  (C)", 18, true, false);
        if (cont) Continue();
        else if (ctx) showCtx = true;
    } else if (bt.state == BS_ASK) {
        // abilities on the left, the Context tile on the right
        Rectangle ct = { bb.x + bb.width - 150, bb.y + 14, 150, bb.height - 14 };
        Rectangle ab = { bb.x, bb.y, bb.width - 162, bb.height };
        if (ContextTile(ct, !confirmFlee)) showCtx = true;
        int learned = 0;
        for (int a = 0; a < NREGIONS; a++) learned += hasAbility(&pl, a);
        if (!learned) {
            DrawRectangleRounded(ab, 0.14f, 8, C_PANEL);
            Txt(fReg, "Abilities appear here. Each mentor teaches one when you finish their lessons.", ab.x + 28, ab.y + 46, 20, C_DIM);
        } else {
            float gap = 8, w = fminf(200, (ab.width - gap * (learned - 1)) / learned), x = ab.x;
            int hovA = -1;
            for (int a = 0; a < NREGIONS; a++) {
                if (!hasAbility(&pl, a)) continue;
                Rectangle r = { x, bb.y + 14, w, bb.height - 14 };
                int cost = abilityCost(&bt, a);
                bool afford = pl.tokens >= cost, hov = ask && Hover(r);
                if (hov) hovA = a;
                DrawRectangleRounded(r, 0.18f, 8, hov ? C_PANEL3 : C_PANEL2);
                DrawRectangleRounded((Rectangle){ r.x, r.y, r.width, 6 }, 0.5f, 4, RegionCol(a));
                if (hov) DrawRectangleRoundedLinesEx(r, 0.18f, 8, 2, C_GOLD);
                float ns = 18;
                while (ns > 11 && TxtW(fBold, REGIONS[a].ability.name, ns) > w - 14) ns -= 1;
                TxtC(fBold, REGIONS[a].ability.name, r.x + w / 2, r.y + 24, ns, afford ? C_TEXT : C_DIM);
                snprintf(s, sizeof s, "%c  |  %d tokens", AB_KEYNAMES[a], cost);
                float ks = 16;
                while (ks > 11 && TxtW(fReg, s, ks) > w - 12) ks -= 1;
                TxtC(fReg, s, r.x + w / 2, r.y + 60, ks, afford ? C_BLUE : C_DIM);
                if (ask && Clicked(r)) UseAbility(a);
                x += w + gap;
            }
            if (hovA >= 0) {
                snprintf(s, sizeof s, "%s: %s", REGIONS[hovA].ability.name, REGIONS[hovA].ability.desc);
                float w2 = TxtW(fReg, s, 18) + 32;
                DrawRectangleRounded((Rectangle){ bb.x, bb.y - 40, w2, 36 }, 0.4f, 6, C_PARCH);
                Txt(fReg, s, bb.x + 16, bb.y - 31, 18, C_INK);
            }
        }
        for (int a = 0; a < NREGIONS && ask; a++)
            if (IsKeyPressed(AB_KEYS[a])) {
                if (hasAbility(&pl, a)) UseAbility(a);
                else Flash("You haven't learned that ability yet.");
            }
    }

    bool fighting = bt.state == BS_ASK || bt.state == BS_FEEDBACK;
    if (fighting && !blocked && !confirmFlee && IsKeyPressed(KEY_C)) showCtx = true;
    if (bt.state == BS_ASK && !confirmFlee && !blocked && IsKeyPressed(KEY_ESCAPE)) confirmFlee = true;
    else if (confirmFlee) {
        DrawRectangle(0, 0, VW, VH, Fade(BLACK, 0.55f));
        Rectangle p = { VW / 2 - 320, 300, 640, 220 };
        DrawRectangleRounded(p, 0.12f, 8, C_PANEL);
        TxtC(fBold, "Retreat from this battle?", VW / 2, p.y + 34, 30, C_TEXT);
        TxtC(fReg, "You keep your HP and tokens but earn no XP.", VW / 2, p.y + 80, 20, C_DIM);
        bool yes = Button((Rectangle){ p.x + 80, p.y + 130, 220, 56 }, "Retreat", 22, true, false);
        bool no = Button((Rectangle){ p.x + 340, p.y + 130, 220, 56 }, "Keep fighting", 22, true, true);
        if (yes || IsKeyPressed(KEY_Y)) { confirmFlee = false; state = ST_REGION; Save(); }
        else if (no || IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) confirmFlee = false;
    }

    if (showCtx) {
        clickNow = realClick;
        mouse = realMouse;
        DrawContext(ctxWasOpen && !showHelp);
    }

    if (bt.state == BS_WON || bt.state == BS_LOST) DrawBattleOverlay();

    if (monHurtT > 0) monHurtT -= dt;
    if (heroHurtT > 0) heroHurtT -= dt;
    if (boltT > 0) boltT -= dt;
    if (allyT > 0) allyT -= dt;
    if (lungeT > 0) lungeT -= dt;
}

// ---------------------------------------------------------------- codex

static void BeginClip(Rectangle r) {
    BeginScissorMode((int)(cam.offset.x + r.x * cam.zoom), (int)(cam.offset.y + r.y * cam.zoom), (int)(r.width * cam.zoom), (int)(r.height * cam.zoom));
}

static void DrawCodex(void) {
    DrawRectangle(0, HUD_H, VW, VH - HUD_H, C_BG);
    Txt(fBold, "Codex", 40, 78, 40, C_GOLD);
    const char *tabs[3] = { "Lessons", "Mistakes", "Progress" };
    for (int i = 0; i < 3; i++) {
        if (Button((Rectangle){ 300 + i * 190, 78, 176, 46 }, tabs[i], 20, true, codexTab == i)) { codexTab = i; codexScroll = 0; }
        if (IsKeyPressed(KEY_ONE + i)) { codexTab = i; codexScroll = 0; }
    }
    if (IsKeyPressed(KEY_TAB)) { codexTab = (codexTab + 1) % 3; codexScroll = 0; }
    TxtR(fReg, "1 2 3 or Tab: switch tabs   |   Wheel or Up/Down: scroll   |   Esc: close", VW - 40, 92, 17, C_DIM);

    Rectangle area = { 40, 144, VW - 80, VH - 170 };
    codexScroll -= GetMouseWheelMove() * 60;
    if (IsKeyDown(KEY_DOWN)) codexScroll += 14;
    if (IsKeyDown(KEY_UP)) codexScroll -= 14;
    char s[400];

    if (codexTab == 0) {
        if (codexRegion > pl.unlocked) codexRegion = pl.unlocked;
        for (int r = 0; r <= pl.unlocked; r++) {
            Rectangle b = { area.x, area.y + r * 62, 340, 54 };
            bool sel = r == codexRegion;
            DrawRectangleRounded(b, 0.2f, 8, sel ? C_PANEL3 : C_PANEL);
            DrawRectangle((int)b.x, (int)b.y + 10, 5, 34, RegionCol(r));
            snprintf(s, sizeof s, "%d. %s", r + 1, REGIONS[r].name);
            Txt(fBold, s, b.x + 20, b.y + 9, 19, sel ? C_TEXT : C_DIM);
            snprintf(s, sizeof s, "%s  |  %d/%d read", REGIONS[r].topic, pl.pagesRead[r], NPAGES);
            Txt(fReg, s, b.x + 20, b.y + 31, 15, C_DIM);
            if (Clicked(b)) { codexRegion = r; codexScroll = 0; }
        }
        if (IsKeyPressed(KEY_RIGHT) && codexRegion < pl.unlocked) { codexRegion++; codexScroll = 0; }
        if (IsKeyPressed(KEY_LEFT) && codexRegion > 0) { codexRegion--; codexScroll = 0; }
        Rectangle txt = { area.x + 370, area.y, area.width - 370, area.height };
        DrawRectangleRounded(txt, 0.03f, 8, C_PARCH);
        BeginClip(txt);
        float y = txt.y + 28 - codexScroll, y0 = y;
        const Region *R = &REGIONS[codexRegion];
        Txt(fBold, R->name, txt.x + 40, y, 34, C_INK); y += 46;
        snprintf(s, sizeof s, "%s  |  taught by %s", R->topic, R->mentor);
        Txt(fReg, s, txt.x + 40, y, 20, C_INK2); y += 50;
        for (int i = 0; i < NPAGES; i++) {
            if (i >= pl.pagesRead[codexRegion]) {
                snprintf(s, sizeof s, "Lesson %d: not read yet. Visit %s.", i + 1, R->mentor);
                Txt(fReg, s, txt.x + 40, y, 21, C_INK2); y += 40;
                continue;
            }
            Txt(fBold, R->pages[i].title, txt.x + 40, y, 26, C_INK); y += 38;
            y += Wrap(fReg, R->pages[i].body, txt.x + 40, y, txt.width - 80, 21, 31, C_INK, true) + 24;
        }
        EndScissorMode();
        codexMax = fmaxf(0, (y + codexScroll - y0) - txt.height + 60);
    } else if (codexTab == 1) {
        int idx[NREGIONS * NQ], n = 0;
        for (int r = 0; r < NREGIONS; r++)
            for (int i = 0; i < NQ; i++)
                if (pl.wrong[r][i]) idx[n++] = r * NQ + i;
        for (int i = 1; i < n; i++) {             // most-missed first
            int v = idx[i], j = i;
            while (j > 0 && pl.wrong[idx[j - 1] / NQ][idx[j - 1] % NQ] < pl.wrong[v / NQ][v % NQ]) { idx[j] = idx[j - 1]; j--; }
            idx[j] = v;
        }
        BeginClip(area);
        float y = area.y - codexScroll, y0 = y;
        if (!n) Wrap(fReg, "No mistakes yet. Every question you miss in battle lands here with its correct answer and explanation, and it will come back more often in future battles until you get it right.", area.x, y + 10, 1000, 22, 32, C_DIM, true);
        for (int k = 0; k < n; k++) {
            int r = idx[k] / NQ, i = idx[k] % NQ;
            const Question *q = &REGIONS[r].qs[i];
            float h = 0;
            h += Wrap(fBold, q->q, 0, 0, area.width - 60, 21, 28, WHITE, false);
            snprintf(s, sizeof s, "Answer: %s", q->opt[q->ans]);
            h += Wrap(fBold, s, 0, 0, area.width - 60, 19, 26, WHITE, false);
            h += Wrap(fReg, q->why, 0, 0, area.width - 60, 18, 25, WHITE, false);
            Rectangle b = { area.x, y, area.width, h + 60 };
            DrawRectangleRounded(b, 0.06f, 8, C_PANEL);
            DrawRectangle((int)b.x, (int)b.y + 12, 5, (int)b.height - 24, RegionCol(r));
            snprintf(s, sizeof s, "%s  |  missed %d, right %d", REGIONS[r].topic, pl.wrong[r][i], pl.right[r][i]);
            Txt(fBold, s, b.x + 30, b.y + 12, 15, pl.right[r][i] > pl.wrong[r][i] ? C_GREEN : C_GOLD);
            float yy = b.y + 36;
            yy += Wrap(fBold, q->q, b.x + 30, yy, area.width - 60, 21, 28, C_TEXT, true);
            snprintf(s, sizeof s, "Answer: %s", q->opt[q->ans]);
            yy += Wrap(fBold, s, b.x + 30, yy, area.width - 60, 19, 26, C_GREEN, true);
            Wrap(fReg, q->why, b.x + 30, yy, area.width - 60, 18, 25, C_DIM, true);
            y += b.height + 12;
        }
        EndScissorMode();
        codexMax = fmaxf(0, (y + codexScroll - y0) - area.height);
    } else {
        BeginClip(area);
        float y = area.y - codexScroll, y0 = y;
        snprintf(s, sizeof s, "Level %d   |   %d battles   |   %d of %d answers correct (%d%%)", pl.level, pl.battles, pl.correct, pl.answered,
                 pl.answered ? pl.correct * 100 / pl.answered : 0);
        Txt(fBold, s, area.x, y, 24, C_TEXT);
        y += 52;
        const char *hd[6] = { "Region", "Lessons", "Wins", "Boss", "Questions seen", "Accuracy" };
        const float cx[6] = { 0, 470, 610, 720, 880, 1110 };
        for (int i = 0; i < 6; i++) Txt(fBold, hd[i], area.x + cx[i], y, 17, C_DIM);
        y += 34;
        for (int r = 0; r < NREGIONS; r++) {
            bool open = r <= pl.unlocked;
            int rt = 0, wr = 0, seen = 0;
            for (int i = 0; i < NQ; i++) { rt += pl.right[r][i]; wr += pl.wrong[r][i]; seen += (pl.right[r][i] + pl.wrong[r][i]) > 0; }
            DrawRectangleRounded((Rectangle){ area.x - 12, y - 8, area.width + 12, 48 }, 0.3f, 6, r % 2 ? C_BG : C_PANEL);
            DrawRectangle((int)area.x - 4, (int)y + 2, 5, 22, open ? RegionCol(r) : C_PANEL3);
            snprintf(s, sizeof s, "%d. %s", r + 1, open ? REGIONS[r].name : "???");
            Txt(fBold, s, area.x + cx[0] + 12, y, 20, open ? C_TEXT : C_DIM);
            if (open) {
                snprintf(s, sizeof s, "%d/%d", pl.pagesRead[r], NPAGES); Txt(fReg, s, area.x + cx[1], y, 20, C_TEXT);
                snprintf(s, sizeof s, "%d", pl.wins[r]); Txt(fReg, s, area.x + cx[2], y, 20, C_TEXT);
                Txt(fReg, pl.bossBeaten[r] ? "Defeated" : "-", area.x + cx[3], y, 20, pl.bossBeaten[r] ? C_GREEN : C_DIM);
                snprintf(s, sizeof s, "%d/%d", seen, NQ); Txt(fReg, s, area.x + cx[4], y, 20, C_TEXT);
                float acc = rt + wr ? (float)rt / (rt + wr) : 0;
                Bar(area.x + cx[5], y + 4, 260, 16, acc, acc >= 0.8f ? C_GREEN : acc >= 0.5f ? C_GOLD : C_RED, C_PANEL3);
                snprintf(s, sizeof s, rt + wr ? "%d%%" : "-", (int)(acc * 100 + 0.5f));
                Txt(fReg, s, area.x + cx[5] + 274, y, 20, C_TEXT);
            }
            y += 54;
        }
        y += 20;
        Wrap(fReg, "Battles mix in review questions from earlier regions (30% of normal battles, 40% of bosses, half of the final boss). Questions you have missed come back more often until you get them right.",
             area.x, y, 1100, 19, 27, C_DIM, true);
        y += 70;
        EndScissorMode();
        codexMax = fmaxf(0, (y + codexScroll - y0) - area.height);
    }
    if (codexScroll > codexMax) codexScroll = codexMax;
    if (codexScroll < 0) codexScroll = 0;
}

// ---------------------------------------------------------------- help & ending

static void DrawHelp(bool input) {
    DrawRectangle(0, 0, VW, VH, Fade(BLACK, 0.7f));
    Rectangle p = { 180, 100, VW - 360, 700 };
    DrawRectangleRounded(p, 0.05f, 8, C_PANEL);
    DrawRectangleRoundedLinesEx(p, 0.05f, 8, 2, C_GOLD);
    TxtC(fBold, "How to play", VW / 2, p.y + 30, 40, C_GOLD);
    const char *how =
        "You are an apprentice learning Claude, from your first prompt to building agents. There are ten regions, and each one builds on the last.\n\n"
        "In each region: read the mentor's five lessons (this teaches you an ability), win at least two battles, then defeat the boss to open the road to the next region.\n\n"
        "Battles are questions. A correct answer strikes the foe; a wrong one lets it hit you, and shows you the right answer and why. Three correct in a row is a combo. Battles also bring back questions from earlier regions, and anything you have missed comes back more often.\n\n"
        "Stuck on a question? Press C or the Context button for background on the ideas it uses. It is free, before or after you answer.\n\n"
        "Abilities cost tokens. Correct answers restore 3 tokens, and the inn restores everything. Each ability is modeled on the feature it is named after: Clarify, Extended Thinking, Version History, Web Search, /rewind, Load Skill, PostToolUse Hook, Spawn Subagent, Prompt Cache and Agent Loop.";
    Wrap(fReg, how, p.x + 60, p.y + 100, p.width - 120, 22, 31, C_TEXT, true);
    const char *keys[][2] = {
        { "1 - 4", "answer" }, { "Q W E R T Y U I O P", "abilities, one per region" }, { "Enter / Space", "confirm, continue" },
        { "C", "context for the question" }, { "Arrow keys", "move, page, scroll" },
        { "J", "Codex: lessons, mistakes, progress" }, { "H", "this help" }, { "Esc", "back, retreat from battle" },
    };
    for (int i = 0; i < 8; i++) {
        float x = p.x + 60 + (i % 4) * 300, y = p.y + p.height - 150 + (i / 4) * 56;
        Txt(fBold, keys[i][0], x, y, 20, C_GOLD);
        Txt(fReg, keys[i][1], x, y + 24, 18, C_DIM);
    }
    TxtC(fReg, "Press H, Esc or click to close", VW / 2, p.y + p.height - 40, 17, C_DIM);
    if (input && (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_ESCAPE) || clickNow)) showHelp = false;
}

static void DrawEnding(void) {
    DrawRectangleGradientV(0, 0, VW, VH, (Color){ 40, 28, 52, 255 }, C_BG);
    Stars(200, 0, 0, VW, VH, 99);
    TxtC(fBold, "The Hallucination Dragon falls.", VW / 2, 110, 56, C_GOLD);
    TxtC(fReg, "You walked the whole road, from a first clear prompt to agents that check their own work.", VW / 2, 196, 26, C_TEXT);
    for (int i = 0; i < NREGIONS; i++) {
        float x = 170 + (i % 5) * 290, y = 280 + (i / 5) * 170;
        DrawCircleV((Vector2){ x, y }, 34, RegionCol(i));
        char n[4];
        snprintf(n, sizeof n, "%d", i + 1);
        TxtC(fBold, n, x, y - 17, 32, C_BG);
        TxtC(fBold, REGIONS[i].topic, x, y + 46, 20, C_TEXT);
        TxtC(fReg, REGIONS[i].ability.name, x, y + 74, 17, C_DIM);
    }
    char s[200];
    snprintf(s, sizeof s, "Level %d   |   %d battles   |   %d%% of answers correct", pl.level, pl.battles, pl.answered ? pl.correct * 100 / pl.answered : 0);
    TxtC(fBold, s, VW / 2, 640, 26, C_TEXT);
    TxtC(fReg, "Keep playing: every region stays open for rematches, and the Codex keeps your notes and mistakes.", VW / 2, 690, 21, C_DIM);
    DrawFigure(220, 860, 1.2f, 1, HeroLook(), 0);
    DrawMonster(VW - 220, 790, 60, 39, Fade(FoeCol(9, 3), 0.35f), false, 0);
    if (Button((Rectangle){ VW / 2 - 140, 760, 280, 60 }, "Back to the map", 22, true, true) || IsKeyPressed(KEY_ENTER)) state = ST_MAP;
}

// ---------------------------------------------------------------- screenshot mode

static int shotStep = -1;

static void FakeProgress(int upto) {
    playerNew(&pl);
    pl.level = 1 + upto + upto / 2;
    pl.unlocked = upto;
    for (int r = 0; r < upto; r++) { pl.pagesRead[r] = NPAGES; pl.wins[r] = 3; pl.bossBeaten[r] = true; }
    pl.pagesRead[upto] = 2;
    pl.wins[upto] = 1;
    for (int i = 0; i < NQ; i++) { pl.right[0][i] = 2; pl.right[1][i] = i % 3; pl.wrong[1][i] = i % 4 == 0; pl.wrong[2][i] = i % 5 == 1; }
    pl.answered = 80; pl.correct = 64; pl.battles = 14;
    pl.hp = maxHp(&pl) - 30;
    pl.tokens = maxTokens(&pl) - 8;
}

// returns false when finished
static bool ShotSetup(int step) {
    showHelp = false;
    switch (step) {
    case 0: state = ST_TITLE; haveSave = true; break;
    case 1: FakeProgress(3); cur = 3; heroMapPos = NODES[3]; state = ST_MAP; break;
    case 2: state = ST_REGION; menuSel = 1; break;
    case 3: state = ST_LESSON; lessonHadAbility = false; lessonPage = 1; break;
    case 4: pl.pagesRead[3] = NPAGES; StartBattle(false); { char m[100]; battleUseAbility(&bt, &pl, AB_THINK, m, 100); } break;
    case 5: for (int i = 0; i < 4; i++) if (!slotIsCorrect(&bt, i)) { Answer(i); break; } break;
    case 6: Continue(); for (int i = 0; i < 4; i++) if (slotIsCorrect(&bt, i)) { Answer(i); break; } break;
    case 7: bt.hp = 0; bt.state = BS_FEEDBACK; Continue(); break;
    case 8: state = ST_CODEX; codexTab = 0; codexRegion = 1; break;
    case 9: codexTab = 1; break;
    case 10: codexTab = 2; break;
    case 11: state = ST_MAP; showHelp = true; break;
    case 12: FakeProgress(9); for (int r = 0; r < NREGIONS; r++) pl.pagesRead[r] = NPAGES; pl.wins[9] = 2; cur = 9; StartBattle(true); break;
    case 13: { char m[100]; battleUseAbility(&bt, &pl, AB_SUBAGENT, m, 100); battleUseAbility(&bt, &pl, AB_CLARIFY, m, 100); } break;
    case 14: state = ST_ENDING; break;
    case 15:                                   // Context panel on the <contract> question
        FakeProgress(0); pl.pagesRead[0] = NPAGES; cur = 0; StartBattle(false);
        bt.qr = 0; bt.qi = 3;
        for (int i = 0; i < 4; i++) bt.hidden[i] = false;
        showCtx = true;
        break;
    default: return false;
    }
    return true;
}

// ---------------------------------------------------------------- main

static Font TryLoadFont(const char *name, int size, int *cps, int ncp, bool *ok) {
    char path[1100];
    snprintf(path, sizeof path, "%sassets/%s", GetApplicationDirectory(), name);
    if (!FileExists(path)) snprintf(path, sizeof path, "assets/%s", name);
    if (FileExists(path)) {
        Font f = LoadFontEx(path, size, cps, ncp);
        if (f.texture.id != 0 && f.glyphCount > 0) {
            GenTextureMipmaps(&f.texture);
            SetTextureFilter(f.texture, TEXTURE_FILTER_TRILINEAR);
            *ok = true;
            return f;
        }
    }
    *ok = false;
    return GetFontDefault();
}

int main(int argc, char **argv) {
    const char *shotDir = NULL;
    if (argc == 3 && strcmp(argv[1], "--shots") == 0) { shotDir = argv[2]; noSave = true; shotStep = 0; }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(1440, 810, "Claude Quest");
    SetWindowMinSize(800, 450);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    srand((unsigned)(GetTime() * 1e6) ^ 0x5A5Au);

    bool ok1, ok2;
    int ui[110], nui = 0;
    for (int c = 32; c < 127; c++) ui[nui++] = c;
    fBold = TryLoadFont("ui_bold.otf", 72, ui, nui, &ok1);
    fReg = TryLoadFont("ui_regular.otf", 56, ui, nui, &ok2);
    hasUI = ok1 && ok2;
    if (!ok2) fReg = fBold;

    snprintf(savePath, sizeof savePath, "%ssave.txt", GetApplicationDirectory());
    playerNew(&pl);
    if (!noSave && loadGame(&pl, savePath)) { haveSave = true; cur = pl.unlocked; }

    bool quit = false;
    int frame = 0;
    while (!WindowShouldClose() && !quit) {
        float dt = GetFrameTime();
        animT += dt;
        if (flashT > 0) flashT -= dt;
        float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
        float zoom = fminf(sw / VW, sh / VH);
        cam = (Camera2D){ .offset = { (sw - VW * zoom) / 2, (sh - VH * zoom) / 2 }, .zoom = zoom };
        mouse = GetScreenToWorld2D(GetMousePosition(), cam);
        clickNow = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (shotDir) {
            clickNow = false;
            mouse = (Vector2){ -100, -100 };
            if (frame % 30 == 0 && !ShotSetup(frame / 30)) break;
        }

        bool helpWasOpen = showHelp;

        // global keys (the overlays and the battle handle Esc themselves)
        if (!showHelp && !confirmNew) {
            bool inFight = state == ST_BATTLE && (bt.state == BS_ASK || bt.state == BS_FEEDBACK);
            if (IsKeyPressed(KEY_H) && state != ST_TITLE) showHelp = true;
            else if (IsKeyPressed(KEY_J) && state != ST_TITLE && state != ST_CODEX && !inFight) {
                codexReturn = state; state = ST_CODEX; codexRegion = cur; codexScroll = 0;
            } else if (IsKeyPressed(KEY_ESCAPE) || (state == ST_CODEX && IsKeyPressed(KEY_J))) {
                if (state == ST_CODEX) state = codexReturn;
                else if (state == ST_LESSON) { state = ST_REGION; Save(); }
                else if (state == ST_REGION) state = ST_MAP;
                else if (state == ST_MAP) state = ST_TITLE;
            }
        }
        bool frozen = showHelp;                   // help swallows input for the screen underneath
        if (frozen) { clickNow = false; mouse = (Vector2){ -100, -100 }; }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(cam);
        DrawRectangle(0, 0, VW, VH, C_BG);
        switch (state) {
        case ST_TITLE: DrawTitle(&quit); break;
        case ST_MAP: DrawMap(); DrawHud(); break;
        case ST_REGION: DrawRegion(); DrawHud(); break;
        case ST_LESSON: DrawLesson(); DrawHud(); break;
        case ST_BATTLE: DrawBattle(); DrawHud(); break;
        case ST_CODEX: DrawCodex(); DrawHud(); break;
        case ST_ENDING: DrawEnding(); break;
        }
        if (flashT > 0 && flashMsg[0]) {
            float w = fminf(TxtW(fBold, flashMsg, 19) + 40, VW - 80);
            float a = flashT > 0.5f ? 1 : flashT * 2;
            Rectangle b = { VW / 2 - w / 2, HUD_H + 14, w, 38 };
            DrawRectangleRounded(b, 0.5f, 8, Fade((Color){ 20, 22, 32, 255 }, 0.95f * a));
            DrawRectangleRoundedLinesEx(b, 0.5f, 8, 1.5f, Fade(C_GOLD, a));
            TxtC(fBold, flashMsg, VW / 2, b.y + 9, 19, Fade(C_TEXT, a));
        }
        if (showHelp) {
            clickNow = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !shotDir;
            DrawHelp(helpWasOpen);
        }
        EndMode2D();
        EndDrawing();

        if (shotDir && frame % 30 == 25) {
            char name[64];
            snprintf(name, sizeof name, "shot_%02d.png", frame / 30);
            TakeScreenshot(name);             // raylib writes next to the executable
            char from[1200], to[1200];
            snprintf(from, sizeof from, "%s%s", GetApplicationDirectory(), name);
            snprintf(to, sizeof to, "%s/%s", shotDir, name);
            rename(from, to);
        }
        frame++;
    }
    CloseWindow();
    return 0;
}
