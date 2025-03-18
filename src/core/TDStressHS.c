#include <stdio.h>
#include <stdlib.h>
#include "mehdi.h"

// Function to compute stresses and strains in an elastic half-space
void TDstressHS(double X, double Y, double Z, double P1[3], double P2[3], double P3[3],
                double Ss, double Ds, double Ts, double mu, double lambda,
                double Stress[6], double Strain[6]) {
    // Check if Z coordinates are positive (half-space condition)
    if (Z > 0 || P1[2] > 0 || P2[2] > 0 || P3[2] > 0) {
        fprintf(stderr, "Error: Half-space solution: Z coordinates must be negative!\n");
        // exit(1); // Exit with error (no exception handling in C like MATLAB)
        exit(EXIT_FAILURE);
    }

    // Calculate main dislocation contribution to strains and stresses
    double StsMS[6], StrMS[6];
    TDstressFS4HS(X, Y, Z, P1, P2, P3, Ss, Ds, Ts, mu, lambda, StsMS, StrMS);

    // Calculate harmonic function contribution to strains and stresses
    double StsFSC[6], StrFSC[6];
    TDstress_HarFunc(X, Y, Z, P1, P2, P3, Ss, Ds, Ts, mu, lambda, StsFSC, StrFSC);

    // Calculate image dislocation contribution to strains and stresses
    double P1_img[3] = {P1[0], P1[1], -P1[2]}; // Reflect P1 across the surface
    double P2_img[3] = {P2[0], P2[1], -P2[2]}; // Reflect P2 across the surface
    double P3_img[3] = {P3[0], P3[1], -P3[2]}; // Reflect P3 across the surface
    double StsIS[6], StrIS[6];
    TDstressFS4HS(X, Y, Z, P1_img, P2_img, P3_img, Ss, Ds, Ts, mu, lambda, StsIS, StrIS);

    // Adjust signs for image stresses and strains if the dislocation is at the surface
    if (P1_img[2] == 0.0 && P2_img[2] == 0.0 && P3_img[2] == 0.0) {
        StsIS[4] = -StsIS[4]; // Sxz
        StsIS[5] = -StsIS[5]; // Syz
        StrIS[4] = -StrIS[4]; // Exz
        StrIS[5] = -StrIS[5]; // Eyz
    }

    // Calculate the complete stress and strain tensor components in EFCS
    for (int i = 0; i < 6; i++) {
        Stress[i] = StsMS[i] + StsIS[i] + StsFSC[i];
        Strain[i] = StrMS[i] + StrIS[i] + StrFSC[i];
    }
}

