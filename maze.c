#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "maze.h"


// functions declaration
void print_horizontal_frame(maze_t *maze);
maze_t *create_maze_stract(int cols, int rows, location_t* player_loc, location_t* target_loc, location_t *portal_zero_loc, location_t *portal_nine_loc);
bool handle_input(maze_t *maze);
char ***tiles_malloc(int rows, int cols);
void fill_tail(maze_t *maze, char *arr);
bool check_move_horizontal(int currenti, int newj, maze_t *maze);
bool check_move_vertical(int currentj,int newi, maze_t *maze);
void print_tile(char* color, char* symbol);
maze_t *read_maze(char* read_maze);
bool check_specials(int currenti, int currentj, maze_t* maze);
void one_side_door(int currenti, int currentj, maze_t* maze);
bool use_portal(maze_t* maze);
void check_if_stands_on_key(maze_t* maze);
void delete_maze(maze_t* maze);

char *msg_quit[] = {"Bye!", "Farewell!", "Goodbye!"}; // should be const
char *msg_normal[] = {"Press A/S/D/W to move, Q to quit!", "To move press A/S/D/W, to quit predd Q!"};
char *msg_start[] = {"Welcome!", "Hi!", "Hello Hello!"}; // should be const
char *msg_hit[] = {"Ouch!", "Oy!", "Ooch!"};
char *msg_win[] = {"You Win!", "You are the king!", "You are rich now!"};
char *msg_unknown[] = {"Unknown key pressed", "I don't understand you", "You can't use this key"};

int main(int argc, char* argv[])
{
    srand(time(NULL));
    set_terminal(true);
    maze_t *maze = read_maze(argv[1]);
    print_maze(maze);

    bool game_over = false;
    do {
        game_over = handle_input(maze);
    } while (!game_over);
    delete_maze(maze);
    set_terminal(false);
    return 0;
}

#define puts_no_newline(s) fputs(s, stdout)

void print_maze(maze_t *maze)
{
    static unsigned frame_num; // this variable retains its value between calls
    // clear terminal
    puts_no_newline(CLEAR_SCREEN);
    // print frame number and message
    printf("%-*.*s%4d\n", maze->outer_walls_cols-2, maze->outer_walls_cols-2,  maze->message, frame_num++);
    // print header row
    print_horizontal_frame(maze);
    putchar('\n');
    // print maze rows (including player)
    for (int i = 0; i < maze->outer_walls_rows; i++) {
        puts_no_newline(S_WALL);
        for (int j = 0; j < maze->outer_walls_cols; j++) {

            if (i == maze->player_location->pos_i && j == maze->player_location->pos_j) {
                if(0 != strcmp(maze->tiles[i][j], T_FLOOR)) {
                    print_tile(COLOR_BLUE, maze->tiles[i][j]);
                } else {
                     puts_no_newline(S_PLAYER);
                }
            } else if (i == maze->target_location->pos_i && j == maze->target_location->pos_j) {
                    puts_no_newline(S_TARGET);
            } else {
                    print_tile(COLOR_LT_PURPLE, maze->tiles[i][j]) ;  
            }  
        }
        puts_no_newline(S_WALL);
        putchar('\n');
    }
    // print footer row
    print_horizontal_frame(maze);
    putchar('\n');
}
void print_tile(char* color, char* symbol) 
{
    printf ("%s", color);
    puts_no_newline(symbol);
    printf("%s", COLOR_OFF);
}
void fill_tail(maze_t *maze, char *arr)
{
    for (int i = 0; i < maze->outer_walls_rows ; i++) {
        for (int j = 0; j < maze->outer_walls_cols ; j++) {
            int num = j + (i * (maze->outer_walls_cols));
            maze->tiles[i][j] = arr[num] == '#' ? T_WALL         : 
                                arr[num] == '|' ? T_VERT_WALL    :
                                arr[num] == '-' ? T_HORIZ_WALL   : 
                                arr[num] == '<' ? T_ONE_S_DOOR_R :
                                arr[num] == '>' ? T_ONE_S_DOOR_L :
                                arr[num] == 'v' ? T_ONE_S_DOOR_U :
                                arr[num] == '^' ? T_ONE_S_DOOR_D :
                                arr[num] == '0' ? T_PORTAL_ZERO  :
                                arr[num] == '9' ? T_PORTAL_NINE  : 
                                arr[num] == '+' ? T_DOOR         :
                                arr[num] == '&' ? T_KEY          : T_FLOOR;
        }
    }
}
void print_horizontal_frame(maze_t *maze)
{
    for (int j = 0; j < maze->outer_walls_cols + 2; j++)
        puts_no_newline(S_WALL);
}

maze_t *create_maze_stract(int cols, int rows, location_t* player_loc, location_t* target_loc, location_t *portal_zero_loc, location_t *portal_nine_loc)
{
    maze_t *maze = malloc(sizeof(maze_t));
    if(NULL == maze)
    {
        printf ("Maze malloc faild");
        free(maze);
        exit(1);
    }
    maze->message = malloc(sizeof(char)*256);
    if(NULL == maze->message)
    {
        printf ("Message malloc faild");
        free(maze->message);
        free (maze);
        exit(1);   
    }
    maze->player_location = malloc(sizeof(location_t));
    if(NULL == maze->player_location)
    {
        printf ("Player location malloc faild");
        free(maze->player_location);
        free(maze->message);
        free (maze);
        exit(1);
    }
    maze->target_location = malloc(sizeof(location_t));
    if(NULL == maze->player_location)
    {
        printf ("Target location malloc faild");
        free(maze->target_location);
        free(maze->player_location);
        free(maze->message);
        free (maze);
        exit(1);
    }
    if (NULL == (maze->tiles = tiles_malloc(rows, cols))) {
        printf ("Tiles malloc faild");
        free(maze->tiles);
        free(maze->target_location);
        free(maze->player_location);
        free(maze->message);
        free (maze);
        exit(1);
    }
    maze->portal_zero_loc = malloc(sizeof(location_t));
    if(NULL == maze->portal_zero_loc)
    {
        printf ("Portal zero malloc faild");
        free(maze->portal_zero_loc);
        free(maze->tiles);
        free(maze->target_location);
        free(maze->player_location);
        free(maze->message);
        free (maze);
        exit(1);
    }
    maze->portal_nine_loc = malloc(sizeof(location_t));
    if(NULL == maze->portal_nine_loc)
    {
        printf ("Portal nine malloc faild");
        free(maze->portal_nine_loc);
        free(maze->portal_zero_loc);
        free(maze->tiles);
        free(maze->target_location);
        free(maze->player_location);
        free(maze->message);
        free (maze);
        exit(1);
    }
    maze->outer_walls_cols = cols;
    maze->outer_walls_rows = rows;
    maze->message = MSG_START;
    maze->player_location->pos_i = player_loc->pos_i;
    maze->player_location->pos_j = player_loc->pos_j;
    maze-> target_location->pos_i= target_loc->pos_i;
    maze-> target_location->pos_j = target_loc->pos_j;
    maze->portal_zero_loc->pos_i = portal_zero_loc->pos_i;
    maze->portal_zero_loc->pos_j = portal_zero_loc->pos_j;
    maze->portal_nine_loc->pos_i = portal_nine_loc->pos_i;
    maze->portal_nine_loc->pos_j = portal_nine_loc->pos_j;
    maze->portal_available = true;
    maze->keys_count = 0;
    return maze;
}
char ***tiles_malloc(int rows, int cols)
{
    int i = 0;
    char ***tiles;
    if( NULL == (tiles =(char***) malloc((rows) * sizeof (char**)))) {
        free(tiles);
        return NULL;
    }
    for (i = 0; i < rows; i ++) {
       if(NULL == (tiles[i] =(char**) malloc((cols) * sizeof(char*)))) {
           free(tiles[i]);
           int j = 0;
           for (j = 0; j < i; j ++) {
               free(tiles[j]);
           }
           return NULL;
        }
        else {
            for (int j = 0; j < cols; j++)
            {
               if(NULL == (tiles[i][j] = (char *)malloc (20 * sizeof(char)))) {
                   free(tiles[i][j]);
                   for (int q = 0; q < i; q++) {
                       for (int m = 0; m < i; m++) {
                           free (tiles[q][m]);
                           return NULL;
                        }
                   } 
               }
            }  
        }
    }
    return tiles;
}
void free_maze(maze_t *maze)
{
    free(maze);
}

bool handle_input(maze_t *maze)
{
    bool game_over = false;
    maze->message = MSG_NORMAL;
        // get user input
    int key = getchar();
    switch(key) {
        case EOF:
        case 'q':
        case 'Q':   // exit game
            maze->message = MSG_QUIT;
            game_over = true;
            break;
        case 'a':
        case 'A':   // go left
            if (!check_specials(maze->player_location->pos_i, maze->player_location->pos_j, maze)) {
                if(check_move_horizontal(maze->player_location->pos_i, maze->player_location->pos_j - 1, maze)) {
                    maze->player_location->pos_j--;
                }
            }
            break;
        case 'd':
        case 'D':   // go right
            if (!check_specials(maze->player_location->pos_i, maze->player_location->pos_j, maze)) {
                if(check_move_horizontal(maze->player_location->pos_i, maze->player_location->pos_j + 1, maze)) {
                    maze->player_location->pos_j++;
                }
            }
            break;
        case 'w':
        case 'W':   // go up
            if (!check_specials(maze->player_location->pos_i, maze->player_location->pos_j, maze)) {
                if(check_move_vertical(maze->player_location->pos_j,maze->player_location->pos_i - 1, maze)) {
                    maze->player_location->pos_i--;
                } 
            }
            break;
        case 's':
        case 'S':  // go down
            if (!check_specials(maze->player_location->pos_i, maze->player_location->pos_j, maze)) {
                if(check_move_vertical(maze->player_location->pos_j,maze->player_location->pos_i + 1, maze)) {
                    maze->player_location->pos_i++;
                }
            }
            break;
        default:
            maze->message = MSG_UNKNOWN;
                
     }
    if(maze->player_location->pos_i == maze->target_location->pos_i &&
         maze->player_location->pos_j == maze->target_location->pos_j && !game_over) {
        game_over = true;
        maze->message = MSG_WIN;
    } 
    check_if_stands_on_key(maze);
    // print maze
    print_maze(maze);
    return game_over;
}
void check_if_stands_on_key(maze_t* maze)
{
     if (0== strcmp(maze->tiles[maze->player_location->pos_i][maze->player_location->pos_j], T_KEY))  {
         maze->tiles[maze->player_location->pos_i][maze->player_location->pos_j] = S_FLOOR;
         maze->keys_count ++;
         maze->message = MSG_KEY;
     }
}
// move left/right
bool check_move_horizontal(int currenti, int newj, maze_t *maze)
{
    bool can_move = true;
    //check current position

    // check new position 
     if(0 > newj || maze->outer_walls_cols == newj) {
        can_move = false;
        maze->message = MSG_HIT;
        return can_move;
    } else {
        can_move = strcmp(maze->tiles[currenti][newj], T_WALL) ==  0      ?   false :   
                   strcmp(maze->tiles[currenti][newj], T_VERT_WALL) == 0  ?   false : true;
        if(!can_move) {
            maze->message = MSG_HIT;
        }             
    } 
    if (strcmp(maze->tiles[currenti][newj], T_DOOR) == 0) {
        if(maze->keys_count == 0) {
            can_move = false;
            maze->message = MSG_DOOR;
        } else
        {
            maze->keys_count --;
            maze->tiles[currenti][newj] = S_FLOOR;
            can_move = true;
        }
    }                            
    return can_move;
}
// move up/down
bool check_move_vertical(int currentj,int newi, maze_t *maze)
{
    bool can_move = true;
    if(0 > newi || maze->outer_walls_rows  == newi) {
        can_move = false;
        maze->message = MSG_HIT;
        return can_move;
    }
    can_move = strcmp(maze->tiles[newi][currentj], T_WALL) ==  0      ?   false :
               strcmp(maze->tiles[newi][currentj], T_HORIZ_WALL) == 0 ?   false : 
               strcmp(maze->tiles[newi][currentj], T_DOOR) == 0 ?   false : true; 
        if(!can_move) {
            maze->message = MSG_HIT;
        }   

    if (strcmp(maze->tiles[newi][currentj], T_DOOR) == 0) {
        if(maze->keys_count == 0) {
            can_move = false;
            maze->message = MSG_DOOR;
        } else {
            maze->keys_count --;
            maze->tiles[newi][currentj] = S_FLOOR;
            can_move = true;
        }
    }                                      
    return can_move;
}

bool check_specials(int currenti, int currentj, maze_t* maze)
{
    bool special = true;
    //check on what the player stands now
    if (0 != strcmp(maze->tiles[currenti][currentj]," ") && 
        0 != strcmp(maze->tiles[currenti][currentj],"|") &&
        0 != strcmp(maze->tiles[currenti][currentj],"-")) {
        if (0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_D)
             || 0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_U)
             || 0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_R)
             || 0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_L)) {

            one_side_door(currenti, currentj, maze);

        } else if (0 == strcmp(maze->tiles[currenti][currentj],"9") || 0 == strcmp(maze->tiles[currenti][currentj],"0")){
            if(use_portal(maze)) {
                maze->portal_available = false;
            } else {
                maze->portal_available = true;
                special = false;
            }  
        }
    } else {
        special = false;
    }
    return special;
}

void one_side_door(int currenti, int currentj, maze_t* maze)
{
    if(0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_D)) {
        maze->player_location->pos_i ++;
    } else if (0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_U)) {
        maze->player_location->pos_i --;
    } else if (0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_R)) {
        maze->player_location->pos_j ++;
    } else if(0 == strcmp(maze->tiles[currenti][currentj],T_ONE_S_DOOR_L)){
        maze->player_location->pos_j --;
    }
}

bool use_portal(maze_t* maze) {
    if(maze->portal_available) {
        if(maze->player_location->pos_i == maze->portal_zero_loc->pos_i &&
            maze->player_location->pos_j == maze->portal_zero_loc->pos_j) {
                // jump to portal 9
                maze->player_location->pos_i = maze->portal_nine_loc->pos_i;
                maze->player_location->pos_j = maze->portal_nine_loc->pos_j;
        } else {
            //jump to portal 0
            maze->player_location->pos_i = maze->portal_zero_loc->pos_i;
            maze->player_location->pos_j = maze->portal_zero_loc->pos_j;
        }
        maze->message = MSG_PORTAL;
        return maze->portal_available = true;
        return maze->portal_available;
    }
    maze->portal_available = false;
    return maze->portal_available;
}

maze_t *read_maze(char* read_maze)
{
    // maze paramerers
    int maze_cols = 0;
    int maze_rows = 0;
    int player_posi = 0;
    int player_posj = 0;
    int target_posi = 0;
    int target_posj = 0;
    int nine_posi = 0;
    int nine_posj = 0;
    int zero_posi = 0;
    int zero_posj = 0;

    // file parameters
    FILE *fp_r = NULL;
    char tav = '0';
    fp_r = fopen(read_maze, "r");
    if (fp_r == NULL) {
        printf("Maze file did not open. Exit.\n");
        exit(1);
    }
    fscanf(fp_r, "%c", &tav);
    // skip the first line
    while ('\n' != tav)
    {
        fscanf(fp_r, "%c", &tav);
    }
    //get maze size
    fscanf(fp_r, "%d", &maze_rows);
    fscanf(fp_r, "%d", &maze_cols);
    // copy the maze map
    char maze_tiles[maze_rows][maze_cols];
    int row_count = 0;
    int col_count = 0;
    //fscanf(fp_r, "%c", &tav);
    fscanf(fp_r, "%c", &tav);
    while (!feof(fp_r))
    {
        fscanf(fp_r, "%c", &tav);
        if('@' == tav) {
            player_posi = row_count;
            player_posj = col_count;
            maze_tiles[row_count][col_count] = ' ';
        } else if ('$' == tav) {
            target_posi = row_count;
            target_posj = col_count;
            maze_tiles[row_count][col_count] = ' ';
        } else if('\n' == tav)
        {
            row_count ++;
            col_count = -1;
        }
         else if ('#' == tav || '|' == tav || '-' == tav || ' ' == tav || 
                  '^' == tav || 'v' == tav || '<' == tav || '>' == tav ||
                  '&' == tav || '+' == tav || '9' == tav || '0' == tav) {
            if('9' == tav) {
                nine_posi = row_count;
                nine_posj = col_count;
            }
            if('0' == tav) {
                zero_posi = row_count;
                zero_posj =col_count;
            }
            maze_tiles[row_count][col_count] = tav;
        } else {
            printf("Unknown symbol %c in the maze map. Exit.\n", tav);
            fclose(fp_r);
            exit(1);
        }
        col_count ++;
    }
    location_t player_loc = {.pos_i = player_posi, .pos_j = player_posj};
    location_t targer_loc = {.pos_i = target_posi, .pos_j = target_posj};
    location_t nine_loc = {.pos_i = nine_posi, .pos_j = nine_posj};
    location_t zero_loc = {.pos_i = zero_posi, .pos_j = zero_posj};
    maze_t *maze = create_maze_stract(maze_cols, maze_rows, &player_loc, &targer_loc, &zero_loc, &nine_loc);
    if (NULL == maze)
    {
        exit(1);
    }
    fill_tail(maze, (char*)maze_tiles);

    fclose(fp_r); 
    return maze;
}

void delete_maze(maze_t* maze) 
{
    for (int i = 0; i < maze->outer_walls_rows; i++)
    {
        free(maze->tiles[i]);
    }
    free(maze->tiles);
    free(maze->player_location);
    free(maze->target_location);
    free(maze->portal_zero_loc);
    free(maze->portal_nine_loc);
    free(maze);
}