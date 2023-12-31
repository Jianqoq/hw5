#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define BUFSIZE 256
#define CONSTANT 128

double begin = 0;

typedef struct Entry {
  uint64_t hashed;
  struct Entry *next;
} Entry;

typedef struct Node {
  int *board;
  int size;
  struct Node *parent;
  int move;
  uint64_t hashed;
} Node;

typedef struct HashSet {
  Entry **Entries;
  size_t size;
} HashSet;

typedef struct Queue {
  Node **node;
  int size;
  int front;
  int rear;
} Queue;

uint64_t encodepuzzle(int *arr, int arr_len) {
  uint64_t encoded = 0;
  for (int i = 0; i < arr_len; i++) {
	encoded |= (uint64_t)arr[i] << (60 - 4 * i);
  }
  return encoded;
}

void enqueue(Queue *queue, Node *node) {
  if (queue->rear == queue->size - 1) {
    queue->node[queue->rear] = node;
    queue->rear = 0;
  } else {
    queue->node[queue->rear++] = node;
  }
}

Node *dequeue(Queue *queue) {
  if (queue->front == queue->rear) {
    return NULL;
  } else {
    if (queue->front == queue->size - 1) {
      int tmp = queue->front;
      queue->front = 0;
      return queue->node[tmp];
    } else {
      return queue->node[queue->front++];
    }
  }
}

int is_empty(Queue *queue) {
  if (queue->front == queue->rear) {
    return 1;
  } else {
    return 0;
  }
}

Queue *init_queue(int size) {
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  queue->node = (Node **)malloc(sizeof(Node *) * size);
  queue->size = size;
  queue->front = 0;
  queue->rear = 0;
  return queue;
}

size_t hash(uint64_t hashed, size_t size) {
  return hashed % size;
}

HashSet *init(size_t size) {
  Entry **entries = (Entry **)malloc(sizeof(Entry *) * (size + 1));
  for (size_t i = 0; i < (size + 1); i++)
    entries[i] = NULL;
  HashSet *set = (HashSet *)malloc(sizeof(HashSet));
  set->Entries = entries;
  set->size = size + 1;
  return set;
}

void insert(HashSet *set, uint64_t hashed) {
  size_t index = hash(hashed, set->size);
  if (set->Entries[index] != NULL) {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    entry->next = NULL;
    entry->hashed = hashed;
    Entry *next = set->Entries[index];
    while (next->next != NULL) {
      next = next->next;
    }
    next->next = entry;
  } else {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    entry->hashed = hashed;
    entry->next = NULL;
    set->Entries[index] = entry;
  }
}

int arr_equal(int *arr1, int *arr2, int size) {
  for (int i = 0; i < size; i++) {
    if (arr1[i] != arr2[i])
      return 0;
  }
  return 1;
}

int is_member(HashSet *set, uint64_t hashed) {
  size_t index = hash(hashed, set->size);
  if (set->Entries[index] != NULL) {
    if (hashed == set->Entries[index]->hashed) {
      return 1;
    } else {
      Entry *entry = set->Entries[index]->next;
      while (entry != NULL) {
        if (hashed == entry->hashed)
          return 1;
        entry = entry->next;
      }
    }
  } else {
    return 0;
  }
}

void printBoard(int *board, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%d ", board[i * size + j]);
    }
    printf("\n");
  }
  printf("\n");
}

void swap(int *board, int i, int j) {
  int temp = board[i];
  board[i] = board[j];
  board[j] = temp;
}

HashSet *set2 = NULL;
Queue *queue = NULL;
int *goal = NULL;
uint64_t goal_hashed = 0;

Node **yield_node(Node *node) {
  Node **nodes = (Node **)malloc(sizeof(Node *) * 4);
  for (int row = 0; row < node->size; row++) {
    for (int column = 0; column < node->size; column++) {
      if (node->board[row * node->size + column] == 0) {
        // Up
        if (row == 0) {
          nodes[0] = NULL;
        } else {
          Node *tmp0 = (Node *)malloc(sizeof(Node));
          tmp0->move = node->board[(row - 1) * node->size + column];
          tmp0->size = node->size;
          tmp0->board = (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp0->board, node->board,
                 sizeof(int) * node->size * node->size);
          swap(tmp0->board, row * node->size + column,
               (row - 1) * node->size + column);
          tmp0->parent = node;
		  tmp0->hashed = encodepuzzle(tmp0->board, tmp0->size * tmp0->size);
          nodes[0] = tmp0;
        }
        // Down
        if (row == node->size - 1) {
          nodes[1] = NULL;
        } else {
          Node *tmp1 = (Node *)malloc(sizeof(Node));
          tmp1->move = node->board[(row + 1) * node->size + column];
          tmp1->size = node->size;
          tmp1->board = (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp1->board, node->board,
                 sizeof(int) * node->size * node->size);
          tmp1->parent = node;
          swap(tmp1->board, row * node->size + column,
               (row + 1) * node->size + column);
			   tmp1->hashed = encodepuzzle(tmp1->board, tmp1->size * tmp1->size);
          nodes[1] = tmp1;
        }
        // Left
        if (column == 0) {
          nodes[2] = NULL;
        } else {
          Node *tmp2 = (Node *)malloc(sizeof(Node));
          tmp2->move = node->board[row * node->size + column - 1];
          tmp2->size = node->size;
          tmp2->board = (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp2->board, node->board,
                 sizeof(int) * node->size * node->size);
          swap(tmp2->board, row * node->size + column,
               row * node->size + column - 1);
          tmp2->parent = node;
		  tmp2->hashed = encodepuzzle(tmp2->board, tmp2->size * tmp2->size);
          nodes[2] = tmp2;
        }
        // Right
        if (column == node->size - 1) {
          nodes[3] = NULL;
        } else {
          Node *tmp3 = (Node *)malloc(sizeof(Node));
          tmp3->move = node->board[row * node->size + column + 1];
          tmp3->size = node->size;
          tmp3->board = (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp3->board, node->board,
                 sizeof(int) * node->size * node->size);
          swap(tmp3->board, row * node->size + column,
               row * node->size + column + 1);
          tmp3->parent = node;
		  tmp3->hashed = encodepuzzle(tmp3->board, tmp3->size * tmp3->size);
          nodes[3] = tmp3;
        }
        if (nodes[0] == NULL && nodes[1] == NULL && nodes[2] == NULL &&
            nodes[3] == NULL) {
          free(nodes);
          return NULL;
        }
        return nodes;
      }
    }
  }

  free(nodes);
  return NULL;
}

char *build_graph(Node *root) {
  enqueue(queue, root);
  insert(set2, root->hashed);
  int total_size = root->size * root->size;
  while (!is_empty(queue)) {
    Node *node = dequeue(queue);
    if (node->hashed == goal_hashed) {
      Node *tmp = node;
      int length = 0;
      while (tmp != NULL) {
        length++;
        tmp = tmp->parent;
      }
      int *moves = (int *)malloc(sizeof(int) * (length - 1));
      tmp = node;
      int idx = length - 2;
      while (tmp != NULL) {
        moves[idx--] = tmp->move;
        tmp = tmp->parent;
      }
      char *result = (char *)malloc(sizeof(char) * 100);
      int tracker = 0;
      for (int i = 0; i < length - 1; i++) {
        char num[100];
        sprintf(num, "%d", moves[i]);
        for (int j = 0; j < strlen(num); j++) {
          result[tracker++] = num[j];
        }
        result[tracker++] = ' ';
      }
      result[tracker] = '\0';
      return result;
    }
    Node **nodes = yield_node(node);
    if (nodes == NULL) {
      continue;
    }
    for (int i = 0; i < 4; i++) {
      if (nodes[i] != NULL) {
        if (is_member(set2, nodes[i]->hashed)) {
          free(nodes[i]->board);
          free(nodes[i]);
          continue;
        } else {
          enqueue(queue, nodes[i]);
          insert(set2, nodes[i]->hashed);
        }
      }
    }
    free(nodes);
  }
  return NULL;
}

int main(int argc, char **argv) {
  FILE *fp_in, *fp_out;

  fp_in = fopen(argv[1], "r");
  if (fp_in == NULL) {
    printf("Could not open a file.\n");
    return -1;
  }

  fp_out = fopen(argv[2], "w");
  if (fp_out == NULL) {
    printf("Could not open a file.\n");
    return -1;
  }

  char *line = NULL;
  size_t lineBuffSize = 0;
  ssize_t lineSize;
  int k;

  getline(&line, &lineBuffSize,
          fp_in); // ignore the first line in file, which is a comment
  fscanf(fp_in, "%d\n", &k); // read size of the board
  // printf("k = %d\n", k); //make sure you read k properly for DEBUG purposes
  getline(&line, &lineBuffSize,
          fp_in); // ignore the second line in file, which is a comment

  int initial_board[k * k]; // get kxk memory to hold the initial board
  for (int i = 0; i < k * k; i++)
    fscanf(fp_in, "%d ", &initial_board[i]);
  // Assuming that I have a function to print the board, print it here to make
  // sure I read the input board properly for DEBUG purposes
  fclose(fp_in);
  ////////////////////
  // do the rest to solve the puzzle
  ////////////////////
  Node *root = (Node *)malloc(sizeof(Node));
  root->size = k;
  root->board = (int *)malloc(sizeof(int) * k * k);
  memcpy(root->board, initial_board, sizeof(int) * k * k);
  root->parent = NULL;
  root->move = -1;
  root->hashed = encodepuzzle(root->board, k * k);
  set2 = init(100000);
  queue = init_queue(100000);
  goal = (int *)malloc(sizeof(int) * k * k);
  for (int i = 1; i < k * k; i++) {
    goal[i - 1] = i;
  }
  goal[k * k - 1] = 0;
	  goal_hashed = encodepuzzle(goal, k * k);
  double begin = clock();
  char *moves = build_graph(root);
  double end = clock();
  printf("Time: %lf\n", (end - begin) / CLOCKS_PER_SEC);
  // once you are done, you can use the code similar to the one below to print
  // the output into file if the puzzle is NOT solvable use something as follows
  fprintf(fp_out, "#moves\n");
  if (moves != NULL)
    fprintf(fp_out, "%s\n", moves);
  else
    fprintf(fp_out, "no solution\n");
  // if it is solvable, then use something as follows:
  // probably within a loop, or however you stored proper moves, print them one
  // by one by leaving a space between moves, as below
  //  for(int i=0;i<numberOfMoves;i++)
  //  	fprintf(fp_out, "%d ", move[i]);

  fclose(fp_out);

  return 0;
}
