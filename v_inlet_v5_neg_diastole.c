#include "udf.h"
#include <math.h>

/* =================================================================== */
/* USER ADJUSTABLE EXPERIMENTAL SETTINGS                               */
/* =================================================================== */

#define BPM 60.0              /* Beats Per Minute */
#define SYS_RATIO 0.4         /* Phase Ratio (Systole fraction) */
#define STROKE_VOLUME 80e-6   /* Stroke Volume in m^3 (80mL = 80e-6) */

/* CROSS-SECTIONAL AREA */
#define INLET_AREA 0.000244     /* Area of inlet in m^2. Update to match your mesh! */

/* BACKFLOW / CLOSING DYNAMICS */
#define T_BACKFLOW 0.1     /* Duration of backflow in seconds (35 ms) */
#define V_BACKFLOW 0.15       /* Peak reverse velocity in m/s */

/* =================================================================== */

#define PI 3.14159265358979323846

DEFINE_PROFILE(pulsatile_inlet_with_backflow, th, i)
{
    face_t f;
    real period, t_sys, v_max_forward, current_v;
    real t_local;

    period = 60.0 / BPM;
    t_sys = period * SYS_RATIO;
    
    /* Calculate peak forward velocity required to hit the target Stroke Volume */
    v_max_forward = (STROKE_VOLUME * PI) / (2.0 * INLET_AREA * t_sys);

    /* Local time within the current cardiac cycle */
    t_local = CURRENT_TIME - period * floor(CURRENT_TIME / period);

    if (t_local <= t_sys)
    {
        /* 1. Positive flow (Systole) */
        current_v = v_max_forward * sin((PI / t_sys) * t_local);
    }
    else if (t_local <= (t_sys + T_BACKFLOW))
    {
        /* 2. Negative flow (Valve Closing Backflow) */
        real t_back = t_local - t_sys;
        
        /* Smooth sine wave for backflow to prevent numerical shock */
        current_v = -V_BACKFLOW * sin((PI / T_BACKFLOW) * t_back);
    }
    else
    {
        /* 3. Rest of Diastole (Valve is closed) */
        current_v = -0.01;
    }

    /* Apply the velocity profile to the faces */
    begin_f_loop(f, th)
    {
        F_PROFILE(f, th, i) = current_v;
    }
    end_f_loop(f, th);
}