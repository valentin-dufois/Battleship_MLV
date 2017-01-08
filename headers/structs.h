#pragma once

#define bool int
#define true 1
#define false 0

typedef struct Ship
{
    int size;               /*Size of the ship (1, 2, 3, 4 or 5)*/
    char direction;         /*vertical or horizontal (v or h)*/
    int posX;               /*X position of the ship on the grid*/
    int posY;               /*Y position of the ship on the grid*/
    bool sinked;
    bool hits[5];           /*Details which cell of the ship has been hit;*/

} Ship;

enum cellType
{
    CELL_EMPTY,
    CELL_BOAT
};

typedef struct Cell
{
    enum cellType type;              /*Type of the cell (e -> empty, s -> ship);*/
    bool hit;               /*Has the cell been hit or not? (true or false)*/
}Cell;

typedef struct Grid
{
    int sizeX;              /*Size of the grid*/
    int sizeY;              /*Size of the grid*/
    int nbrOfShips;         /*Nbr of ships*/
    Cell cells[10][10];     /*Cells of the grid*/
    Ship * ships;           /*Ships on the grid*/
} Grid;

enum playerType
{
    PLAYER_HUMAN,
    PLAYER_AI
};

typedef struct Player
{
    enum playerType type;              /*Type of player, can be either h (human) or c (computer/ai)*/
    char score;  
    char name[100];
    Grid grid;              /*If multiple game are fdone, keep track of scores.*/
} Player;

enum elState
{
    IDLE,
    HOVER,
    FORCEHOVER,
    ACTIVE,
    FOCUS,
    BLUR
};

enum elType
{
    BUTTON,
    PICTURE,
    TEXTBOX,
    NUMBERBOX,
    TEXT
};

enum btnType
{
    BTN_GRAPHIC,
    BTN_PLAINCOLOR
};

typedef struct Button
{
    int x;                      /*x position of the button*/
    int y;                      /*y position of the button*/
    int width;                  /*width of the button*/
    int height;                 /*height of the button*/
    enum btnType type;                  /*button type. Can be either g(graphic/image) or c(plainColor)*/
    char text[100];             /*Text to display on the button*/
    bool canToggle;
    bool checked;

    /*Back colors (used when type is )*/
    MLV_Color idleBackColor;    /*Back color when button is idle*/
    MLV_Color hoverBackColor;   /*Back color when button is hovered*/
    MLV_Color activeBackColor;  /*Back color when button is pressed*/
 
    /*Text colors*/
    MLV_Color idleTextColor;    /*Text color when button is idle*/
    MLV_Color hoverTextColor;   /*Text color when button is hovered*/
    MLV_Color activeTextColor;  /*Text color when button is pressed*/

    /*images*/
    MLV_Image * idleImage;      /*Image used at idle state for graphic button*/
    MLV_Image * hoverImage;     /*Image used at hover state for graphic button*/
    MLV_Image * activeImage;    /*Image used at hover state for graphic button */

    bool simpleCallback;
    int callback;              /*value to be returned when the button is pressed*/
    void * activeCallbackArgument;
    void (*activeCallback)(void *);
    void (*hoverCallback)(int);/*function called when the button is hovered;*/

} Button;

typedef struct Picture
{
    int x;                      /*X position of the picture*/
    int y;                      /*Y position of the picture*/
    int width;                  /*Width of the picture     */
    int height;                 /*Height of the picture    */

    MLV_Image * image;          /*The picture by itself    */
} Picture;

typedef struct TextBox
{
    int x;                      /*X Position of the text box*/
    int y;                      /*Y position of the text box*/
    int width;                  /*Width of the text box*/
    int height;                 /*Height pg the text box*/
    char type;                  /*Type of the text box. Can be either c (Plain colors) or g (graphic);*/

    bool cursorShown;
    int lastCursorSwitch;    

    char placeHolder[100];      /*Placeholder to be printed on the text box*/
    char content[100];          /*Actuel content of the text box*/

    MLV_Color backColor;        /*Background color, used if type equalc c*/
    MLV_Color textColor;        /*Color of the text*/
    MLV_Color placeHolderColor; /*Color of the text*/

    MLV_Image * backImage;      /*Image to be displayed behind the textBox. Used if type equals g*/
    int imgOffsetX;             /*Image offset on the X axis*/
    int imgOffsetY;             /*Image offset on the Y axis*/
} TextBox;

typedef struct NumberBox
{
    int x;                      /*X Position of the text box*/
    int y;                      /*Y position of the text box*/

    int value;
    int minValue;
    int maxValue;

    MLV_Image * backImage;

    Button * plusBtn;
    Button * lessBtn;
} NumberBox;

typedef struct Text
{
    int x;                      /*X Position of the text box*/
    int y;                      /*Y position of the text box*/
    int width;
    int height;

    char content[256];
} Text;

typedef struct PrintElement
{
    union 
    {
        Button * btn;
        Picture * pict;
        TextBox * tB;
        NumberBox * nB;
        Text * txt;
    } element;                           /*A pointer to the element to display*/
    enum elType type;               /*Type of the element. Can be b (Button), and more to come*/
    enum elState state;             /*If needed relative to the cursor (idle, hover, active, or force hover)*/
    bool canFade;
    bool display;

} PrintElement;

typedef struct GameObj      /*The gameObj carry all the variables used by the game*/
{
    Player player1;         /*First player obj*/
    Player player2;         /*Second player obj*/
    int currTurn;           /*Current player. Can be either 1, 2 or 0 if not in game;  */
    int nbrPlayer;          /*Number of human player in the game*/
    int nbrShips[6];        /*Nbr of ships per size (1 to 5); */

    int boatBeingPlacedSize;
    int boatBeingPlacedDirection;

    int gridSizeX;          /*Nbr of cell on the width of the grid*/
    int gridSizeY;          /*Nbr of cell on the height of the grid*/
    int gridOffsetTop;      /*Top margin of the grid*/
    int gridOffsetLeft;     /*Left margin of the grid*/

    int wWidth;             /*Width of the window*/
    int wHeight;            /*Height of the window*/

    PrintElement * toPrint; /*Elements to print on the next frame;*/
    int nbrToPrint;         /*Number of elements to print*/

    MLV_Font * inputFont;   /*Font used to render input box*/
    MLV_Font * waitFont;   /*Font used to render input box*/

    MLV_Color defaultInputColor;
    MLV_Color defaultPlaceHolderColor;

    MLV_Image * woodBckg;   /*Background used for menu interface*/
    MLV_Image * gameBoard;  /*The background used in the current Game*/

    char gameState;         /*Current state of the game. Can be either 'm' (menu), 'a' (active/inGame) or 'p' (pause);*/

    bool printLogs;         /*Shall we print the logs or not ?*/

} GameObj;

GameObj * gameObj;          /*Define the gameObj accessible from anywhere in the application*/