/*----------------------------------------------------------------------* 
 * File:     palmwars.h                                                 *
 * Purpose:  Mabwars for the Palm                                       *
 * Author:   Richard Leggett                                            *
 * Language: C                                                          *
 * History:  04-Jul-01: RML: Created                                    *
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------* 
 * Constants                                                            *
 *----------------------------------------------------------------------*/
#define MAX_STARS          100
#define MAX_LINKS          15
#define MAX_PLAYERS        4
#define MAX_PLAYERNAMESIZE 10

#define PLAYER_MABS 0
#define PLAYER_NONE (MAX_PLAYERS + 1)

#define GAMESTATE_IDLE       0
#define GAMESTATE_PLAYERINIT 1
#define GAMESTATE_TURN       2
#define GAMESTATE_FINISHED   3

#define RML_CREATOR_ID        0x524D4C31 /* "RML1" */
#define FEATURE_PALMWARSSTATE 0x2424


/*----------------------------------------------------------------------* 
 * Structures                                                           *
 *----------------------------------------------------------------------*/
typedef struct
{
    char    playerNames[MAX_PLAYERS+1][MAX_PLAYERNAMESIZE+1];
    UInt8   starsOccupied[MAX_PLAYERS+1];
    UInt8   turnOrder[MAX_PLAYERS];
    UInt8   occupier[MAX_STARS];
    UInt8   armies[MAX_STARS];
    UInt8   numSystems;
    UInt8   numStars;
    UInt8   numPlayers;
    UInt8   gameState;
    UInt8   currentPlayer;
    UInt8   currentSystem;
    UInt8   turn;
    UInt8   playerMove;
    UInt8   subMove;
    UInt8   selectedStar;
    UInt8   winningPlayer;
    Boolean withMabs;
    Boolean gameOn;
    UInt8   reserved[32];
} PWGAMESTATE;

typedef struct
{
    int mapXSize;
    int mapYSize;
    int mapXMult;
    int mapYMult;
    int mapXOff;
    int mapYOff;
    int backXOff;
    int backYOff;
    int textYOff;
    int starXSize;
    int starYSize;
    int starRadius;
    int numStars;
} PWMAPDRAW;
