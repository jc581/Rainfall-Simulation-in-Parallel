#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Frac {
  int willTrickle;
  double up;
  double down;
  double left;
  double right;
};

double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
}

double min(double a, double b) {
  if (a < b) {
    return a;
  }
  else {
    return b;
  }
}

double** doAlloc(int N) {
  int i;
  // allocate N*N matrix and initialize to 0
  double** p = calloc(N, sizeof(*p));
  for (i = 0; i < N; i++) {
    p[i] = calloc(N, sizeof(double));
  }
  return p;
}

void doFree(double** p, int N) {
  int i;
  for (i = 0; i < N; i++) {
    free(p[i]);
  }
  free(p);
}

void doFree_frac(struct Frac** fraction, int N) {
  int i;
  for (i = 0; i < N; i++) {
    free(fraction[i]);
  }
  free(fraction);
}

int simulate(double** land, double** absorb, struct Frac** fraction, double** curr, double** trickle, int M, double A, int N) {
  int i, j;
  int t = 0;
  // flag to denote if there still remains some water at certain points
  int isWet = 0;
  
  while (isWet || M > 0) { // each iteration is one time step
    isWet = 0;

    // first traverse
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
	// 1) receive rain drop 
	if (M > 0) {
	  curr[i][j] += 1;
	}
	// 2) absorb water into ground
	if (curr[i][j] > 0) {
	  double abosorbAmt = min(A, curr[i][j]);
	  absorb[i][j] += abosorbAmt;
	  curr[i][j] -= abosorbAmt;
	}
	// 3a) populate the temporary trickle matrix, do nothing if there remains no water at this point or this point has no lower neighbors
	struct Frac* f = &fraction[i][j];

	if (curr[i][j] > 0 && f->willTrickle) {
	  double trickleAmt = min(1, curr[i][j]);
	  curr[i][j] -= trickleAmt;
	  if (i-1 >= 0 && f->up != 0) { // up (i-1,j)
	    trickle[i-1][j] += trickleAmt * f->up;
	  }
	  if (i+1 < N && f->down != 0) { // down (i+1,j)
	    trickle[i+1][j] += trickleAmt * f->down;
	  }
	  if (j-1 >= 0 && f->left != 0) { // left (i,j-1)
	    trickle[i][j-1] += trickleAmt * f->left;
	  }
	  if (j+1 < N && f->right != 0) { //right (i,j+1)
	    trickle[i][j+1] += trickleAmt * f->right;
	  }
	}
      }
    }

    // 3b) second traverse
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
	curr[i][j] += trickle[i][j];
	trickle[i][j] = 0; // reinitialize trickle matrix to 0
	if (curr[i][j] > 0) {
	  isWet = 1;
	}
      }
    }
    
    t++;
    if (M > 0) {
      M--;
    }
  }
  
  return t;
}
  
void calcFraction(struct Frac** fraction, double** land, int N) {
  int i, j;
  // traverse the land matrix to populate fraction matrix
  for(i = 0; i < N; i++) {
    for(j = 0; j < N; j++) {
      // first traversal of neighbors: determine min_elevation of the 5
      double min_elevation = land[i][j];
      if (i-1 >= 0 && land[i-1][j] < min_elevation) { // up (i-1,j)
	min_elevation = land[i-1][j];
      }
      if (i+1 < N && land[i+1][j] < min_elevation) { // down (i+1,j)
	min_elevation = land[i+1][j];
      }
      if (j-1 >= 0 && land[i][j-1] < min_elevation) { // left (i,j-1)
	min_elevation = land[i][j-1];
      }
      if (j+1 < N && land[i][j+1] < min_elevation) { // right (i,j+1)  
	min_elevation = land[i][j+1];
      }

      // check if (i, j) has any lower neighbor, if there is, then proceed, otherwise do nothing(all struct members already initialized to 0, willTrickle=0 meaning no trickle) and continue to the next iteration(go on to inspect the next point)
      if (min_elevation == land[i][j]) {
	continue;
      }
      // has lower neighbor, will trickle, so proceed

      struct Frac* f = &fraction[i][j];
      f->willTrickle = 1;
      
      // second traversal of neighbors: determine min_cnt of the 4
      double min_cnt = 0;
      if (i-1 >= 0 && land[i-1][j] == min_elevation) { // up (i-1,j)
	min_cnt++;
      }
      if (i+1 < N && land[i+1][j] == min_elevation) { // down (i+1,j)
	min_cnt++;
      }
      if (j-1 >= 0 && land[i][j-1] == min_elevation) { // left (i,j-1)  
	min_cnt++;
      }
      if (j+1 < N && land[i][j+1] == min_elevation) { // right (i,j+1) 
	min_cnt++;
      }

      // third traversal of neighbors: calulate the fraction of the trickle that each neighbor should receive from (i,j)
      double each = 1 / min_cnt;
      if (i-1 >= 0 && land[i-1][j] == min_elevation) { // up (i-1,j)
	f->up = each;
      }
      if (i+1 < N && land[i+1][j] == min_elevation) { // down (i+1,j)
	f->down = each;
      }
      if (j-1 >= 0 && land[i][j-1] == min_elevation) { // left (i,j-1)  
	f->left = each;
      }
      if (j+1 < N && land[i][j+1] == min_elevation) { // right (i,j+1) 
	f->right = each;
      }
    }
  }
}

int main(int argc, char** argv){
  // validate input
  if (argc != 5) {
    printf("Usage:\n./rainfall <M> <A> <N> <elevation_file>\n");
    return EXIT_FAILURE;
  }
  
  int M = atoi(argv[1]); // # of simulation time steps
  double A = atof(argv[2]); // absorption rate
  int N = atoi(argv[3]); // dimension of the landscape
  int i, j;
  int t; // timesteps needed to finish simulating 

  // matrix to represent the landscape
  double** land = doAlloc(N);
  // matrix to record current amount of rain at each point
  double** curr = doAlloc(N);  
  // absorb matrix to record the amount of water draining to land
  double** absorb = doAlloc(N);
  // temporary matrix for the current time step, to record trickle in the first traverse, then used in the second traverse to update curr matrix
  double** trickle = doAlloc(N);
  
  struct timespec start_time, end_time;

  // open file
  FILE * f = fopen(argv[4], "r");
  if (f == NULL) {
    perror("Cannot open file\n");
    return EXIT_FAILURE;
  }
  
  // read the landscape into land matrix
  for(i = 0; i < N; i++) {
    for(j = 0; j < N; j++) {
      if (!fscanf(f, "%lf", &land[i][j])) {
	break;
      }
    }
  }
  
  // close file
  if (fclose(f) != 0) {
    perror("Failed to close the input file");
    return EXIT_FAILURE;
  }
  
  // fraction matrix to determine: for each point, if it will trickle water to its neighbors, and if so, what is the fraction for each of its neighbor(up, down, left, right), e.g: 0, 1, 1/2, 1/3, 1/4
  struct Frac** fraction = calloc(N, sizeof(*fraction));
  for (i = 0; i < N; i++) {
    fraction[i] = calloc(N, sizeof(struct Frac));
  }

  // populate the fraction matrix
  calcFraction(fraction, land, N);

  clock_gettime(CLOCK_MONOTONIC, &start_time);
  // do the simulation
  t = simulate(land, absorb, fraction, curr, trickle, M, A, N);
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  double elapsed_ns = calc_time(start_time, end_time);
  double elapsed_ms = elapsed_ns / 1000000.0;
  printf("Time=%f milliseconds\n", elapsed_ms);
  
  // print result to stdout according to assignment spec  
  printf("Rainfall simulation took %d time steps to complete.\n", t);
  printf("The following grid shows the number of raindrops absorbed at each point:\n");
  for(i = 0; i < N; i++) {
    for(j = 0; j < N; j++) {
      printf("%g ", absorb[i][j]);
    }
    printf("\n");
  }
  
  // free the allocated space for serveral matrixes
  doFree(curr, N);  
  doFree(land, N);
  doFree(absorb, N);
  doFree(trickle, N); 
  doFree_frac(fraction, N);   
  
  return EXIT_SUCCESS;
}
