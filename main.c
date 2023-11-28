#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define BUFSIZE 256
#define CONSTANT 128

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
          nodes[0] = (Node *)malloc(sizeof(Node));
          nodes[0]->move = node->board[(row - 1) * node->size + column];
          nodes[0]->size = node->size;
          nodes[0]->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(nodes[0]->board, node->board,
                 sizeof(int) * node->size * node->size);
          nodes[0]->board_str = (char *)malloc(sizeof(char) * 100);
          swap(nodes[0]->board, row * node->size + column,
               (row - 1) * node->size + column);
          nodes[0]->parent = node;
          int tracker = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            char num[100];
            sprintf(num, "%d", nodes[0]->board[i]);
            for (int j = 0; j < strlen(num); j++) {
              nodes[0]->board_str[tracker++] = num[j];
            }
          }
          nodes[0]->board_str[tracker] = '\0';
        }
        // Down
        if (row == node->size - 1) {
          nodes[1] = NULL;
        } else {
          nodes[1] = (Node *)malloc(sizeof(Node));
          nodes[1]->move = node->board[(row + 1) * node->size + column];
          nodes[1]->size = node->size;
          nodes[1]->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(nodes[1]->board, node->board,
                 sizeof(int) * node->size * node->size);
          nodes[1]->parent = node;
          nodes[1]->board_str = (char *)malloc(sizeof(char) * 100);
          nodes[1]->board_str[node->size * node->size] = '\0';
          swap(nodes[1]->board, row * node->size + column,
               (row + 1) * node->size + column);
          int tracker = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            char num[100];
            sprintf(num, "%d", nodes[1]->board[i]);
            for (int j = 0; j < strlen(num); j++) {
              nodes[1]->board_str[tracker++] = num[j];
            }
          }
          nodes[1]->board_str[tracker] = '\0';
        }
        // Left
        if (column == 0) {
          nodes[2] = NULL;
        } else {
          nodes[2] = (Node *)malloc(sizeof(Node));
          nodes[2]->move = node->board[row * node->size + column - 1];
          nodes[2]->size = node->size;
          nodes[2]->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(nodes[2]->board, node->board,
                 sizeof(int) * node->size * node->size);
          nodes[2]->board_str = (char *)malloc(sizeof(char) * 100);
          nodes[2]->board_str[node->size * node->size] = '\0';
          swap(nodes[2]->board, row * node->size + column,
               row * node->size + column - 1);
          nodes[2]->parent = node;
          int tracker = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            char num[2];
            sprintf(num, "%d", nodes[2]->board[i]);
            for (int j = 0; j < strlen(num); j++) {
              nodes[2]->board_str[tracker++] = num[j];
            }
          }
          nodes[2]->board_str[tracker] = '\0';
        }
        // Right
        if (column == node->size - 1) {
          nodes[3] = NULL;
        } else {
          nodes[3] = (Node *)malloc(sizeof(Node));
          nodes[3]->move = node->board[row * node->size + column + 1];
          nodes[3]->size = node->size;
          nodes[3]->board =
              (int *)malloc(sizeof(int) * node->size * node->size);
          memcpy(nodes[3]->board, node->board,
                 sizeof(int) * node->size * node->size);
          nodes[3]->board_str = (char *)malloc(sizeof(char) * 100);
          nodes[3]->board_str[node->size * node->size] = '\0';
          swap(nodes[3]->board, row * node->size + column,
               row * node->size + column + 1);
          nodes[3]->parent = node;
          int tracker = 0;
          for (int i = 0; i < node->size * node->size; i++) {
            char num[100];
            sprintf(num, "%d", nodes[3]->board[i]);
            for (int j = 0; j < strlen(num); j++) {
              nodes[3]->board_str[tracker++] = num[j];
            }
          }
          nodes[3]->board_str[tracker] = '\0';
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

int number = 0;
char *build_graph(Node *root) {
  enqueue(queue, root);
  insert(set2, root->board_str);
  while (!is_empty(queue)) {
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
    if (nodes == NULL) {
      continue;
    }
    for (int i = 0; i < 4; i++) {
      if (nodes[i] != NULL) {
        if (is_member(set, nodes[i]->board_str)) {
          free(nodes[i]->board);
          free(nodes[i]->board_str);
          free(nodes[i]);
          continue;
        } else {
          if (is_member(set2, nodes[i]->board_str)) {
            free(nodes[i]->board);
            free(nodes[i]->board_str);
            free(nodes[i]);
            continue;
          } else {
            enqueue(queue, nodes[i]);
            insert(set2, nodes[i]->board_str);
          }
        }
      }
    }
    free(nodes);
    insert(set, node->board_str);
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
  int N = 103;
  root->board_str[k * k] = '\0';
  root->parent = NULL;
  root->move = -1;
  set = init(10000);
  set2 = init(10000);
  queue = init_queue(10000);
  char *moves = build_graph(root);
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
