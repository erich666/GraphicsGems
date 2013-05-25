/*
 * Revisions to Spherical Polygon Area (GG vol.IV entry II.4 [page 136])
 * as described in GG vol.V entry II.2 (page 45, bottom; 46, top).
 */

/* OLD */
/*    if (Lam2 < Lam1) Excess = - Excess; */

/* NEW */
     double Lam;
     Lam = (Lam2 - Lam1 > 0) ? Lam2 - Lam1 : Lam2 - Lam1 + 4*HalfPi;
     if (Lam > 2*HalfPi) Excess= -Excess; 
