#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include <MLV/MLV_all.h>
#include "../headers/structs.h"
#include "../headers/functions.h"

int waitForAction() 		/*Keep application idle until a button callBack is fired. It handle mouse hovering */
{
    int mouseX, mouseY, i, unicode;
    MLV_Event event;
    MLV_Button_state state;
    MLV_Keyboard_button keyPressed;
    int callbackValue = 0;
    
    do                                                          /*Loop until the user press a btn*/
    {
        event = MLV_get_event(&keyPressed, NULL, &unicode, NULL, NULL, &mouseX, &mouseY, NULL, &state);    /*Get last event*/
        MLV_flush_event_queue();

        if(event == MLV_MOUSE_MOTION || event == MLV_MOUSE_BUTTON || event == MLV_KEY)  /*Is this a movement of the mouse?*/
        {
            for(i = 0; i < gameObj->nbrToPrint; i++)            /*iterate through all the element to print*/
            {
                if(gameObj->toPrint[i].type == BUTTON && gameObj->toPrint[i].display)             /*is this a button ?*/
                {
                    if(isCursorOnBtn(gameObj->toPrint[i].element.btn, mouseX, mouseY))  /*Is the cursoron this button?*/
                    {
                        if(event == MLV_MOUSE_BUTTON)/*We do this if the action was a click released*/
                        {
                            gameObj->toPrint[i].state = ACTIVE;
                            if(state == MLV_RELEASED)
                            {
                                if(gameObj->toPrint[i].element.btn->simpleCallback)
                                    callbackValue = gameObj->toPrint[i].element.btn->callback;
                                else
                                    gameObj->toPrint[i].element.btn->activeCallback(gameObj->toPrint[i].element.btn->activeCallbackArgument);
                            }
                        }
                        else if(event == MLV_MOUSE_MOTION)      /*We do this if the action is just a hover*/
                        {
                            gameObj->toPrint[i].state = HOVER;   
                            if(gameObj->toPrint[i].element.btn->hoverCallback != NULL)
                            {
                                gameObj->toPrint[i].element.btn->hoverCallback(i);
                            }
                        }
                    } 
                    else
                    {
                        if(gameObj->toPrint[i].state != FORCEHOVER)
                            gameObj->toPrint[i].state = IDLE;        /*now, make sure state is idle*/
                    }
                }
                else if(gameObj->toPrint[i].type == TEXTBOX && gameObj->toPrint[i].display)  /*The event is an input box one*/
                {
                    if(event == MLV_MOUSE_BUTTON && state == MLV_RELEASED)
                    {
                        if(isCursorOnInput(gameObj->toPrint[i].element.tB, mouseX, mouseY))
                        {
                            gameObj->toPrint[i].state = FOCUS;  /*The user clicked on this inputBox, we give it the focus*/
                        }
                        else
                        {
                            gameObj->toPrint[i].state = BLUR;  /*The user didn't clicked on this input box, we make sure it is blured*/
                        }
                    }
                    else if(gameObj->toPrint[i].state == FOCUS && event == MLV_KEY && state == MLV_PRESSED)
                    {
                        updateTextBox(i, keyPressed, unicode);  /*The user typed something in the text box, we update it*/
                    }
                }
            }
        }

        printFrame();					/*Update window*/

        if(gameObj->printLogs)
            printConsumption();

    } while(callbackValue == 0);

    return callbackValue;
}

bool isCursorOnBtn(Button * Btn, int mouseX, int mouseY)	/*check if cursor is on Button. Return true if it is, false otherwise*/
{
    int x = Btn->x;                     /*Get X position of the button*/
    int y = Btn->y;                     /*Get Y position of of the button*/
    int width = Btn->width;             /*Get width of the button*/
    int height = Btn->height;           /*Get height of the button*/

    if(mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height))
        return true;
    else
        return false;
}

bool isCursorOnInput(TextBox * input, int mouseX, int mouseY)	/*check if cursor is on textBox. Return true if it is, false otherwise*/
{
    int x = input->x + input->imgOffsetX;               /*Get X position of the text box detection area*/
    int y = input->y + input->imgOffsetY;               /*Get Y position of of the text box detection area*/
    int width = input->width - input->imgOffsetX;       /*Get width of the text box detection area*/
    int height = input->height - input->imgOffsetX;     /*Get height of the text box detection area*/

    if(mouseX >= x && mouseX <= (x + width) && mouseY >= y && mouseY <= (y + height))
        return true;
    else
        return false;
}

void updateTextBox(int inputPos, MLV_Keyboard_button keyPressed, int unicode)
{
    int contentLength;
    contentLength = strlen(gameObj->toPrint[inputPos].element.tB->content);
    
    if(unicode == MLV_KEYBOARD_BACKSPACE)
    {
        if(contentLength > 0)
            gameObj->toPrint[inputPos].element.tB->content[contentLength-1] = '\0';
    }
    else if(unicode != MLV_KEYBOARD_RETURN)
    {
        if(contentLength < 100)
           strcat(gameObj->toPrint[inputPos].element.tB->content, MLV_convert_unicode_to_string(unicode));
    }
}

void incrementNumberBox(void * nB)
{
    if(((NumberBox*)nB)->value < ((NumberBox*)nB)->maxValue)
        ((NumberBox*)nB)->value++;
}

void decrementNumberBox(void * nB)
{
    if(((NumberBox*)nB)->value > ((NumberBox*)nB)->minValue)
        ((NumberBox*)nB)->value--;
}

void quitGame()                         /*This function properly end the game*/
{
    MLV_free_window();                  /*Close the window*/
    exit(0);
}

void waitForPlayer()
{
    int i, j;
    char playerName[100];
    Button * goBtn;

    cleanToPrint();

    /*Add some fog on the map*/
    for(i = 0; i < gameObj->gridSizeX; i++)
    {
        for(j = 0; j < gameObj->gridSizeY; j++)
        { 
            addToPrint(createPicture(gameObj->gridOffsetLeft+(35*i)+1, gameObj->gridOffsetTop+(35*j), "images/fog.png"), PICTURE);
            addToPrint(createPicture(gameObj->gridOffsetLeft+(35*i)+559, gameObj->gridOffsetTop+(35*j), "images/fog.png"), PICTURE);
        }
    }

    addToPrint(createPicture(0, 0, "images/waitScreen.png"), PICTURE);

    if(gameObj->currTurn == 1)
        strcpy(playerName, gameObj->player1.name);
    else
        strcpy(playerName, gameObj->player2.name);

    addToPrint(createText(0, 275, 1100, 50, playerName), TEXT);

    goBtn = createBtn(percentOffset(50, 'w', -92), 370, 183, 50, BTN_GRAPHIC);
    goBtn->idleImage = MLV_load_image("images/buttons/goBtn_idle.png");
    goBtn->hoverImage = MLV_load_image("images/buttons/goBtn_hover.png");
    goBtn->activeImage = MLV_load_image("images/buttons/goBtn_active.png");
    goBtn->callback = 1;

    addToPrint(goBtn, BUTTON);

    waitForAction();
}

/********************************************************************************/
/***** Elements creation functions **********************************************/
/********************************************************************************/

Button * createBtn(int x, int y, int width, int height, enum btnType type)  /*Create a Btn and assign specified values*/
{
    Button * btn = allocate(sizeof(Button));    /*Create an empty button*/
    btn->x = x;                                 /*Set X position of the button*/
    btn->y = y;                                 /*Set Y position of the button*/
    btn->width = width;                         /*Set width of the button*/
    btn->height = height;                       /*Set height of the button*/
    btn->type = type;                           /*Set type of the button*/
    btn->simpleCallback = true;
    btn->callback = 0;                          /*Set callback as NULL */
    btn->hoverCallback = NULL;                  /*Set hoverCallback as NULL */
    btn->activeCallbackArgument = NULL;
    btn->activeCallback = NULL;
    btn->canToggle = false;
    btn->checked = false;

    if(type == BTN_GRAPHIC)                             /*If the button is a graphic one*/
    {
        btn->idleImage = NULL;                  /*Set images as default  */
        btn->hoverImage = NULL;
        btn->activeImage = NULL;
    }
    else                                        /*Otherwise, treat as a plain color button*/
    {
        btn->idleBackColor = MLV_COLOR_BLACK;   /*Set default background color*/
        btn->hoverBackColor = MLV_COLOR_BLACK;
        btn->activeBackColor = MLV_COLOR_BLACK;
        
        btn->idleTextColor = MLV_COLOR_WHITE;   /*Set default text color*/
        btn->hoverTextColor = MLV_COLOR_WHITE;
        btn->activeTextColor = MLV_COLOR_WHITE;
    }

    return btn;
}

Picture * createPicture(int x, int y, char * fileURL)    /*Create an Image element and return it*/
{
    Picture * img = allocate(sizeof(Picture));                  /*create the element*/
    img->x = x;                                                 /*Set X position*/
    img->y = y;                                                 /*Set Y position*/
    img->image = MLV_load_image(fileURL);                       /*Load image*/
    MLV_get_image_size(img->image, &img->width, &img->height);  /*Get image height and width*/

    return img;                                                 /*Return the newly created element*/
}

Text * createText(int x, int y, int width, int height, char * content)    /*Create an Image element and return it*/
{
    Text * txt = allocate(sizeof(Text));                  /*create the element*/
    txt->x = x;                                                 /*Set X position*/
    txt->y = y;                                                 /*Set Y position*/
    txt->width = width;
    txt->height = height;
    strcpy(txt->content, content);

    return txt;                                                 /*Return the newly created element*/
}

TextBox * createTextBox(int x, int y, int width, int height, char type, char placeHolder[100])
{
    TextBox * tB = allocate(sizeof(TextBox));
    tB->x = x;
    tB->y = y;
    tB->width = width;
    tB->height = height;
    tB->type = type;
    strcpy(tB->placeHolder, placeHolder);
    tB->content[0] = '\0';

    tB->cursorShown = false;
    tB->lastCursorSwitch = 0;

    tB->textColor = gameObj->defaultInputColor;
    tB->placeHolderColor = gameObj->defaultPlaceHolderColor;

    if(type == 'g')
        tB->backColor = MLV_ALPHA_TRANSPARENT;
    else
        tB->backColor = MLV_COLOR_BLACK;

    tB->backImage = NULL;
    tB->imgOffsetX = 0;
    tB->imgOffsetY = 0;

    return tB;
}

NumberBox * createNumberBox(int x, int y, int defaultVal, int minVal, int maxVal)
{
    NumberBox * nB = allocate(sizeof(NumberBox));
    Button * plusBtn, * lessBtn;

    nB->x = x;
    nB->y = y;
    nB->value = defaultVal;
    nB->minValue = minVal;
    nB->maxValue = maxVal;

    nB->backImage = MLV_load_image("images/inputBox_small.png");

    plusBtn = createBtn(x+125, y+2, 44, 36, BTN_GRAPHIC);
    plusBtn->idleImage = MLV_load_image("images/buttons/plusBtn_small_idle.png");
    plusBtn->hoverImage = MLV_load_image("images/buttons/plusBtn_small_hover.png");
    plusBtn->activeImage = MLV_load_image("images/buttons/plusBtn_small_active.png");
    plusBtn->simpleCallback = false;
    plusBtn->activeCallbackArgument = nB;
    plusBtn->activeCallback = incrementNumberBox;

    nB->plusBtn = plusBtn;

    lessBtn = createBtn(x, y+2, 44, 36, BTN_GRAPHIC);
    lessBtn->idleImage = MLV_load_image("images/buttons/lessBtn_small_idle.png");
    lessBtn->hoverImage = MLV_load_image("images/buttons/lessBtn_small_hover.png");
    lessBtn->activeImage = MLV_load_image("images/buttons/lessBtn_small_active.png");
    lessBtn->simpleCallback = false;
    lessBtn->activeCallbackArgument = nB;
    lessBtn->activeCallback = decrementNumberBox;

    nB->lessBtn = lessBtn;

    return nB;
}


/********************************************************************************/
/***** Functions to generate the game board *************************************/
/********************************************************************************/

void createBoardGame(int w, int h)
{
    int topOffset = 250 + ((10 - h) * 35) / 2, topStep = 35;
    int leftOffset = 59 + ((10 - w) * 35) / 2, leftStep = 35;
    MLV_Image * mainBckg = MLV_load_image("images/mainBackground.png");
    MLV_Image * dirtBloc = MLV_load_image("images/dirtBloc.png");
    
    gameObj->gridOffsetLeft = leftOffset+35;
    gameObj->gridOffsetTop = topOffset+35;

    /*Left Grid*/
    createGrid('l', w, h, topOffset, topStep, leftOffset, leftStep, mainBckg, dirtBloc);

    /*Right Grid*/
    leftOffset += 595;

    createGrid('r', w, h, topOffset, topStep, leftOffset, leftStep, mainBckg, dirtBloc);

    MLV_free_image(dirtBloc);

    gameObj->gameBoard = mainBckg;
}

void createGrid(char side, int w, int h, int topOffset, int topStep, int leftOffset, int leftStep, MLV_Image * mainBckg, MLV_Image * dirtBloc)
{
    int x, y;
    int signY = 0, signX;
    MLV_Image * sign = NULL;
    char signURL[50];

    if(side == 'l')
        signX = 0;
    else
        signX = w;

    for(y = 0; y <= h; y++)
    {
        for(x = 0; x <= w; x++)
        {
            if((x == signX && y != signY) || (y == signY && x != signX))
            { 
                if(y == signY)
                {
                    if(side == 'l')
                        sprintf(signURL, "images/signs/%c.png", x+64);
                    else if(side == 'r')
                        sprintf(signURL, "images/signs/%c.png", x+65);    

                    sign = MLV_load_image(signURL);
                    MLV_draw_image_on_image	(sign, mainBckg, leftOffset+4+(x*leftStep), topOffset+(y*topStep));     
                    MLV_free_image(sign);   
                }
                else if(x == signX)
                {
                    sprintf(signURL, "images/signs/%d.png", y);
                    
                    sign = MLV_load_image(signURL);
                    if(side == 'r')
                        MLV_draw_image_on_image	(sign, mainBckg, leftOffset+6+(x*leftStep), topOffset+(y*topStep));
                    else 
                        MLV_draw_image_on_image	(sign, mainBckg, leftOffset+(x*leftStep), topOffset+(y*topStep));

                    MLV_free_image(sign);
                }

            }
            else if(x != signX && y != signY)
            {
                if(y%2 == x%2)
                    MLV_draw_image_on_image	(dirtBloc, mainBckg, leftOffset+(x*leftStep), topOffset+(y*topStep));
            }
        }
    }
}