#ifndef PSXF_GUARD_TWEEN_H
#define PSXF_GUARD_TWEEN_H

#include "fixed.h" // Include your fixed-point arithmetic definitions
#include "stage.h"
#include <stddef.h> // For NULL definition

// Example easing function type definition
typedef float (*FlxEase)(float, float, float, float);

// Define FlxTween structure for fixed-point values
typedef struct {
    fixed_t *target;
    fixed_t from;
    fixed_t to;
    u32 duration;
    u32 startTime;
    FlxEase ease;
} FlxTween;

// Define maximum number of active tweens
#define MAX_TWEENS 10

// Array of active tweens and number of active tweens
extern FlxTween activeTweens[MAX_TWEENS];
extern int numActiveTweens;

// Function prototypes
void FlxTween_tweenFixed(fixed_t *target, fixed_t to, u32 duration, FlxEase ease);
void FlxTween_angle(fixed_t *angle, fixed_t to, u32 duration, FlxEase ease);
void updateTweens(void);

#endif // TWEEN_H