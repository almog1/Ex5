#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <termio.h>
#include <stdlib.h>

#define LENGTH 20
#define WIDTH 20
#define ZERO 0
#define ONE 1
#define TWO 2
#define QUIT 'q'

#define SIGN '*'
#define SPACE ' '
#define TETRIS '-'

struct sigaction act;
int isQuit;
char board[WIDTH][LENGTH];

//point for x&y
typedef struct point {
    int x;
    int y;
} Point;

Point currentLeft, currentRight, currentMid;

void removeOld() {
    board[currentLeft.x][currentLeft.y] = SPACE;
    board[currentMid.x][currentMid.y] = SPACE;
    board[currentRight.x][currentRight.y] = SPACE;

}

void changeBoard() {
    board[currentLeft.x][currentLeft.y] = TETRIS;
    board[currentMid.x][currentMid.y] = TETRIS;
    board[currentRight.x][currentRight.y] = TETRIS;
}

void initCurrents() {
    int mid;
    mid = WIDTH / 2;
    currentMid.x = mid;
    currentMid.y = ZERO;

    currentLeft.x = mid - ONE;
    currentLeft.y = ZERO;

    currentRight.x = mid + ONE;
    currentRight.y = ZERO;
}

void printBoard() {
    int i, j;

    //high - one in the begging and one in the end
    for (j = ZERO; j < LENGTH; j++) {
        for (i = ZERO; i < WIDTH; i++) {
            write(1, &board[i][j], 1);
        }
        printf("\n");
    }
}

void aKeyHandler() {
    //need to move to the left
    //check if curr left isn't in the leftest
    if (currentLeft.x > ONE) {
        system("clear");
        removeOld();
        //can left it
        currentLeft.x = currentLeft.x - ONE;
        currentMid.x = currentMid.x - ONE;
        currentRight.x = currentRight.x - ONE;
        changeBoard();
        printBoard();
    }
}

void dKeyHandler() {
    //need to move to the right
    //check if curr right isn't in the rightest
    if (currentRight.x < (WIDTH - TWO)) {
        system("clear");
        removeOld();
        //can left it
        currentLeft.x = currentLeft.x + ONE;
        currentMid.x = currentMid.x + ONE;
        currentRight.x = currentRight.x + ONE;
        changeBoard();
        printBoard();
    }
}

//change direction
void wKeyHandler() {
    //check current dir
    if (currentLeft.y != currentRight.y) {
        //in different rows
        if (currentMid.x > ONE && currentMid.x < (WIDTH - TWO)) {
            system("clear");
            removeOld();
            //can put in the side
            //left is the low
            currentLeft.y = currentMid.y;
            currentLeft.x = currentLeft.x - ONE;

            currentRight.y = currentMid.y;
            currentRight.x = currentRight.x + ONE;

            changeBoard();
            printBoard();
        }
    } else {
        //in the same row
        if (currentLeft.y < (LENGTH - TWO) && currentLeft.y > ZERO) {
            system("clear");
            removeOld();
            //make sure +1 to the y will not out of the line
            currentRight.x = currentMid.x;
            currentRight.y = currentRight.y - ONE;

            currentLeft.x = currentMid.x;
            currentLeft.y = currentLeft.y + ONE;

            changeBoard();
            printBoard();
        }
    }

}


//also for alarm
void alarmHandler() {
    signal(SIGALRM, alarmHandler);
    alarm(ONE);
    system("clear");

    removeOld();

    //need to move down
    //left is the most low
    if (currentLeft.y < (LENGTH - TWO)) {
        //can left it
        currentLeft.y = currentLeft.y + ONE;
        currentMid.y = currentMid.y + ONE;
        currentRight.y = currentRight.y + ONE;
    } else {
        initCurrents();
    }
    changeBoard();
    printBoard();
}

void sKeyHandler() {
    //if can move down
    if (currentLeft.y < (LENGTH - TWO)) {
        system("clear");
        removeOld();
        //can left it
        currentLeft.y = currentLeft.y + ONE;
        currentMid.y = currentMid.y + ONE;
        currentRight.y = currentRight.y + ONE;
        changeBoard();
        printBoard();
    }
}

void createBoard() {
    int i, j;
    //high - one in the begging and one in the end
    for (j = ZERO; j < LENGTH; j++) {
        for (i = ZERO; i < WIDTH; i++) {
            if ((j == LENGTH - ONE) ||
                (i == ZERO || i == (WIDTH - ONE))) {
                //if it is first or last
                board[i][j] = SIGN;
                //write(1,SIGN,1);
            } else {
                board[i][j] = SPACE;
                //   write(1,SPACE,1);
            }
        }
    }
}

void alarm_hand(int sig) {
    signal(SIGALRM, alarm_hand);
    alarm(ONE);
    printf("got an alarm wakeup signal\n");
    //alarm(ONE);
}

//function get char
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

void sighandler(int signum, siginfo_t *info, void *ptr) {

    //when get signal
    char input;
    char buff[1];
    //take the char from input - pipe
    // input = getch();
    //   input = getchar();

    int num = read(0, buff, 1);

    input = (char) (buff[0]);
    //wait until get from input (changed before to pipe)
    // while (input != 'q') {
    //  printf("Received signal %d\n", signum);
    // printf("CHAR %c\n", input);


    //  }
    // printf("Signal originates from process %lu\n", (unsigned long) info->si_pid);
    if (input == QUIT) {
        isQuit = 0;
    } else {
        if (input == 'a') {
            aKeyHandler();
        } else if (input == 's') {
            sKeyHandler();
        } else if (input == 'w') {
            wKeyHandler();
        } else if (input == 'd') {
            dKeyHandler();
        }
    }

}


void initCurr() {
    int mid = WIDTH / 2;
    mid = 17;
    currentMid.x = mid;
    currentMid.y = LENGTH - 3;

    currentLeft.x = mid;
    currentLeft.y = LENGTH - 2;

    currentRight.x = mid;
    currentRight.y = LENGTH - 4;
}

int main() {
    char input;
    //  currentLeft = (Point *)(malloc(sizeof(Point)));
    createBoard();
    initCurrents();
    // initCurr();
    changeBoard();
    //initCurr();
    removeOld();
    wKeyHandler();
    changeBoard();

    // alarmHandler();
    //changeBoard();

    //  removeOld();
    // wKeyHandler();
    // alarmHandler();
    // changeBoard();

//    removeOld();
    // wKeyHandler();
    //  changeBoard();
    // aKeyHandler();
    sigset_t block_mask;
    sigset_t signalMask;
    isQuit = 1;
    //create board
    printBoard();
    signal(SIGALRM, alarmHandler);
    alarm(ONE);

    //signal are blocked?
    // sigfillset (&block_mask);

    //clear the mask
    sigemptyset(&signalMask);
    //act.sa_mask = block_mask;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sighandler;
    while (isQuit) {
        sigaction(SIGUSR2, &act, NULL);
    }
    printf("BYE, World!\n");
    system("clear");
    return 0;
}