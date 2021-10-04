#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define mapWidth 80
#define mapHeight 30

typedef struct SObject {
    float x, y;
    float width, height;
    float vertSpeed;
    BOOL LsFly;
    char cType;
    float horizSpeed;
} TObject;

char map[mapHeight][mapWidth+1];
TObject mario;
TObject *brick = NULL;
int brickLength;

TObject *moving;
int movingLength;

int level = 1;
int score;
int maxlvl;

void ClearMap() {

    for (int i = 0; i < mapWidth; i++)
        map[0][i] = ' ';
   // map[0][mapWidth] = '\0';
    for (int j = 0; j < mapHeight; j++)
        sprintf(map[j], map[0]);
}

void ShowMap() {

    map[mapHeight  - 1][mapWidth - 1] = '\0';
    for (int j = 0; j < mapHeight; j++)
        printf("%s\n", map[j]);
           // printf(" lvl %i   coins %i ", level, coins);
}

void SetObjectPos(TObject *obj, float xPos, float yPos) {
    
    (*obj).x = xPos;
    (*obj).y = yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType) {

    SetObjectPos(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).cType = inType;
    (*obj).horizSpeed = 0.3;
}

void CreateLevel(int lvl);

void PlayerDed() {

    system("color 4F");
    Sleep(1000);
    CreateLevel(level);
}

BOOL IsCollision(TObject o1, TObject o2);
void ShowPriviewLevel();
TObject *GetMewMoving();

void VertMoveObject(TObject *obj) {
 
    (*obj).LsFly = TRUE;
    (*obj).vertSpeed += 0.05;
    SetObjectPos(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);

    for (int i = 0; i < brickLength; i++) {
        if (IsCollision(*obj, brick[i])) {

            if (obj[0].vertSpeed > 0)
               (*obj).LsFly = FALSE;

            if ( (brick[i].cType == '?') && (obj[0].vertSpeed < 0) && (obj == & mario)) {

                brick[i].cType = '-';
                InitObject(GetMewMoving(), brick[i].x, brick[i].y-3, 3, 2, '$');
                moving[movingLength - 1].vertSpeed = -0.7;
            }

            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;
            
            if (brick[i].cType == '+') {

                level++;
                if (level > maxlvl) level =1;

                //Sleep(1000);
                CreateLevel(level);
                ShowPriviewLevel();
                system("color 2F");
            }
            break;
        if (brick[i].cType == '0') {

             if ((*obj).cType == brick[i].cType)
                brick[i].cType = brick[i].cType - brick[i].cType;
            }
            break;
        }
    }
}

void DeleteMoving(int i) {

    movingLength--;
    moving[i] = moving[movingLength];
    moving = realloc(moving, sizeof(*moving) *movingLength);
}

void MarioCollision() {

    for (int i = 0; i < movingLength; i++)
        if (IsCollision(mario, moving[i])) {

            if (moving[i].cType == '0') {
                
                score += 50;
                if (   (mario.LsFly == TRUE)
                    && (mario.vertSpeed > 0)
                    && (mario.y + mario.height < moving[i].y + moving[i].height *0.5)) {

                        DeleteMoving(i);
                        i--;
                        continue;
                }
                else
                    PlayerDed();
            }
            if (moving[i].cType == '$') {

                score += 100;
                DeleteMoving(i);
                i--;
                continue;
            }
        }
}

void HorizonMoveObject(TObject *obj) {

    obj[0].x += obj[0].horizSpeed;

    for (int i = 0; i < brickLength; i++)
        if (IsCollision(obj[0], brick[i])) {

            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
            return;
        }
    if (obj[0].cType == '0') {
    
       TObject tmp = *obj;
       VertMoveObject(&tmp);
       if (tmp.LsFly == TRUE) {

          obj[0].x -= obj[0].horizSpeed;
          obj[0].horizSpeed = -obj[0].horizSpeed;
        }
    }
    
}

BOOL IsPosInMap(int x, int y) {
    
    return ( (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight));
}

void PutObjectOnMap(TObject obj) {

    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < (ix + iWidth); i++)
        for (int j = iy; j <(iy + iHeight); j++)
            if (IsPosInMap(i,j))
                map[j][i] = obj.cType;
}

void setCur(int x, int y) {

    COORD coord;
    coord.X = x;
    coord.Y =y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HorizonMoveMap(float dx) {

    mario.x -= dx;
    for (int i = 0; i < brickLength; i++)
        if (IsCollision(mario, brick[i])) {

            mario.x += dx;
            return;
        }
    mario.x += dx;
    
    for (int i = 0; i < brickLength; i++)
        brick[i].x += dx;
    for (int i = 0; i < movingLength; i++)
        moving[i].x += dx;
}

BOOL IsCollision(TObject o1, TObject o2) {

    return ((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

TObject *GetMewBrick() {
    
    brickLength++;
    brick = realloc(brick, sizeof(*brick) *brickLength);
    return brick + brickLength -1;
}

TObject *GetMewMoving() {
    
    movingLength++;
    moving = realloc(moving, sizeof(*moving) *movingLength);
    return moving + movingLength -1;
}

void PutScoreOnMap() {

    char c[30];
    sprintf(c, "Score: %d", score);
    int len = strlen(c);
    for ( int i = 0; i < len; i++) {

        map[1][i+5] = c[i];
    }

}

void CreateLevel(int lvl) {

    system("color 3F");

    brickLength = 0;
    brick = realloc(brick, 0);
    movingLength = 0;
    moving = realloc(moving, 0);

    InitObject(&mario, 39, 10, 3, 3, '@');
    score = 0;

    if (lvl == 1) {
        
        InitObject(GetMewBrick(), 20, 25, 40, 5, '#');
            InitObject(GetMewBrick(), 30, 15, 5, 3, '?');
            InitObject(GetMewBrick(), 50, 15, 5, 3, '?');
        InitObject(GetMewBrick(), 60, 20, 40, 10, '#');
            InitObject(GetMewBrick(), 60, 10, 10, 3, '-');
            InitObject(GetMewBrick(), 70, 10, 5, 3, '?');
            InitObject(GetMewBrick(), 75, 10, 5, 3, '-');
            InitObject(GetMewBrick(), 80, 10, 5, 3, '?');
            InitObject(GetMewBrick(), 85, 10, 10, 3, '-');
        InitObject(GetMewBrick(), 100, 25, 20, 5, '#');
        InitObject(GetMewBrick(), 120, 20, 10, 10, '#');
        InitObject(GetMewBrick(), 150, 25, 40, 5, '#');
        InitObject(GetMewBrick(), 210, 20, 10, 10, '#');
        InitObject(GetMewBrick(), 230, 20, 10, 5, '+');

        InitObject(GetMewMoving(), 25, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 80, 15, 3, 2, '0');
        InitObject(GetMewMoving(), 151, 15, 3, 2, '0');
        InitObject(GetMewMoving(), 189, 15, 3, 2, '0');
    }
    if (lvl == 2) {
        
        InitObject(GetMewBrick(), 20, 25, 40, 5, '#');
        InitObject(GetMewBrick(), 60, 20, 10, 10, '#');
        InitObject(GetMewBrick(), 80, 25, 20, 5, '#');
        InitObject(GetMewBrick(), 120, 20, 10, 10, '#');
        InitObject(GetMewBrick(), 150, 25, 40, 5, '#');
        InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
        InitObject(GetMewBrick(), 230, 15, 50, 3, '#');
        InitObject(GetMewBrick(), 300, 22, 45, 15, '#');
        InitObject(GetMewBrick(), 315, 18, 74, 8, '#');
        InitObject(GetMewBrick(), 409, 16, 5, 3, '#');
        InitObject(GetMewBrick(), 435, 22, 43, 10, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
  //    InitObject(GetMewBrick(), 200, 22, 10, 3, '#');
        InitObject(GetMewBrick(), 468, 17, 10, 5, '+');
        
        InitObject(GetMewMoving(), 25, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 81, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 155, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 190, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 213, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 233, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 279, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 302, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 320, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 344, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 388, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 415, 10, 3, 2, '0');
    }
     if (lvl == 3) {
        
        InitObject(GetMewBrick(), 20, 25, 40, 5, '#');
        InitObject(GetMewBrick(), 75, 25, 15, 5, '#');
        InitObject(GetMewBrick(), 100, 25, 20, 5, '#');
        InitObject(GetMewBrick(), 135, 23, 30, 7, '#');
        InitObject(GetMewBrick(), 140, 30, 44, 10, '#');
        InitObject(GetMewBrick(), 180, 22, 25, 3, '#');
        InitObject(GetMewBrick(), 222, 25, 30, 10, '+');
        
        InitObject(GetMewMoving(), 25, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 74, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 99, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 135, 10, 3, 2, '0');
        InitObject(GetMewMoving(), 184, 10, 3, 2, '0');
    }
    maxlvl = 3;
}

void ShowPriview() {

    system("cls");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n \t\t\t\t  MARIO \n\n\n\n\t\t\tControl  \n\n\t\t\tLeft  A \n\n\t\t\tRight  D \n\n\t\t\tJump Space");
    Sleep(10000);
    system("cls");
}

void ShowPriviewLevel() {

    system("cls");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n \t\t\t\t  LEVEL %d", level);
    Sleep(1000);
    system("cls");
}

int main() {
    
    CreateLevel(level);
    
   // ShowPriview();
   // ShowPriviewLevel();

    do {

        ClearMap();

        if ( (mario.LsFly == FALSE) && (GetKeyState(VK_SPACE) < 0)) mario.vertSpeed = -1;
        if (GetKeyState('A') < 0) HorizonMoveMap(1);
        if (GetKeyState('D') < 0) HorizonMoveMap(-1);

        if (mario.y > mapHeight) PlayerDed();

        VertMoveObject(&mario);
        MarioCollision();

        for (int i = 0; i < brickLength; i++)
            PutObjectOnMap(brick[i]);
        for (int i = 0; i < movingLength; i++) {

            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);
            if (moving[i].y > mapHeight) {

                DeleteMoving(i);
                i++;
                continue;
            }
            PutObjectOnMap(moving[i]);
        }
        PutObjectOnMap(mario);
        PutScoreOnMap();

        setCur(0, 0);
        ShowMap();

        Sleep(10);
    }
    while (GetKeyState(VK_ESCAPE) >= 0);
    
    return 0;
}

/* 0 = Черный 8 = Серый
 1 = Синий 9 = Светло-синий
 2 = Зеленый A = Светло-зеленый
 3 = Голубой B = Светло-голубой
 4 = Красный C = Светло-красный
 5 = Лиловый D = Светло-лиловый
 6 = Желтый E = Светло-желтый
 7 = Белый F = Ярко-белый*/