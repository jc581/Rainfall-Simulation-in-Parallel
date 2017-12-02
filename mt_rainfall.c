#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

#define NUM_THREADS 16

typedef struct _Frac {
  int willTrickle;
  double up;
  double down;
  double left;
  double right;
} Frac;

typedef struct _thr_arg {
  double** absorb;  
  double** curr;
  double** trickle;
  Frac** fraction;
  int M;
  double A;
  int N;
  int id;
  int* p_isWet;
  int* p_t;
  pthread_barrier_t* p_mybarrier;
  pthread_mutex_t* p_locks;
} thr_arg;

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

void doFree_frac(Frac** fraction, int N) {
  int i;
  for (i = 0; i < N; i++) {
    free(fraction[i]);
  }
  free(fraction);
}
 
void calcFraction(Frac** fraction, double** land, int N) {
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

      Frac* f = &fraction[i][j];
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

void* simulate(void* varg) {
  /* recv args */
  thr_arg* arg = varg;
  double** absorb = arg->absorb;
  double** curr = arg->curr;  
  double** trickle = arg->trickle;
  Frac** fraction = arg->fraction;
  int M = arg->M;
  double A = arg->A;
  int N = arg->N;
  int id = arg->id;
  int* p_isWet = arg->p_isWet;
  int* p_t = arg->p_t;
  pthread_barrier_t* p_mybarrier = arg->p_mybarrier;
  pthread_mutex_t* locks = arg->p_locks;
  
  /* compute bounds for this thread */
  int startRow = id * N / NUM_THREADS;
  int endRow = (id + 1) * (N / NUM_THREADS) - 1;
  
  int i, j;
  while (*p_isWet || M > 0) {
    // barrier to make sure every thread enters while loop, ready to re-ini isWet to 0
    pthread_barrier_wait(p_mybarrier);
    *p_isWet = 0;
    // first traverse
    for (i = startRow; i <= endRow; i++) {
      // NOTE: need synchronization only when i is BORDER ROW: i == startRow || i == startRow + 1 || i == endRow || i == endRow - 1
      if (i == startRow || i == startRow + 1 || i == endRow || i == endRow - 1) {
      	if (i - 1 >= 0) { // acquire the lock of row i-1, if there is any
	  pthread_mutex_lock(&locks[i-1]);
	}
	pthread_mutex_lock(&locks[i]); // acquire the lock of up row i
	if (i + 1 < N) { // acquire the lock of row i+1, if there is any 
	  pthread_mutex_lock(&locks[i+1]);
	}
      }
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
	// 3a) populate trickle matrix, only if there remains some water at this point and this point has some lower neighbor
	Frac* f = &fraction[i][j];
	if (curr[i][j] > 0 && f->willTrickle) {
	  double trickleAmt = min(1, curr[i][j]);
	  curr[i][j] -= trickleAmt;
	  // up (i-1,j)
	  if (i-1 >= 0 && f->up != 0) {
	    trickle[i-1][j] += trickleAmt * f->up;
	  } 	
	  // down (i+1,j) 
	  if (i+1 < N && f->down != 0) {
	    trickle[i+1][j] += trickleAmt * f->down;
	  }
	  // left (i,j-1)       
	  if (j-1 >= 0 && f->left != 0) {
	    trickle[i][j-1] += trickleAmt * f->left;
	  }
	  //right (i,j+1)  
	  if (j+1 < N && f->right != 0) { 
	    trickle[i][j+1] += trickleAmt * f->right;
	  }
	} // end 3a)
      } // end j loop
      if (i == startRow || i == startRow + 1 || i == endRow || i == endRow - 1) {
	if (i - 1 >= 0) {
          pthread_mutex_unlock(&locks[i-1]);
        }
	pthread_mutex_unlock(&locks[i]);
	if (i + 1 < N) {
          pthread_mutex_unlock(&locks[i+1]);
        }
      }
    } // end i loop
    
    // barrier to make sure every thread has finished first traverse (at current iteration), ready to do the second traverse
    pthread_barrier_wait(p_mybarrier);

    // 3b) second traverse
    for (i = startRow; i <= endRow; i++) {
      for (j = 0; j < N; j++) {
	curr[i][j] += trickle[i][j];
	trickle[i][j] = 0; // reinitialize trickle matrix to 0
	if (curr[i][j] > 0) {
	  *p_isWet = 1;
	}
      }
    }
  
    if (M > 0) {  
      M--;
    }  
    if (id == 0) { // only let one thread count the time steps to finish simulation
      (*p_t)++;
    }
    // barrier to make sure all threads can see the same value of isWet (whether set to 1 or not), before checking the while loop condition for the next iteration
    pthread_barrier_wait(p_mybarrier);
  } // end while loop
  
  /* free thr_arg */
  free(arg);
  return NULL;
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
  
  assert(NUM_THREADS <= N);

  int i, j;
  struct timespec start_time, end_time;
  int isWet = 0; // flag to denote if there still remains any water at certain points
  int t = 0; // timesteps needed to finish simulating 
  pthread_t* threads;
  pthread_barrier_t mybarrier;
  pthread_barrier_init(&mybarrier, NULL, NUM_THREADS);

  // matrix to represent the landscape
  double** land = doAlloc(N);
  // matrix to record current amount of rain at each point
  double** curr = doAlloc(N);
  // absorb matrix to record the amount of water draining to land
  double** absorb = doAlloc(N);
  // temporary matrix for the current time step, to record trickle in the first traverse, then used in the second traverse to update curr matrix
  double** trickle = doAlloc(N);
    
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
  Frac** fraction = calloc(N, sizeof(*fraction));
  for (i = 0; i < N; i++) {
    fraction[i] = calloc(N, sizeof(Frac));
  }

  // populate the fraction matrix
  calcFraction(fraction, land, N);
  
  // allocate and ini locks
  pthread_mutex_t* locks = malloc(N * sizeof(*locks));
  for (i = 0; i < N; i++) {
    pthread_mutex_init(&locks[i], NULL);
  }
  /********************************************* let threads do the simulation *********************************************/
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  
  /* Allocate thread objects */
  threads = (pthread_t *) malloc(NUM_THREADS * sizeof(pthread_t));
  /* Create threads and assign the work */
  for (i = 0; i < NUM_THREADS; i++) { 
    /* allocate and initialize the thread argument */
    thr_arg* p = malloc(sizeof(*p));
    p->absorb = absorb;
    p->curr = curr;
    p->trickle = trickle;
    p->fraction = fraction;
    p->M = M;
    p->A = A;
    p->N = N;
    p->id = i;
    p->p_isWet = &isWet;
    p->p_t = &t; 
    p->p_mybarrier = &mybarrier;
    p->p_locks = locks;
    pthread_create(&threads[i], NULL, simulate, (void *)(p));  
  }
  /* Join the threads to complete simulation */
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);  
  }
 
  //t = simulate(absorb, fraction, M, A, N);
clock_gettime(CLOCK_MONOTONIC, &end_time);
  /**************************************************** end simulation ****************************************************/
  
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

  // Destroy locks...
  
  // Destroy barrier...
  
  // free thread objects
  free(threads);
  // free locks
  free(locks);
  // free the allocated space for serveral matrixes
  doFree(curr, N);
  doFree(land, N);
  doFree(absorb, N);
  doFree(trickle, N); 
  doFree_frac(fraction, N);   
  
  return EXIT_SUCCESS;
}
