#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {
  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_rows = 18;

  state->board = malloc(sizeof(char*) * state->num_rows);

  for (unsigned int i = 0; i < state->num_rows; i++) {
    state->board[i] = malloc(sizeof(char) * 21); 
    for (unsigned int j = 0; j < 20; j++) {
      if (i == 0 || i == 17) {
        state->board[i][j] = '#';  
      } else if (j == 0 || j == 19) {
        state->board[i][j] = '#';  
      } else {
        state->board[i][j] = ' ';  
      }
    }
    state->board[i][20] = '\0'; 
  }

  state->board[2][9] = '*';

  state->board[2][2] = 'd';
  state->board[2][3] = '>';
  state->board[2][4] = 'D';

  state->num_snakes = 1;
  state->snakes = malloc(sizeof(snake_t));
  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live = true;

  return state;
}


/* Tarea 2 */
void free_state(game_state_t* state) {
  // TODO: Implementar esta funcion.
  if (state == NULL) return;

  for (unsigned int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }

    free(state->board);
    free(state->snakes);
    free(state);
  return;
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implementar esta funcion.
  for (unsigned int i = 0; i < state->num_rows; i++) {
    fprintf(fp,"%s\n", state->board[i]);
  }
  return;
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */
bool is_tail(char c);
bool is_head(char c);
bool is_snake(char c);
char body_to_tail(char c);
char head_to_body(char c);
unsigned int get_next_row(unsigned int cur_row, char c);
unsigned int get_next_col(unsigned int cur_col, char c);

/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  // TODO: Implementar esta funcion
  return c == 'w'|| c == 'a' || c == 's' || c == 'd';
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  // TODO: Implementar esta funcion.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implementar esta funcion.
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>';
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.

  if (c == '^' || c == 'W') return 'w';
  if (c == '<' || c == 'A') return 'a';
  if (c == 'v' || c == 'S') return 's';
  if (c == '>' || c == 'D') return 'd';
  return ' ';
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  if (c == 'W') {
    return '^';  
  } else if (c == 'A') {
    return '<';  
  } else if (c == 'S') {
    return 'v';  
  } else if (c == 'D') {
    return '>'; 
  } else {
    return '?'; 
  }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  fprintf(stderr, "get_next_row: cur_row = %u, c = %c\n", cur_row, c); 
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  fprintf(stderr, "get_next_col: cur_col = %u, c = %c\n", cur_col, c); 
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t *snake = &state->snakes[snum];
  char head = get_board_at(state, snake->head_row, snake->head_col);
  unsigned int next_row = get_next_row(snake->head_row, head);
  unsigned int next_col = get_next_col(snake->head_col, head);

  char next = get_board_at(state, next_row, next_col);

  fprintf(stderr, "[Debug] Snake %u head: %c at (%u, %u) -> next (%u, %u): %c\n",
          snum, head, snake->head_row, snake->head_col, next_row, next_col, next);
  return next;
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* snake = &state->snakes[snum];
  
  char head = get_board_at(state, snake->head_row, snake->head_col);
  char body = head_to_body(head);
  unsigned int next_row = get_next_row(snake->head_row, head);
  unsigned int next_col = get_next_col(snake->head_col, head);

  set_board_at(state, snake->head_row, snake->head_col, body);
 
  set_board_at(state, next_row, next_col, head);

  snake->head_row = next_row;
  snake->head_col = next_col;
  return;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {

  snake_t *snake = &state->snakes[snum];
  if (!snake->live) return;

  unsigned int tail_row = snake->tail_row;
  unsigned int tail_col = snake->tail_col;
  char tail_char = get_board_at(state, tail_row, tail_col);

  set_board_at(state, tail_row, tail_col, ' ');

  int new_row = (int) tail_row;
  int new_col = (int) tail_col;

  if (tail_char == 'w') new_row--;
  else if (tail_char == 's') new_row++;
  else if (tail_char == 'a') new_col--;
  else if (tail_char == 'd') new_col++;

  char body_char = get_board_at(state, (unsigned int)new_row, (unsigned int)new_col);
  char new_tail_char = body_to_tail(body_char);
  set_board_at(state, (unsigned int)new_row, (unsigned int)new_col, new_tail_char);

  snake->tail_row = (unsigned int) new_row;
  snake->tail_col = (unsigned int) new_col;

}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    snake_t* snake = &state->snakes[i];
    if (!snake->live) {
      continue;
    }

    unsigned int head_row = snake->head_row;
    unsigned int head_col = snake->head_col;
    char head_char = get_board_at(state, head_row, head_col);

    unsigned int next_row = (unsigned int) get_next_row(head_row, head_char);
    unsigned int next_col = (unsigned int) get_next_col(head_col, head_char);
    char next_tile = get_board_at(state, (unsigned int)next_row, (unsigned int)next_col);

    if (next_tile == '#' || is_snake(next_tile)) {
       snake->live = false;
       set_board_at(state, head_row, head_col, 'x'); 
       continue;
    }

    set_board_at(state, head_row, head_col, head_to_body(head_char));
    set_board_at(state, next_row, next_col, head_char);
    snake->head_row = next_row;
    snake->head_col = next_col;

    if (next_tile == '*') {
       if (add_food != NULL) {
           add_food(state); 
       }
    } else {
        update_tail(state, i); 
    }
  } 
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir archivo: %s\n", filename);
        return NULL;
    }

    game_state_t* state = malloc(sizeof(game_state_t));
    if (state == NULL) {
        fprintf(stderr, "Error: no se pudo asignar memoria para el estado\n");
        fclose(fp);
        exit(1);
    }
    state->board = NULL;
    state->num_rows = 0;
    state->snakes = NULL;
    state->num_snakes = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    unsigned int num_snakes = 0;

    while ((read = getline(&line, &len, fp)) != -1){ 
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        char **new_board = realloc(state->board, sizeof(char *) * (state->num_rows + 1));
        if (new_board == NULL) {
            fprintf(stderr, "Error: no se pudo realocar memoria para el tablero\n");
            free(line);
            fclose(fp);
            exit(1);
        }
        state->board = new_board;

        char *row_copy = malloc(read + 1);
        if (row_copy == NULL) {
            fprintf(stderr, "Error: no se pudo asignar memoria para una fila\n");
            free(line);
            fclose(fp);
            exit(1);
        }
        strcpy(row_copy, line);
        state->board[state->num_rows] = row_copy;
        state->num_rows++;

        for (unsigned int i = 0; i < read; i++) {
            char c = line[i];
            if (c == 'W' || c == 'A' || c == 'S' || c == 'D'){
                num_snakes++;
            }
        }
    }

    free(line);
    fclose(fp);

    state->num_snakes = num_snakes;
    state->snakes = malloc(num_snakes * sizeof(snake_t));

    return state;         

}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
    snake_t* snake = &state->snakes[snum];
    unsigned int row = snake->tail_row;
    unsigned int col = snake->tail_col;
    char c = state->board[row][col];
    
    while (true) {
        row = get_next_row(row, c);
        col = get_next_col(col, c);
        c = state->board[row][col];

        if (c == 'W' || c == 'A' || c == 'S' || c == 'D') {
            break;
        }
    }


    snake->head_row = row;
    snake->head_col = col;
    return snake;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
    state->num_snakes = 0;

    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
            char head_char = state->board[i][j];

            if (head_char == 'W' || head_char == 'A' || head_char == 'S' || head_char == 'D') {
                snake_t* snake = &state->snakes[state->num_snakes++];

                unsigned int row = i;
                unsigned int col = j;
                char current = head_char;

                unsigned int tail_row = 0;
                unsigned int tail_col = 0;
                bool found_tail = false;

                while (true) {
                    row = get_next_row(row, current);
                    col = get_next_col(col, current);
                    current = state->board[row][col];


                    if (current == 'w' || current == 'a' || current == 's' || current == 'd') {
                        tail_row = row;
                        tail_col = col;
                        found_tail = true;
                        break;
                    }

                    if (current != '>' && current != '<' && current != '^' && current != 'v') {
                        printf("[ERROR] Cuerpo de la serpiente inválido en (%u, %u): %c\n", row, col, current);
                        found_tail = false;
                        break;
                    }
                }
                if (found_tail) {
                    snake_t* snake = &state->snakes[state->num_snakes++];
                    snake->head_row = i;
                    snake->head_col = j;
                    snake->tail_row = tail_row;
                    snake->tail_col = tail_col;
                    snake->live = true;
                }
            }
        }
    }           
    return state;       
}
