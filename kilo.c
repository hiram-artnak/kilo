/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


/*** data ***/
struct termios orig_termios;


/*** terminal ***/
void die(const char *s){
    perror(s);
    exit(1);
}

void disableRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode(){
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    // BRKINT: Stop break condition from sending SIGINT
    // ICRNL: Stop translating carriage return to newline
    // INPCK: Stop checking parity
    // ISTRIP: Stop stripping 8th bit of each input byte
    // IXON: Stop software flow control
    raw.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON );
    // Turn off output processing
    raw.c_oflag &= ~(OPOST);
    // Character size is 8 bits per byte
    raw.c_cflag |= (CS8);
    // ECHO: Stop echoing input characters
    // ICANON: Stop canonical mode
    // IEXTEN: Stop Ctrl-V
    // ISIG: Stop sending SIGINT on Ctrl-C
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN |ISIG);
    // VMIN: Minimum number of bytes of input needed before read() can return
    raw.c_cc[VMIN] = 0;
    // VTIME: Maximum amount of time to wait before read() returns
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/
int main(){
    enableRawMode();
    char c;
    while (1){
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c)){
            pritnf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }
    
    return 0;
}
