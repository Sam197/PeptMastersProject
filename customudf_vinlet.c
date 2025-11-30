/***********************************************************************/
/* pulsatile_adjustable.c                                              */
/* Easy-to-adjust Pulsatile Flow UDF                                   */
/***********************************************************************/

#include "udf.h"

/* =================================================================== */
/* USER ADJUSTABLE SETTINGS (CHANGE THESE)                  */
/* =================================================================== */

/* 1. HEART RATE */
/* Set the beats per minute. 60 BPM = 1 sec period. */
#define BPM 60.0   

/* 2. SYSTOLE RATIO */
/* Fraction of the heartbeat spent pumping (Systole). 
   e.g., 0.35 means 35% of the time is the pulse, 65% is resting. */
#define SYS_RATIO 0.35 

/* 3. AMPLITUDES */
/* If using Velocity Inlet: Units are m/s. 
   If using Pressure Inlet: Units are Pascals. */
#define MAX_AMPLITUDE 1.2  // The peak of the sine wave
#define MIN_AMPLITUDE 0.1  // The resting (diastolic) baseline

/* =================================================================== */
/* LOGIC SECTION (DO NOT EDIT BELOW)                        */
/* =================================================================== */

#define PI 3.141592654

DEFINE_PROFILE(pulsatile_inlet, th, i)
{
    face_t f;
    double period;
    double t_local;
    double systole_duration;
    double current_val;

    /* Calculate the period in seconds based on BPM */
    period = 60.0 / BPM;

    /* Calculate how long the "pulse" part lasts in seconds */
    systole_duration = period * SYS_RATIO;

    /* Find local time within the current cycle (0 to period) */
    t_local = fmod(CURRENT_TIME, period);

    begin_f_loop(f, th)
    {
        /* LOGIC: Are we in the Systole (pumping) phase? */
        if (t_local <= systole_duration)
        {
            /* We create a sine wave that goes from 0 to PI over the duration 
               of the systole.
               Formula: Base + (Height of wave * sin(scaled_time))
            */
            current_val = MIN_AMPLITUDE + (MAX_AMPLITUDE - MIN_AMPLITUDE) * sin((PI / systole_duration) * t_local);
        }
        else
        {
            /* We are in Diastole (resting) phase */
            current_val = MIN_AMPLITUDE;
        }

        /* Apply the value to the boundary face */
        F_PROFILE(f, th, i) = current_val;
    }
    end_f_loop(f, th);
}