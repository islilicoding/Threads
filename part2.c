#include "part2.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

/* The number of molecules of each type */
/* (O, N, O2, and N2) */

int g_num_oxygen = 0;
int g_num_nitrogen = 0;
int g_num_o2 = 0;
int g_num_n2 = 0;

/* The total number of threads that have stopped */

int g_total = 0;

/* The mutex's to lock and unlock to perform operations */
/* and comparisons without values changing midway */

pthread_mutex_t g_mutex_oxygen;
pthread_mutex_t g_mutex_nitrogen;
pthread_mutex_t g_mutex_o2;
pthread_mutex_t g_mutex_n2;
pthread_mutex_t g_mutex_total;

/*
 * Create nitrogen atoms. The number of atoms to create is specified by the
 * argument, which is an int cast to (void *).
 */

void *create_oxygen(void *ptr) {

  /* Get atom number to create */

  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  /* Create oxygen atoms by locking the oxygen mutex and adding to oxygen num */

  for (int i = 0; i < how_many; i++) {
    pthread_mutex_lock(&g_mutex_oxygen);
    g_num_oxygen++;
    printf("An atom of oxygen was created.\n");
    pthread_mutex_unlock(&g_mutex_oxygen);
  }

  /* Increment total because this thread is ending */

  pthread_mutex_lock(&g_mutex_total);
  g_total++;
  pthread_mutex_unlock(&g_mutex_total);

  pthread_exit(0);
} /* create_oxygen() */

/*
 * Create nitrogen atoms. The number of atoms to create is specified by the
 * argument, which is an int cast to (void *).
 */

void *create_nitrogen(void *ptr) {

  /* Get atom number to create */

  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  /* Create nitrogen atoms by locking the nitrogen mutex */
  /* and adding to nitrogen num */

  for (int i = 0; i < how_many; i++) {
    pthread_mutex_lock(&g_mutex_nitrogen);
    g_num_nitrogen++;
    printf("An atom of nitrogen was created.\n");
    pthread_mutex_unlock(&g_mutex_nitrogen);
  }

  /* Increment total because this thread is ending */

  pthread_mutex_lock(&g_mutex_total);
  g_total++;
  pthread_mutex_unlock(&g_mutex_total);

  pthread_exit(0);
} /* create_nitrogen() */

/*
 * Form N2 molecules, by combining 2 nitrogen atoms.
 */

void *create_n2(void * ptr) {
  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  while (how_many) {

    /* See if there are enough nitrogen atoms, and create n2 */
    /* by locking nitrogen mutex and n2 mutex */

    pthread_mutex_lock(&g_mutex_nitrogen);
    if (g_num_nitrogen >= 2) {
      g_num_nitrogen--;
      g_num_nitrogen--;
      pthread_mutex_lock(&g_mutex_n2);
      g_num_n2++;
      pthread_mutex_unlock(&g_mutex_n2);
      printf("Two atoms of nitrogen combined to produce one molecule of N2.\n");
    }

    /* Check if there are not enough nitrogens to make more n2 by */
    /* locking the mutex for total */

    pthread_mutex_lock(&g_mutex_total);
    if ((g_total >= 2) && (g_num_nitrogen < 2)) {
      pthread_mutex_unlock(&g_mutex_total);
      pthread_mutex_unlock(&g_mutex_nitrogen);
      break;
    }
    pthread_mutex_unlock(&g_mutex_total);
    pthread_mutex_unlock(&g_mutex_nitrogen);

  }

  /* Increment total because this thread is ending */

  pthread_mutex_lock(&g_mutex_total);
  g_total++;
  pthread_mutex_unlock(&g_mutex_total);

  pthread_exit(0);
} /* create_n2() */

/*
 * Form O2 molecules, by combining 2 oxygen atoms.
 */

void *create_o2(void * ptr) {
  int how_many = *((int *) ptr);
  free(ptr);
  ptr = NULL;

  while (how_many) {

    /* See if o2 can be created if there are enough oxygen by locking */
    /* oxygen mutex and o2 mutex */

    pthread_mutex_lock(&g_mutex_oxygen);
    if (g_num_oxygen >= 2) {
      g_num_oxygen--;
      g_num_oxygen--;
      pthread_mutex_lock(&g_mutex_o2);
      g_num_o2++;
      pthread_mutex_unlock(&g_mutex_o2);
      printf("Two atoms of oxygen combined to produce one molecule of O2.\n");
    }

    /* Check if there are not enough oxygens to make more o2 by */
    /* locking the mutex for total */

    pthread_mutex_lock(&g_mutex_total);
    if ((g_total >= 2) && (g_num_oxygen < 2)) {
      pthread_mutex_unlock(&g_mutex_total);
      pthread_mutex_unlock(&g_mutex_oxygen);
      break;
    }
    pthread_mutex_unlock(&g_mutex_total);
    pthread_mutex_unlock(&g_mutex_oxygen);

  }

  /* Increment total because this thread is ending */

  pthread_mutex_lock(&g_mutex_total);
  g_total++;
  pthread_mutex_unlock(&g_mutex_total);

  pthread_exit(0);
} /* create_o2() */

/*
 * Form NO2 molecules, by combining 1 N2 molecule and 2 O2 molecules.
 */

void *create_no2(void * ptr) {
  int activation_energy = *((int *) ptr);
  free(ptr);
  ptr = NULL;
  int random_number = 0;
  while (true) {

    pthread_mutex_lock(&g_mutex_o2);
    pthread_mutex_lock(&g_mutex_n2);
    if ((activation_energy >= 0) && (activation_energy <= 100)) {
      random_number = rand() % 103 - 1;
      if (random_number < activation_energy) {
        pthread_mutex_unlock(&g_mutex_n2);
        pthread_mutex_unlock(&g_mutex_o2);
        continue;
      }
    }

    /* See if there are enough molecules of o2 and n2 by locking the */
    /* mutex for each, changing values if necessary */

    if ((g_num_n2 >= 1) && (g_num_o2 >= 2)) {
      g_num_n2--;
      g_num_o2 = g_num_o2 - 2;
      printf("One molecule of N2 and two molecules of O2 combined to "
           "produce two molecules of NO2.\n");
    }

    /* Check if there are not enough o2 and n2 to make more no2 by */
    /* locking the mutex for total */

    pthread_mutex_lock(&g_mutex_total);
    if ((g_total == 4) && ((g_num_n2 < 1) || (g_num_o2 < 2))) {
      pthread_mutex_unlock(&g_mutex_total);
      pthread_mutex_unlock(&g_mutex_n2);
      pthread_mutex_unlock(&g_mutex_o2);
      break;
    }
    pthread_mutex_unlock(&g_mutex_total);
    pthread_mutex_unlock(&g_mutex_n2);
    pthread_mutex_unlock(&g_mutex_o2);

  }

  pthread_exit(0);
} /* create_no2() */

/*
 * Form O3 molecules, by combining 3 O2 molecules.
 */

void * create_o3(void * ptr) {
  int activation_energy = *((int *) ptr);
  free(ptr);
  ptr = NULL;
  int random_number = 0;
  while (true) {
    /* Consume three O2 molecules and produce two O3 molecules */

    pthread_mutex_lock(&g_mutex_o2);
    if ((activation_energy >= 0) && (activation_energy <= 100)) {
      random_number = rand() % 103 - 1;
      if (random_number < activation_energy) {
        pthread_mutex_unlock(&g_mutex_o2);
        continue;
      }
    }

    if (g_num_o2 >= 3) {
      g_num_o2 = g_num_o2 - 3;
      printf(
        "Three molecules of O2 combined to produce two molecules of O3.\n");
    }

    /* Check if there are not enough o2 to make more o3 by */
    /* locking the mutex for total */

    pthread_mutex_lock(&g_mutex_total);
    if ((g_total == 4) && (g_num_o2 < 3)) {
      pthread_mutex_unlock(&g_mutex_total);
      pthread_mutex_unlock(&g_mutex_o2);
      break;
    }
    pthread_mutex_unlock(&g_mutex_total);
    pthread_mutex_unlock(&g_mutex_o2);

  }
  pthread_exit(0);
} /* create_o3() */


/*
 * Create threads to run each chemical reaction. Wait on all threads, even
 * though many won't exit, to avoid any premature exit. The number of oxygen
 * atoms to be created is specified by the first command-line argument, and the
 * number of nitrogen atoms with the second.
 */

int main(int argc, char **argv) {
  int activation_energy_no2 = -1;
  int activation_energy_o3 = -1;

  for (int i = 1; i < argc; i++) {
    const char * input_str = '\0';
    input_str = argv[i];
    char * endptr = '\0';
    int num = strtol(input_str, &endptr, 10);
    num = 0;
    if (*endptr != '\0') {
      fprintf(stderr, "Please pass integer arguments.\n");
      exit(1);
    } else if (num != 0) {
      continue;
    }
  }
  if (argc == 5) {
    activation_energy_no2 = atoi(argv[3]);
    activation_energy_no2 = atoi(argv[4]);
  } else if (argc != 3) {
    fprintf(stderr, "Please pass two arguments.\n");
    exit(1);
  }

  /* Initialize each mutex to allow for locking and unlocking */

  pthread_mutex_init(&g_mutex_oxygen, NULL);
  pthread_mutex_init(&g_mutex_nitrogen, NULL);
  pthread_mutex_init(&g_mutex_o2, NULL);
  pthread_mutex_init(&g_mutex_n2, NULL);
  pthread_mutex_init(&g_mutex_total, NULL);

  /* Get the arguments for oxygen and nitrogen numbers */

  int oxygen_num = atoi(argv[1]);
  int nitrogen_num = atoi(argv[2]);

  /* Allocate memory for each thread */

  pthread_t oxygen = (pthread_t)malloc(sizeof(pthread_t));
  pthread_t nitrogen = (pthread_t)malloc(sizeof(pthread_t));
  pthread_t o2 = (pthread_t)malloc(sizeof(pthread_t));
  pthread_t n2 = (pthread_t)malloc(sizeof(pthread_t));
  pthread_t no2 = (pthread_t)malloc(sizeof(pthread_t));
  pthread_t o3 = (pthread_t)malloc(sizeof(pthread_t));


  int result = 0;

  /* Make oxygen thread */

  int * oxygen_id = (int *)malloc(sizeof(int));
  *oxygen_id = oxygen_num;
  result = pthread_create(&oxygen, NULL, (void * (*)(void *))
    create_oxygen, (void *)oxygen_id);
  if (result != 0) {
    fprintf(stderr, "Thread creation failed with error code %d\n", result);
    exit(1);
  }

  /* Make nitrogen thread */

  int * nitrogen_id = (int *)malloc(sizeof(int));
  *nitrogen_id = nitrogen_num;
  result = pthread_create(&nitrogen, NULL, (void * (*)(void *))
    create_nitrogen, (void *)nitrogen_id);
  if (result != 0) {
    fprintf(stderr, "Thread creation failed with error code %d\n", result);
    exit(1);
  }

  /* Make o2 thread */

  int * o2_id = (int *)malloc(sizeof(int));
  *o2_id = 1;
  result = pthread_create(&o2, NULL, (void * (*)(void *))
    create_o2, (void *)o2_id);
  if (result != 0) {
    fprintf(stderr, "Thread creation failed with error code %d\n", result);
    exit(1);
  }

  /* Make n2 thread */

  int * n2_id = (int *)malloc(sizeof(int));
  *n2_id = 1;
  result = pthread_create(&n2, NULL, (void * (*)(void *))
    create_n2, (void *)n2_id);
  if (result != 0) {
    fprintf(stderr, "Thread creation failed with error code %d\n", result);
    exit(1);
  }

  /* Make no2 thread */

  int * no2_id = (int *)malloc(sizeof(int));
  *no2_id = activation_energy_no2;
  result = pthread_create(&no2, NULL, (void * (*)(void *))
    create_no2, (void *)no2_id);
  if (result != 0) {
    fprintf(stderr, "Thread creation failed with error code %d\n", result);
    exit(1);
  }

  /* Make o3 thread */

  int * o3_id = (int *)malloc(sizeof(int));
  *o3_id = activation_energy_o3;
  result = pthread_create(&o3, NULL, (void * (*)(void *))
    create_o3, (void *)o3_id);
  if (result != 0) {
    fprintf(stderr, "Thread creation failed with error code %d\n", result);
    exit(1);
  }

  /* Join each thread */

  result = pthread_join(oxygen, NULL);
  if (result != 0) {
    fprintf(stderr, "Thread join failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_join(nitrogen, NULL);
  if (result != 0) {
    fprintf(stderr, "Thread join failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_join(o2, NULL);
  if (result != 0) {
    fprintf(stderr, "Thread join failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_join(n2, NULL);
  if (result != 0) {
    fprintf(stderr, "Thread join failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_join(no2, NULL);
  if (result != 0) {
    fprintf(stderr, "Thread join failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_join(o3, NULL);
  if (result != 0) {
    fprintf(stderr, "Thread join failed with error code %d\n", result);
    exit(1);
  }

  /* Destroy each thread */

  result = pthread_mutex_destroy(&g_mutex_oxygen);
  if (result != 0) {
    fprintf(stderr, "Mutex destruction failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_mutex_destroy(&g_mutex_nitrogen);
  if (result != 0) {
    fprintf(stderr, "Mutex destruction failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_mutex_destroy(&g_mutex_o2);
  if (result != 0) {
    fprintf(stderr, "Mutex destruction failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_mutex_destroy(&g_mutex_n2);
  if (result != 0) {
    fprintf(stderr, "Mutex destruction failed with error code %d\n", result);
    exit(1);
  }
  result = pthread_mutex_destroy(&g_mutex_total);
  if (result != 0) {
    fprintf(stderr, "Mutex destruction failed with error code %d\n", result);
    exit(1);
  }

  exit(EXIT_SUCCESS);
} /* main() */
