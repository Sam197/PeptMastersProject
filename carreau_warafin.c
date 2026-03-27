#include "udf.h"

/* Carreau Model Constants */
#define MU_INF 0.00274073  /* Infinite shear viscosity (Pa-s) */
#define MU_0   0.0087632    /* Zero shear viscosity (Pa-s)      */
#define LAMBDA 0.32100201    /* Relaxation time (s)              */
#define N_IND  0.4772361    /* Power law index                  */

DEFINE_PROPERTY(cell_viscosity_carreau, c, t)
{
    real mu;
    real shear_rate = C_STRAIN_RATE_MAG(c, t); /* Get local shear rate */

    /* The Carreau Equation */
    mu = MU_INF + (MU_0 - MU_INF) * pow(1.0 + pow(LAMBDA * shear_rate, 2.0), (N_IND - 1.0) / 2.0);

    return mu;
}