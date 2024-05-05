/*
 * Heat diffusion (Jacobi-type iteration)
 *
 * Usage: see function usage();
 * 
 * Volker Strumpen, Boston                                 August 1996
 *
 * Copyright (c) 1996 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cilk/cilk.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <sys/time.h>
#include <chrono>
#include <iostream>


extern int errno;


/* Define ERROR_SUMMARY if you want to check your numerical results */
#undef ERROR_SUMMARY

unsigned long long todval (struct timeval *tp) {
    return tp->tv_sec * 1000 * 1000 + tp->tv_usec;
}

#define f(x,y)     (sin(x)*sin(y))
#define randa(x,t) (0.0)
#define randb(x,t) (exp(-2*(t))*sin(x))
#define randc(y,t) (0.0)
#define randd(y,t) (exp(-2*(t))*sin(y))
#define solu(x,y,t) (exp(-2*(t))*sin(x)*sin(y))

/* apparently register storage specifier is deprecated */
#define register

int nx, ny, nt;
double xu, xo, yu, yo, tu, to;
double dx, dy, dt;

double dtdxsq, dtdysq;
double t;

int leafmaxcol;


/*****************   Allocation of grid partition  ********************/

void allcgrid(double **neww, double **old, int lb, int ub) {

  int j;
  double **rne, **rol;

  for (j=lb, rol=old+lb, rne=neww+lb; j < ub; j++, rol++, rne++) {
    *rol = (double *) malloc(ny * sizeof(double));
    *rne = (double *) malloc(ny * sizeof(double));
  }
}

/*****************   Initialization of grid partition  ********************/

void initgrid(double **old, int lb, int ub) {

  int a, b, llb, lub;

  llb = (lb == 0) ? 1 : lb;
  lub = (ub == nx) ? nx - 1 : ub;

  for (a=llb, b=0; a < lub; a++)		/* boundary nodes */
    old[a][b] = randa(xu + a * dx, 0);

  for (a=llb, b=ny-1; a < lub; a++)
    old[a][b] = randb(xu + a * dx, 0);

  if (lb == 0) {
    for (a=0, b=0; b < ny; b++)
      old[a][b] = randc(yu + b * dy, 0);
  }
  if (ub == nx) {
    for (a=nx-1, b=0; b < ny; b++)
      old[a][b] = randd(yu + b * dy, 0);
  }
  for (a=llb; a < lub; a++) {	/* inner nodes */
    for (b=1; b < ny-1; b++) {
      old[a][b] = f(xu + a * dx, yu + b * dy);
    }
  }
}


/***************** Five-Point-Stencil Computation ********************/

void compstripe(register double **neww, register double **old, int lb, int ub) {

  register int a, b, llb, lub;

  llb = (lb == 0) ? 1 : lb;
  lub = (ub == nx) ? nx - 1 : ub;

  for (a=llb; a < lub; a++) {
    for (b=1; b < ny-1; b++) {
      neww[a][b] =   dtdxsq * (old[a+1][b] - 2 * old[a][b] + old[a-1][b])
        + dtdysq * (old[a][b+1] - 2 * old[a][b] + old[a][b-1])
        + old[a][b];
    }
  }

  for (a=llb, b=ny-1; a < lub; a++)
    neww[a][b] = randb(xu + a * dx, t);

  for (a=llb, b=0; a < lub; a++)
    neww[a][b] = randa(xu + a * dx, t);

  if (lb == 0) {
    for (a=0, b=0; b < ny; b++)
      neww[a][b] = randc(yu + b * dy, t);
  }
  if (ub == nx) {
    for (a=nx-1, b=0; b < ny; b++)
      neww[a][b] = randd(yu + b * dy, t);
  }
}


/***************** Decomposition of 2D grids in stripes ********************/

#define ALLC       0
#define INIT       1
#define COMP       2

int divide(int lb, int ub, double **neww, 
           double **old, int mode, int timestep){

  if (ub - lb > leafmaxcol) {
    int _tmp;
    int l = 0, r;
    l = cilk_spawn divide(lb, (ub + lb) / 2, neww, old, mode, timestep);
    r = divide((ub + lb) / 2, ub, neww, old, mode, timestep);
    cilk_sync;

    _tmp = l + r;
    return _tmp;

  } else {
    switch (mode) {
      case COMP:
        if (timestep % 2)
          compstripe(neww, old, lb, ub);
        else
          compstripe(old, neww, lb, ub);
        return 1;

      case ALLC:
        allcgrid(neww, old, lb, ub);
        return 1;

      case INIT:
        initgrid(old, lb, ub);
        return 1;
    }

    return 0;
  }
}

int heat(void) {

  double **old, **neww;
  int  c, l;

#ifdef ERROR_SUMMARY  
    double tmp, **mat;
  double mae = 0.0;
  double mre = 0.0;
  double me = 0.0;
  int a, b;
#endif

  /* Memory Allocation */
  old = (double **) malloc(nx * sizeof(double *));
  neww = (double **) malloc(nx * sizeof(double *));

  l = divide(0, nx, neww, old, ALLC, 0);
  /* 
   * Sequential allocation might be faster than parallel!
   * Substitute the following call for the preceding divide:
   *
   * allcgrid(new, old, 0, nx);
   */

  /* Initialization */
  // l = divide(0, nx, new, old, INIT, 0);

  /* Jacobi Iteration (divide x-dimension of 2D grid into stripes) */

  l = divide(0, nx, neww, old, INIT, 0);

  for (c = 1; c <= nt; c++) {
    t = tu + c * dt;
    l = divide(0, nx, neww, old, COMP, c);
  }

#ifdef ERROR_SUMMARY  
  /* Error summary computation: Not parallelized! */
  mat = (c % 2) ? old : neww;
  printf("\n Error summary of last time frame comparing with exact solution:");
  for (a=0; a<nx; a++)
    for (b=0; b<ny; b++) {
      tmp = fabs(mat[a][b] - solu(xu + a * dx, yu + b * dy, to));
      if (tmp > mae)
        mae = tmp;
    }

  printf("\n   Local maximal absolute error  %10e ", mae);

  for (a=0; a<nx; a++)
    for (b=0; b<ny; b++) {
      tmp = fabs(mat[a][b] - solu(xu + a * dx, yu + b * dy, to));
      if (mat[a][b] != 0.0)
        tmp = tmp / mat[a][b];
      if (tmp > mre)
        mre = tmp;
    }

  printf("\n   Local maximal relative error  %10e %s ", mre * 100, "%");

  me = 0.0;
  for (a=0; a<nx; a++)
    for (b=0; b<ny; b++) {
      me += fabs(mat[a][b] - solu(xu + a * dx, yu + b * dy, to));
    }

  me = me / (nx * ny);
  printf("\n   Global Mean absolute error    %10e\n\n", me);
#endif

  return 0;
}



int main(int argc, char *argv[]) { 

  int ret, benchmark, help;

  nx = 4096;
  ny = 1024;
  nt = 100;
  xu = 0.0;
  xo = 1.570796326794896558;
  yu = 0.0;
  yo = 1.570796326794896558;
  tu = 0.0;
  to = 0.0000001;
  leafmaxcol = 1;

  // use the math related function before parallel region;
  // there is some benigh race in initalization code for the math functions.



  dx = (xo - xu) / (nx - 1);
  dy = (yo - yu) / (ny - 1);
  dt = (to - tu) / nt;	/* nt effective time steps! */

  dtdxsq = dt / (dx * dx);
  dtdysq = dt / (dy * dy);


  auto start = std::chrono::steady_clock::now();
  ret = heat();
  auto end = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Heat's time: " << duration.count() << " ms" << std::endl;


  return 0;
}
