#include <assert.h>
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
  char *word;
  struct Entry *next;
} Entry;

typedef struct Node {
  int *board;
  int size;
  char *board_str;
  struct Node *parent;
  int move;
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

size_t hash(char *word, size_t size) {

  size_t index = 0;
  int idx = 0;
  while (word[idx] != '\0') {
    index = (index + word[idx]) * 101;
    idx++;
  }
  return (size_t)(index * CONSTANT) % size;
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

void insert(HashSet *set, char *word) {
  size_t index = hash(word, set->size);
  if (set->Entries[index] != NULL) {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    char *string = (char *)malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(string, word);
    entry->word = string;
    entry->next = NULL;
    Entry *next = set->Entries[index];
    while (next->next != NULL) {
      next = next->next;
    }
    next->next = entry;
  } else {
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    char *string = (char *)malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(string, word);
    entry->word = string;
    entry->next = NULL;
    set->Entries[index] = entry;
  }
}

char *get(HashSet *set, char *word) {
  size_t index = hash(word, set->size);
  if (set->Entries[index] != NULL) {
    if (!strcmp(word, set->Entries[index]->word)) {
      return set->Entries[index]->word;
    } else {
      Entry *entry = set->Entries[index]->next;
      while (entry != NULL) {
        if (!strcmp(word, entry->word))
          return entry->word;
        entry = entry->next;
      }
    }
  } else {
    return NULL;
  }
}

int is_member(HashSet *set, char *word) {
  size_t index = hash(word, set->size);
  if (set->Entries[index] != NULL) {
    if (!strcmp(word, set->Entries[index]->word)) {
      return 1;
    } else {
      Entry *entry = set->Entries[index]->next;
      while (entry != NULL) {
        if (!strcmp(word, entry->word))
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
      // printf("[%d: %d]", board[i * size + j]->number, board[i * size +
      // j]->visited);
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

HashSet *set = NULL;
HashSet *set2 = NULL;
Queue *queue = NULL;
char *goal = NULL;

Node **yield_node(Node *node) {
  Node **nodes = (Node **)malloc(sizeof(Node *) * 4);
  for (int row = 0; row < node->size; row++) {
    for (int column = 0; column < node->size; column++) {
      if (node->board[row * node->size + column] == 0) {
        // Up
        if (row == 0) {
          nodes[0] = NULL;
        } else {
          Node* tmp0 = (Node *)malloc(sizeof(Node));
          tmp0->move = node->board[(row - 1) * node->size + column];
          tmp0->size = node->size;
          tmp0->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp0->board, node->board,
                 sizeof(int) * node->size * node->size);
          tmp0->board_str = (char *)malloc(sizeof(char) * 100);
          swap(tmp0->board, row * node->size + column,
               (row - 1) * node->size + column);
          tmp0->parent = node;
		  clock_t _begin = clock();
          int pos = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            pos += sprintf(tmp0->board_str + pos, "%d", tmp0->board[i]);
          }
          tmp0->board_str[pos] = '\0';
		//   printf("%s\n", tmp0->board_str);
		  begin += (double)(clock() - _begin) / CLOCKS_PER_SEC;
		  nodes[0] = tmp0;
        }
        // Down
        if (row == node->size - 1) {
          nodes[1] = NULL;
        } else {
          Node* tmp1 = (Node *)malloc(sizeof(Node));
          tmp1->move = node->board[(row + 1) * node->size + column];
          tmp1->size = node->size;
          tmp1->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp1->board, node->board,
                 sizeof(int) * node->size * node->size);
          tmp1->parent = node;
          tmp1->board_str = (char *)malloc(sizeof(char) * 100);
          tmp1->board_str[node->size * node->size] = '\0';
          swap(tmp1->board, row * node->size + column,
               (row + 1) * node->size + column);
			clock_t _begin = clock();
          int tracker = 0;
          int pos = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            pos += sprintf(tmp1->board_str + pos, "%d", tmp1->board[i]);
          }
          tmp1->board_str[pos] = '\0';
		//   printf("%s\n", tmp1->board_str);
		  begin += (double)(clock() - _begin) / CLOCKS_PER_SEC;
		  nodes[1] = tmp1;
        }
        // Left
        if (column == 0) {
          nodes[2] = NULL;
        } else {
          Node* tmp2 = (Node *)malloc(sizeof(Node));
          tmp2->move = node->board[row * node->size + column - 1];
          tmp2->size = node->size;
          tmp2->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp2->board, node->board,
                 sizeof(int) * node->size * node->size);
          tmp2->board_str = (char *)malloc(sizeof(char) * 100);
          tmp2->board_str[node->size * node->size] = '\0';
          swap(tmp2->board, row * node->size + column,
               row * node->size + column - 1);
          tmp2->parent = node;
		  clock_t _begin = clock();
          int pos = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            pos += sprintf(tmp2->board_str + pos, "%d", tmp2->board[i]);
          }
          tmp2->board_str[pos] = '\0';
		//   printf("%s\n", tmp2->board_str);
		  begin += (double)(clock() - _begin) / CLOCKS_PER_SEC;
		  nodes[2] = tmp2;
        }
        // Right
        if (column == node->size - 1) {
          nodes[3] = NULL;
        } else {
          Node* tmp3 = (Node *)malloc(sizeof(Node));
          tmp3->move = node->board[row * node->size + column + 1];
          tmp3->size = node->size;
          tmp3->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(tmp3->board, node->board,
                 sizeof(int) * node->size * node->size);
          tmp3->board_str = (char *)malloc(sizeof(char) * 100);
          tmp3->board_str[node->size * node->size] = '\0';
          swap(tmp3->board, row * node->size + column,
               row * node->size + column + 1);
          tmp3->parent = node;
		  clock_t _begin = clock();
		  
          int pos = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            pos += sprintf(tmp3->board_str + pos, "%d", tmp3->board[i]);
          }
          tmp3->board_str[pos] = '\0';
		  begin += (double)(clock() - _begin) / CLOCKS_PER_SEC;
		  nodes[3] = tmp3;
        }
        if (nodes[0] == NULL && nodes[1] == NULL && nodes[2] == NULL &&
            nodes[3] == NULL) {
          return NULL;
        }
        return nodes;
      }
    }
  }

  free(nodes);
  return NULL;
}

int number = 0;
char *build_graph(Node *root) {
  enqueue(queue, root);
  insert(set2, root->board_str);
  while (!is_empty(queue)) {
	// if (number++ > 1) {
	// 	return NULL;
	// }
    // printf("%d\n", number++);
    Node *node = dequeue(queue);
    if (!strcmp(node->board_str, goal)) {
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
	// free(node->board);
	// free(node->board_str);
	// free(node);
    if (nodes == NULL) {
      continue;
    }
    for (int i = 0; i < 4; i++) {
      if (nodes[i] != NULL) {
		// printBoard(nodes[i]->board, nodes[i]->size);
		// printf("%s\n", nodes[i]->board_str);
          if (is_member(set2, nodes[i]->board_str)) {
            // free(nodes[i]->board);
            // free(nodes[i]->board_str);
            // free(nodes[i]);
            continue;
          } else {
            enqueue(queue, nodes[i]);
            insert(set2, nodes[i]->board_str);
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
  goal = (char *)malloc(sizeof(char) * 100);
  int tracker = 0;
  for (int i = 1; i < k * k; i++) {
    char num[5];
    sprintf(num, "%d", i);
    for (int j = 0; j < strlen(num); j++) {
      goal[tracker++] = num[j];
    }
  }
  goal[tracker++] = '0';
  goal[tracker] = '\0';
  ////////////////////
  // do the rest to solve the puzzle
  ////////////////////
  Node *root = (Node *)malloc(sizeof(Node));
  root->size = k;
  root->board = (int *)malloc(sizeof(int) * k * k);
  memcpy(root->board, initial_board, sizeof(int) * k * k);
  root->board_str = (char *)malloc(sizeof(char) * (k * k + 1));
  for (int i = 0; i < k * k; i++) {
    char num[5];
    sprintf(num, "%d", root->board[i]);
  }
  root->board_str[k * k] = '\0';
  root->parent = NULL;
  root->move = -1;
  set = init(300000);
  set2 = init(300000);
  queue = init_queue(300000);
  printBoard(root->board, root->size);
  clock_t begin2 = clock();
  char *moves = build_graph(root);
  clock_t end = clock();
  double time_spent = (double)(end - begin2) / CLOCKS_PER_SEC;
  printf("Time spent: %f\n", time_spent);
  printf("Time spent yield node: %f\n", begin);
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
