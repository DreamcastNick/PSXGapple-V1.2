#include "tween.h"

#include "timer.h"

// Array of active tweens and number of active tweens
FlxTween activeTweens[MAX_TWEENS];
int numActiveTweens = 0;

// Quad out easing in fixed-point space
fixed_t FlxEase_quadOut(fixed_t t, fixed_t b, fixed_t c, fixed_t d)
{
    (void)d; // We always use normalized [0..1] time

    // -c * t * (t - 2) + b
    fixed_t term = -FIXED_MUL(t, t - FIXED_DEC(2,1));
    return b + FIXED_MUL(c, term);
}

static fixed_t FlxTween_GetCurrentTime(void)
{
    return timer_sec;
}

// Function to initiate a tween for fixed-point values
void FlxTween_tweenFixed(fixed_t *target, fixed_t to, fixed_t duration, FlxEase ease)
{
    int i;

    if (target == NULL || ease == NULL)
        return;

    // Replace any existing tween driving this target so it can smoothly retarget.
    for (i = 0; i < numActiveTweens; ++i)
    {
        if (activeTweens[i].target == target)
        {
            for (; i < numActiveTweens - 1; ++i)
                activeTweens[i] = activeTweens[i + 1];
            numActiveTweens--;
            break;
        }
    }

    if (numActiveTweens < MAX_TWEENS)
    {
        FlxTween *tween = &activeTweens[numActiveTweens++];
        tween->target = target;
        tween->from = *target;
        tween->to = to;
        tween->duration = duration;
        tween->startTime = FlxTween_GetCurrentTime();
        tween->ease = ease;
    }
}

// Function to initiate a tween for rotation angle
void FlxTween_angle(fixed_t *angle, fixed_t to, fixed_t duration, FlxEase ease)
{
    FlxTween_tweenFixed(angle, to, duration, ease);
}

// Function to update active tweens
void updateTweens(void)
{
    fixed_t currentTime = FlxTween_GetCurrentTime();

    // Update active tweens
    for (int i = 0; i < numActiveTweens; ++i)
    {
        FlxTween *tween = &activeTweens[i];

        if (tween->duration == 0 || currentTime >= tween->startTime + tween->duration)
        {
            // Tween completed
            *tween->target = tween->to;

            // Remove tween from list
            for (int j = i; j < numActiveTweens - 1; ++j)
                activeTweens[j] = activeTweens[j + 1];

            numActiveTweens--;
            i--;
        }
        else
        {
            // Tween in progress (normalized 0..1 fixed-point)
            fixed_t elapsed = currentTime - tween->startTime;
            fixed_t t = FIXED_DIV(elapsed, tween->duration);
            *tween->target = tween->ease(t, tween->from, tween->to - tween->from, FIXED_UNIT);
        }
    }
}
