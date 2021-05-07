#include <stdbool.h>

#define MSG_NORMAL  msg_normal[rand() % (sizeof(msg_normal) / sizeof(msg_normal[0]))] 
#define MSG_START  msg_start[rand() % (sizeof(msg_start) / sizeof(msg_start[0]))] 
#define MSG_QUIT msg_quit[rand() % (sizeof(msg_quit) / sizeof(msg_quit[0]))] 
#define MSG_HIT msg_hit[rand() % (sizeof(msg_hit) / sizeof(msg_hit[0]))] 
#define MSG_WIN msg_win[rand() % (sizeof(msg_win) / sizeof(msg_win[0]))] 
#define MSG_UNKNOWN msg_unknown[rand() % (sizeof(msg_unknown) / sizeof(msg_unknown[0]))] 
#define MSG_PORTAL "Wheee!"
#define MSG_KEY "I found a key!"
#define MSG_DOOR "The door is locked..." 

typedef struct {
    unsigned pos_i;
    unsigned pos_j;
} location_t;

typedef struct {
	location_t *player_location;
    location_t *target_location;
    location_t *portal_zero_loc;
    location_t *portal_nine_loc;
    bool portal_available;
    
    char *message;
    int outer_walls_cols;
    int outer_walls_rows;
    int keys_count;
    char ***tiles;
} maze_t;

void set_terminal(bool start);
void print_maze(maze_t *maze);

/************************************
 * TERMINAL CODES ("MAGIC STRINGS") *
 ************************************/

#define COLOR_OFF       "\e[0m"
#define COLOR_BLACK     "\e[0;30m"
#define COLOR_GRAY      "\e[1;30m"
#define COLOR_RED       "\e[0;31m"
#define COLOR_LT_RED    "\e[1;31m"
#define COLOR_GREEN     "\e[0;32m"
#define COLOR_LT_GREEN  "\e[1;32m"
#define COLOR_BROWN     "\e[0;33m"
#define COLOR_YELLOW    "\e[1;33m"
#define COLOR_BLUE      "\e[0;34m"
#define COLOR_LT_BLUE   "\e[1;34m"
#define COLOR_PURPLE    "\e[0;35m"
#define COLOR_LT_PURPLE "\e[1;35m"
#define COLOR_CYAN      "\e[0;36m"
#define COLOR_LT_CYAN   "\e[1;36m"
#define COLOR_LT_GRAY   "\e[0;37m"
#define COLOR_WHITE     "\e[1;37m"

#define CLEAR_SCREEN    "\e[2J\e[1;1H"

/*****************************
 * SYMBOLS FOR GAME ELEMENTS *
 *****************************/

#define S_FLOOR     " "
#define S_PLAYER    COLOR_BLUE "@" COLOR_OFF
#define S_WALL      COLOR_LT_PURPLE "#" COLOR_OFF
#define S_TARGET    COLOR_GREEN "$" COLOR_OFF

#define T_FLOOR             " "
#define T_WALL              "#" 
#define T_VERT_WALL         "|" 
#define T_HORIZ_WALL        "-" 
#define T_ONE_S_DOOR_R      "<"
#define T_ONE_S_DOOR_L      ">"
#define T_ONE_S_DOOR_U      "v"
#define T_ONE_S_DOOR_D      "^"
#define T_PORTAL_ZERO       "0"
#define T_PORTAL_NINE       "9"
#define T_DOOR             COLOR_YELLOW"+"COLOR_OFF
#define T_KEY              COLOR_YELLOW"&"COLOR_OFF
