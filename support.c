#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "lcd.h"
#include "midi.h"
#include "midiplay.h"

//extern Voice voice[VOICES];
extern int16_t* shoot;

static void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}


// Copy a subset of a large source picture into a smaller destination.
// sx,sy are the offset into the source picture.
void pic_subset(Picture *dst, const Picture *src, int sx, int sy)
{
    int dw = dst->width;
    int dh = dst->height;
    for(int y=0; y<dh; y++) {
        if (y+sy < 0)
            continue;
        if (y+sy >= src->height)
            break;
        for(int x=0; x<dw; x++) {
            if (x+sx < 0)
                continue;
            if (x+sx >= src->width)
                break;
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+sy) + x + sx];
        }
    }
}

// Overlay a picture onto a destination picture.
// xoffset,yoffset are the offset into the destination picture that the
// source picture is placed.
// Any pixel in the source that is the 'transparent' color will not be
// copied.  This defines a color in the source that can be used as a
// transparent color.
void pic_overlay(Picture *dst, int xoffset, int yoffset, const Picture *src, int transparent)
{
    for(int y=0; y<src->height; y++) {
        int dy = y+yoffset;
        if (dy < 0)
            continue;
        if (dy >= dst->height)
            break;
        for(int x=0; x<src->width; x++) {
            int dx = x+xoffset;
            if (dx < 0)
                continue;
            if (dx >= dst->width)
                break;
            unsigned short int p = src->pix2[y*src->width + x];
            if (p != transparent)
                dst->pix2[dy*dst->width + dx] = p;
        }
    }
}

extern const Picture background; // A 240x320 background image
extern const Picture player;
extern const Picture goodBullet;
extern const Picture bulletShadow;
extern const Picture badGuy;
extern const Picture shield;
extern const Picture badBullet;
extern const Picture titlecrawl;


// This C macro will create an array of Picture elements.
// Really, you'll just use it as a pointer to a single Picture
// element with an internal pix2[] array large enough to hold
// an image of the specified size.
// BE CAREFUL HOW LARGE OF A PICTURE YOU TRY TO CREATE:
// A 100x100 picture uses 20000 bytes.  You have 32768 bytes of SRAM.
#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }

void erase(int x, int y, int value)
{
    if(value ==0){
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    //pic_overlay(tmp, 5,5, &player, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
    }
    else if(value == 1){
        TempPicturePtr(tmp,12,18); // Create a temporary 29x29 image.
            pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
            //pic_overlay(tmp, 5,5, &player, 0xffff); // Overlay the ball
            LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
    }

}

void update(int x, int y, int Value)
{
    if (x < 228 && x > 15 && Value == 2) {
        LCD_DrawPicture(x-player.width/2,y-player.height/2, &player); // Draw the player
    }
    else if(Value == 1) {
        LCD_DrawPicture(x-goodBullet.width/2,y-goodBullet.height/2, &goodBullet);
    }
    else if(Value == 0) {
        LCD_DrawPicture(x-bulletShadow.width/2,(y)-bulletShadow.height/2, &bulletShadow);
        }
    else if(Value == 3){
        LCD_DrawPicture(x-badGuy.width/2,(y)-badGuy.height/2, &badGuy);
    }
    else if(Value == 4){
        LCD_DrawPicture(x-shield.width/2,(y)-shield.height/2, &shield);
    }
    else if(Value == 5){
        LCD_DrawPicture(x-badBullet.width/2,(y)-badBullet.height/2, &badBullet);
    }
    else if(Value == 6){
        LCD_DrawPicture(x-titlecrawl.width/2,(y)-titlecrawl.height/2, &titlecrawl);

    }
}

void update2(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 5,5, &player, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void rocketMan(void)
{
    // Draw the background.
    MIDI_Player *mp = midi_init(midifile);
    init_tim2(10417);

    srand(250);

    int x = 120;
    int y = 22;

    int right, left, shootah, gbX, gbY;
    int gbCheck = 0;
    int bg1Check = 1;
    int bg2Check = 1;
    int bg3Check = 1;
    int bg4Check = 1;
    int bg5Check = 1;
    int bg6Check = 1;
    int bg7Check = 1;

    int bgBullet1;
    int bgBullet2;
    int bgBullet3;
    int bgBullet4;
    int bgBullet5;
    int bgBullet6;
    int bgBullet7;

    int bgBX1;
    int bgBX2;
    int bgBX3;
    int bgBX4;
    int bgBX5;
    int bgBX6;
    int bgBX7;

    int bgBY1;
    int bgBY2;
    int bgBY3;
    int bgBY4;
    int bgBY5;
    int bgBY6;
    int bgBY7;

    int randNum;

    int shieldX = 120;
    int shieldY = 50;
    int shieldDir = 1;

    int bgX1 = 90;
    int bgY1 = 280; //initializing bad guy 1

    int bgX2 = 150;
    int bgY2 = 280; //initializing bad guy 2

    int bgX3 = 210;
    int bgY3 = 280; //initializing bad guy 3

    int bgX4 = 60;
    int bgY4 = 295; //initializing bad guy 4

    int bgX5 = 120;
    int bgY5 = 295; //initializing bad guy 5

    int bgX6 = 180;
    int bgY6 = 295; //initializing bad guy 6

    int bgX7 = 30;
    int bgY7 = 280; //initializing bad guy 7

    int alternate = 1;

    int MAX = 6400;
    for(;;)
    {
        for(int z=0; z<4; z++)
        {

            nano_wait(2000000); // wait
            right = GPIOC->IDR & 1<<6;
            left = GPIOC->IDR & 1<<7;
            shootah = GPIOC->IDR & 1<<8;

            if((shootah && 1<<8) && (gbCheck != 1) && !((x > shieldX - 20 + 10 * shieldDir) && (x < shieldX + 20 + 10 * shieldDir))) // initializing good bullet
            {
//                voice[VOICES].number = 1;
//                voice[VOICES].soundEffect = 1;
//                voice[VOICES].in_use = 1;
//                voice[VOICES].step = 1;
//                voice[VOICES].volume = 200;
                gbCheck = 1;
                gbX = x;
                gbY = 30;
            }

            if(gbCheck) //moving good bullet along
            {
                update(gbX-2, gbY + 15, 1);//bullet
                update(gbX-2, gbY+2, 0);//shadow
                gbY += 1;
            }

            if (right && 1<<7)
            {
                //nano_wait(5000000);
                x++;
                update(x, y, 2);
            }
            else if(left && 1<<6)
            {
                //nano_wait(5000000);
                x--;
                update(x, y, 2);
            }
            gbCheck = gbCheckVal(gbY);

            if(shieldX > 210){
                shieldDir = -1;
            }
            else if(shieldX < 30){
                shieldDir = 1;
            }

            if(alternate % 2){
                shieldX += shieldDir;
                update(shieldX, shieldY, 4);
            }



            //BAD GUY SHOOTING STUFF
            if(alternate % 100 == 0)
            {
              randNum = rand() % 7;
//            bgBullet1 = bgBulletCheckAndGen(bgBullet1);
//            bgBullet2 = bgBulletCheckAndGen(bgBullet2);
//            bgBullet3 = bgBulletCheckAndGen(bgBullet3);
//            bgBullet4 = bgBulletCheckAndGen(bgBullet4);
//            bgBullet5 = bgBulletCheckAndGen(bgBullet5);
//            bgBullet6 = bgBulletCheckAndGen(bgBullet6);
//            bgBullet7 = bgBulletCheckAndGen(bgBullet7);

            if(randNum == 0 && !bgBullet1)
            {
                bgBullet1 = 1;
                bgBX1 = bgX1;
                bgBY1 = bgY1 - 10;

            }
            if(randNum == 1 && !bgBullet2)
            {
                bgBullet2 = 1;
                bgBX2 = bgX2;
                bgBY2 = bgY2 - 10;
            }
            if(randNum == 2 && !bgBullet3)
            {
                bgBullet3 = 1;
                bgBX3 = bgX3;
                bgBY3 = bgY3 - 10;
            }
            if(randNum == 3 && !bgBullet4)
            {
                bgBullet4 = 1;
                bgBX4 = bgX4;
                bgBY4 = bgY4 - 10;
            }
            if(randNum == 4 && !bgBullet5)
            {
                bgBullet5 = 1;
                bgBX5 = bgX5;
                bgBY5 = bgY5 - 10;
            }
            if(randNum == 5 && !bgBullet6)
            {
                bgBullet6 = 1;
                bgBX6 = bgX6;
                bgBY6 = bgY6 - 10;
            }
            if(randNum == 6 && !bgBullet7)
            {
                bgBullet7 = 1;
                bgBX7 = bgX7;
                bgBY7 = bgY7 - 10;
             }

            }

            if(bgBullet1, bg1Check)
            {
                update(bgBX1, bgBY1, 5);
                bgBY1 -=1;
                bgBullet1 = bgBulletCheck(bgBX1, bgBY1, x, y, shieldX, shieldY);
                if(bgBullet1 == -1){
                    break;
                }
            }

            if(bgBullet2 && bg2Check)
            {
                update(bgBX2, bgBY2, 5);
                bgBY2 -=1;
                bgBullet2 = bgBulletCheck(bgBX2, bgBY2, x, y, shieldX, shieldY);
                if(bgBullet2 == -1){
                    break;
                }
            }

            if(bgBullet3 && bg3Check)
            {
                update(bgBX3, bgBY3, 5);
                bgBY3 -=1;
                bgBullet3 = bgBulletCheck(bgBX3, bgBY3, x, y, shieldX, shieldY);
                if(bgBullet3 == -1){
                    break;
                }
            }
            if(bgBullet4 && bg4Check)
            {
                update(bgBX4, bgBY4, 5);
                bgBY4 -=1;
                bgBullet4 = bgBulletCheck(bgBX4, bgBY4, x, y, shieldX, shieldY);
                if(bgBullet4 == -1){
                    break;
                }
            }
            if(bgBullet5 && bg5Check)
            {
                update(bgBX5, bgBY5, 5);
                bgBY5 -=1;
                bgBullet5 = bgBulletCheck(bgBX5, bgBY5, x, y, shieldX, shieldY);
                if(bgBullet5 == -1){
                    break;
                }
            }
            if(bgBullet6 && bg6Check)
            {
                update(bgBX6, bgBY6, 5);
                bgBY6 -=1;
                bgBullet6 = bgBulletCheck(bgBX6, bgBY6, x, y, shieldX, shieldY);
                if(bgBullet6 == -1){
                    break;
                }
            }
            if(bgBullet7 && bg7Check)
            {
                update(bgBX7, bgBY7, 5);
                bgBY7 -=1;
                bgBullet7 = bgBulletCheck(bgBX7, bgBY7, x, y, shieldX, shieldY);

                if(bgBullet7 == -1)
                {
                    break;
                }
            }



            if(bg1Check)
            {
                moveBadGuys(&bgX1, &bgY1, alternate);
                bg1Check = bgCheck(bgX1, bgY1, gbX, gbY, &gbCheck);
                if(!bg1Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg2Check)
            {
                moveBadGuys(&bgX2, &bgY2, alternate);
                bg2Check = bgCheck(bgX2, bgY2, gbX, gbY, &gbCheck);
                if(!bg2Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg3Check)
            {
                moveBadGuys(&bgX3, &bgY3, alternate);
                bg3Check = bgCheck(bgX3, bgY3, gbX, gbY, &gbCheck);
                if(!bg3Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg4Check)
            {
                moveBadGuys(&bgX4, &bgY4, alternate);
                bg4Check = bgCheck(bgX4, bgY4, gbX, gbY, &gbCheck);
                if(!bg4Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }
            if(bg5Check)
            {
                moveBadGuys(&bgX5, &bgY5, alternate);
                bg5Check = bgCheck(bgX5, bgY5, gbX, gbY, &gbCheck);
                if(!bg5Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg6Check)
            {
                moveBadGuys(&bgX6, &bgY6, alternate);
                bg6Check = bgCheck(bgX6, bgY6, gbX, gbY, &gbCheck);
                if(!bg6Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            if(bg7Check)
            {
                moveBadGuys(&bgX7, &bgY7, alternate);
                bg7Check = bgCheck(bgX7, bgY7, gbX, gbY, &gbCheck);
                if(!bg7Check){
                    gbCheck = 0;
                    while(gbY<325)
                    {
                        update(gbX-2, ++gbY, 0);
                    }
                }
            }

            alternate = altInc(alternate);

            asm("wfi");
            if (mp->nexttick == MAXTICKS)
            {
            mp = midi_init(midifile);
            }
        }
    }
}

int bgCheck(int, int, int, int, int*);

int bgCheck(int bgX, int bgY, int gbX, int gbY, int* gbCheck) {
    if(gbX > (bgX - 7) && gbX < (bgX + 7)) //if bgX-5 < gbX < bgX + 5
        {
        if(gbY > (bgY - 15) && gbY < (bgY)) {
            erase(bgX, bgY, 1);
            *gbCheck = 0;
            return 0;
        }
        }


    return 1;
}

int bgBulletCheck(int BX, int BY, int PX, int PY, int shieldX, int shieldY){
    BX+=5;
    if(BX < shieldX + 15 && BX > shieldX - 15 && BY > shieldY && BY < shieldY + 20){
//    if(BX > PX - 10 || BX < PX + 10){
//         //GAME OVER animation
//        return -1;
//    }
        nano_wait(1000);
        BY += 20;
        while(BY>50)
        {
            update(BX+2, --BY, 0);
        }
        return 0;

        }
    if(BY < -20){
        return 0;
    }

    return 1;



}


int altInc(int inc) {
    nano_wait(1000);
    inc++;
    if(inc == 6400)
    {
       inc = 0;
    }
    return inc;
}

int gbCheckVal(int val) {
    if(val > 325)
    {
        return 0;
    }
    return 1;
}


void generateGame(void) {
    LCD_DrawPicture(0,0,&background);

    update(120,22,2); //initializing spaceship

    update(90, 280, 3); //initializing bad guys
    update(150, 280, 3);
    update(210, 280, 3);
    update(60, 295, 3); //initializing bad guys
    update(120, 295, 3);
    update(180, 295, 3);

    update(120, 50, 4);
}


void titleScreen(void){
    LCD_DrawPicture(0,0,&background);
    int titlex = 120;
    int titley = -50;

    while(titley < 400){
        update(titlex,titley,6);
        nano_wait(4000000000);
        titley++;
    }
//    while(true){
//
//    }

    LCD_DrawPicture(0,0,&background);

}

void basic_drawing(void)
{
    LCD_Clear(0);
    LCD_DrawRectangle(10, 10, 30, 50, GREEN);
    LCD_DrawFillRectangle(50, 10, 70, 50, BLUE);
    LCD_DrawLine(10, 10, 70, 50, RED);
    LCD_Circle(50, 90, 40, 1, CYAN);
    LCD_DrawTriangle(90,10, 120,10, 90,30, YELLOW);
    LCD_DrawFillTriangle(90,90, 120,120, 90,120, GRAY);
    LCD_DrawFillRectangle(10, 140, 120, 159, WHITE);
    LCD_DrawString(20,141, BLACK, WHITE, "Test string!", 16, 0); // opaque background
}

void moveBadGuys(int *x, int *y, int alternate, int MAX)
{

    switch(alternate)
    {
    case 100:
        *y += 2;
        update(*x,*y,3);
        break;
    case 200:
        *x += 2;
        update(*x,*y,3);
        break;
    case 300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 400:
        *x += 2;
        update(*x,*y,3);
        break;
    case 500:
        *y += 2;
        update(*x,*y,3);
        break;
    case 600:
        *x += 2;
        update(*x,*y,3);
        break;
    case 700:
        *y -= 2;
        update(*x,*y,3);
        break;
    case 800:
        *x += 2;
        update(*x,*y,3);
        break;
    case (900):
        *y += 2;
        update(*x,*y,3);
        break;
    case 1000:
        *x += 2;
        update(*x,*y,3);
        break;
    case 1100:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 1200:
        *x += 2;
        update(*x,*y,3);
      break;
    case 1300:
        *y += 2;
        update(*x,*y,3);
      break;
    case 1400:
        *x += 2;
        update(*x,*y,3);
      break;
    case 1500:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 1600:
        *x += 2;
        update(*x,*y,3);
      break;
    case 1700:
        *y += 2;
        update(*x,*y,3);
      break;
    case 1800:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 1900:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 2000:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 2100:
        *y += 2;
        update(*x,*y,3);
      break;
    case 2200:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 2300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 2400:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 2500:
        *y += 2;
        update(*x,*y,3);
      break;
    case 2600:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 2700:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 2800:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 2900:
        *y += 2;
        update(*x,*y,3);
      break;
    case 3000:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 3100:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 3200:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 3300:
        *y += 2;
        update(*x,*y,3);
      break;
    case 3400:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 3500:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 3600:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 3700:
        *y += 2;
        update(*x,*y,3);
      break;
    case 3800:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 3900:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 4000:
        *x -= 2;
        update(*x,*y,3);
        break;
    case 4100:
        *y += 2;
        update(*x,*y,3);
      break;
    case 4200:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 4400:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4500:
        *y += 2;
        update(*x,*y,3);
      break;
    case 4600:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4700:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 4800:
        *x -= 2;
        update(*x,*y,3);
      break;
    case 4900:
        *y += 2;
        update(*x,*y,3);
        break;
    case 5000:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5100:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 5200:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5300:
        *y += 2;
        update(*x,*y,3);
        break;
    case 5400:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5500:
        *y -= 2;
        update(*x,*y,3);
        break;
    case 5600:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5700:
        *y += 2;
        update(*x,*y,3);
        break;
    case 5800:
        *x += 2;
        update(*x,*y,3);
        break;
    case 5900:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 6000:
        *x += 2;
        update(*x,*y,3);
      break;
    case 6100:
        *y += 2;
        update(*x,*y,3);
      break;
    case 6200:
        *x += 2;
        update(*x,*y,3);
      break;
    case 6300:
        *y -= 2;
        update(*x,*y,3);
      break;
    case 0:
        *x += 2;
        update(*x,*y,3);
        break;  }
//nano_wait(1000);
//update(*x,*y,3);
}
