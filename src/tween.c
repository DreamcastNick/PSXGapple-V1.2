#include "tween.h"


// Array of active tweens and number of active tweens
FlxTween activeTweens[MAX_TWEENS];
int numActiveTweens = 0;

// Example easing function implementation
float FlxEase_quadOut(float t, float b, float c, float d) {
    return -c * (t /= d) * (t - 2) + b;
}

// Function to initiate a tween for fixed-point values
void FlxTween_tweenFixed(fixed_t *target, fixed_t to, u32 duration, FlxEase ease) {
    if (numActiveTweens < MAX_TWEENS) {
        FlxTween *tween = &activeTweens[numActiveTweens++];
        tween->target = target;
        tween->from = *target;
        tween->to = to;
        tween->duration = duration;
        tween->startTime = getCurrentTime(); // Replace with your function to get current time
        tween->ease = ease;
    }
}

// Function to initiate a tween for rotation angle
void FlxTween_angle(fixed_t *angle, fixed_t to, u32 duration, FlxEase ease) {
    FlxTween_tweenFixed(angle, to, duration, ease);
}

// Function to update active tweens
void updateTweens() {
    u32 currentTime = getCurrentTime(); // Replace with your function to get current time

    // Update active tweens
    for (int i = 0; i < numActiveTweens; ++i) {
        FlxTween *tween = &activeTweens[i];

        if (currentTime >= tween->startTime + tween->duration) {
            // Tween completed
            *tween->target = tween->to;
            // Remove tween from list or mark as inactive
            for (int j = i; j < numActiveTweens - 1; ++j) {
                activeTweens[j] = activeTweens[j + 1];
            }
            numActiveTweens--;
            i--;
        } else {
            // Tween in progress
            float t = (float)(currentTime - tween->startTime) / tween->duration;
            *tween->target = tween->ease(t, tween->from, tween->to - tween->from, 1.0f);
        }
    }
}
