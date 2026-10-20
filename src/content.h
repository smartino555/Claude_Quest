// content.h - the curriculum: regions, lessons, questions, foes and abilities
#ifndef CONTENT_H
#define CONTENT_H

#define NREGIONS 10
#define NPAGES 5
#define NQ 48

typedef struct { const char *q; const char *opt[4]; int ans; const char *why; } Question;
typedef struct { const char *title; const char *body; } Page;
typedef struct { const char *name; const char *taunt; } Foe;
typedef struct { const char *name; const char *desc; int cost; } Ability;

typedef struct {
    const char *name;        // place on the map
    const char *topic;       // what the region teaches
    const char *mentor;
    const char *greeting;    // mentor's first line
    Page pages[NPAGES];
    Question qs[NQ];
    Foe foes[3];
    Foe boss;
    Ability ability;         // granted after the last lesson page
    unsigned char col[3];    // theme colour
} Region;

extern const Region REGIONS[NREGIONS];

// background for REGIONS[r].qs[i], shown by the Context button (context.c)
extern const char *const CONTEXT[NREGIONS][NQ];

#endif
