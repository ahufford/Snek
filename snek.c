#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>
#include "snakeTiles.c"
#include <rand.h>


#define BG_SPRITE  0x70
unsigned char bgSnakeTile;

#define BG_EMPTY  0x71
unsigned char bgEmptyTile;

//structs
struct Segment {
    uint8_t x;
    uint8_t y;

};

//Globals
const uint8_t headId = 0;
const uint8_t snakeTile = 0;

const uint8_t foodTile = 1;
const uint8_t foodId = 1;

uint8_t playing;

//snek
struct Segment body[100];
uint8_t headX;
uint8_t headY;
int8_t l;
int8_t vX;
int8_t vY;

//food
uint8_t foodX;
uint8_t foodY;


//methods
void clearScreen();
void wait(uint8_t loops);
void loadSprites();
void moveSnek();
void getInput();
void newFood();
void addBody();
void checkCollision();
void gameOver();
void newGame();
void clearBG();

void main() {

    loadSprites();
    
    printf("\n\n\n\n\n\n\n        Snek!\n     By VaderOG");

    //Use the user input to generate a seed #
    waitpad(0xFF);
    uint16_t seed = LY_REG;
    seed |= (uint16_t)DIV_REG << 8;
    initrand(seed);


    while (1) {

        newGame();

        uint8_t cycle = 0;
        while(playing){

            getInput();

            if (headY == foodY && headX == foodX) {
                //we got a food
                newFood();
                addBody();
            }

            if (cycle == 0){
                moveSnek();
                checkCollision();

            }
            
            cycle++;
            if(cycle == 255){
                cycle = 0;
            }
        }

        gameOver();
        waitpad(0xFF);

    }



}

void newGame() {
    headX = 1;
    headY = 10;
    vX = 1;
    vY = 0;
    l = 0;
    playing = 1;

    clearBG();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    newFood();
}

void checkCollision() {
    //OOB
    if (headX < 1 || headX > 20 || headY < 1 || headY > 19){
        playing = 0;
        return;
    }
    int8_t i;
    for(i = 0; i < l; i++) {
        if (i != 0) {
            if (headX == body[i].x && headY == body[i].y) {
                playing = 0;
                return; 
            }
        }

    }
}

void addBody(){
    struct Segment newPart;
    newPart.x = headX;
    newPart.y = headY;
    body[l] = newPart;
    l++;
}

void moveSnek() {

    int8_t i;
    
    for(i = l; i > 0; i--) {

        if (i == 1){
            body[i - 1].x = headX;
            body[i - 1].y = headY;
 
        } else {
            body[i - 1].x = body[i - 2].x;
            body[i - 1].y = body[i - 2].y;

        }
    }
    


    headX += vX;
    headY += vY;

    move_sprite(headId, headX * 8, headY * 8);

    for(i = 0; i < l; i++) {
        set_bkg_tiles(body[i].x - 1 , body[i].y -2, 1, 1, &bgSnakeTile);
        if (i == l - 1){
            set_bkg_tiles(body[i].x - 1, body[i].y - 2, 1, 1, &bgEmptyTile);
        }
    }

}

void newFood() {
    foodX = (((uint8_t) rand()) % (uint8_t) 20) + 1u;
    foodY = (((uint8_t) rand()) % (uint8_t) 18) + 2u;
    move_sprite(foodId, foodX * 8, foodY * 8);

    int8_t i;
    for (i = 0; i < l; i++){
        if (foodX == body[i].x && foodY == body[i].y){
            newFood();
        }
    }

}

void gameOver() {
    HIDE_SPRITES;
    int8_t i;
    for (i = 0; i < 24; i++){
        printf("\n");
    }
    SHOW_BKG;
    printf("     Game over!\n     Score: %d", l);
    for (i = 0; i < 10; i++){
        printf("\n");
    }
    for (i = 0; i < l; i++){
        move_sprite(i + 2, 0, 0);
    }
}

void getInput() {
    if( (joypad() & J_LEFT) && vX != 1){
        vX = -1;
        vY = 0;
    }
    if( (joypad() & J_RIGHT) && vX != -1){
        vX = 1;
        vY = 0;
    }
    if( (joypad() & J_DOWN) && vY != -1){
        vX = 0;
        vY = 1;
    }
    if( (joypad() & J_UP) && vY != 1){
        vX = 0;
        vY = -1;
    }
}

void loadSprites() {
    //Load snake tile and food tile
    set_sprite_data(0, 2, snakeTiles);

    set_sprite_tile(headId, snakeTile);
    set_sprite_tile(foodId, foodTile);

    set_bkg_data(0x70, 1, snakeTiles);

    bgSnakeTile = BG_SPRITE;
    bgEmptyTile = BG_EMPTY;
    
    SHOW_BKG;

    //move_sprite(headId, 10, 10);
}

void wait(uint8_t loops){
    uint8_t i;
    for(i = 0; i < loops; i++){
        wait_vbl_done();
    }
}

void clearBG(){
    int8_t i;
    int8_t j;
    for(i = 0; i < 20; i++) {
        for(j = 0; j < 18; j++) {
            set_bkg_tiles(i, j, 1, 1, &bgEmptyTile);
        }
    }
}

