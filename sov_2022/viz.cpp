#include <ncurses.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

const int CL_NORMAL = 1;
const int CL_CONTROLS = 2;
const int CL_TAKEN = 3;
const int CL_FREE = 4;
const int CL_HIGHLIGHT = 5;
const int CL_YES = 6;
const int CL_NO = 7;
const int CL_ALTTAKEN = 8;

const int EF_FIRSTFIT = 0;
const int EF_LASTFIT = 1;
const int EF_BESTFIT = 2;
const int EF_WORSTFIT = 3;
const int EF_LENGTH = 4;


const int F_ALLOCATING = 1;
const int F_COMPACTING = 2;
const int F_CULLING = 4;

const char* const EF_NAMES[] = {
    "First fit",
    "Last fit",
    "Best fit",
    "Worst fit"
};

const char* outputName = "commandFifo";
const char* inputName = "stateFifo";

int row, col; 
int mainStartX = 0;
int mainStartY = 0;
int mainWidth = 0;
int mainHeight = 0;

int footerStartX = 0;
int footerStartY = 0;
int footerWidth = 0; 
int footerHeight = 0;
WINDOW* mw = NULL;
WINDOW* fw = NULL;
int maxInRow;
volatile bool stop = false;

void resizeHandler(){
    if(mw != NULL && fw != NULL){
        wclear(mw);
        wclear(fw);
        destroy_win(mw);
        destroy_win(fw);

        mainStartX = 0;
        mainStartY = 0;
        mainWidth = col;
        mainHeight = row - 3;

        footerStartX = 0;
        footerStartY = row-3;
        footerWidth = col; 
        footerHeight = 3;

        maxInRow = col / 17;
        maxInRow = maxInRow - 2;  

        mw = create_newwin(mainHeight, mainWidth, mainStartY, mainStartX);
        fw = create_newwin(footerHeight, footerWidth, footerStartY, footerStartX);
        wrefresh(mw);
        wattron(fw, COLOR_PAIR(CL_CONTROLS));
        mvwprintw(fw, 1, 1, " F1 ");
        wattroff(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, "Zaustavi program    ");
        wattron(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, " F2 ");
        wattroff(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, "Vrsta algoritma alokacije ");
        wattron(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, " F3 ");
        wattroff(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, "Kompakcija memorije");
        wattron(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, " F4 ");
        wattroff(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, "Ukljuci/iskljuci alokaciju");
        wattron(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, " F5 ");
        wattroff(fw, COLOR_PAIR(CL_CONTROLS));
        wprintw(fw, "Ukljuci/iskljuci LRU     ");
        wrefresh(fw);
        wrefresh(fw);
        stop = true;
    }
}

int main(){
    int output;
    int input; 
    char buffer[8192];
    printf("Startujem vizuelizator. Obavezno startujte ovo, a onda glavni program u v modu.\n");
    mkfifo(outputName, 0600);
    mkfifo(inputName, 0600);
    printf("Fajlovi napravljeni...\n");
    printf("Sada startujte glavni program u drugoj konzoli. Cekam...\n");
    output = open(outputName, O_WRONLY);
    input = open(inputName, O_RDONLY);
    printf("Nadjeno!\n");
    initscr();
    noecho();
    raw();
    start_color();
    init_pair(CL_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(CL_CONTROLS, COLOR_CYAN, COLOR_MAGENTA);
    init_pair(CL_FREE, COLOR_WHITE, COLOR_GREEN);
    init_pair(CL_TAKEN, COLOR_WHITE, COLOR_BLUE);
    init_pair(CL_HIGHLIGHT, COLOR_BLUE, COLOR_CYAN);
    init_pair(CL_YES, COLOR_GREEN, COLOR_BLACK);
    init_pair(CL_NO, COLOR_RED, COLOR_BLACK);
    init_pair(CL_ALTTAKEN, COLOR_BLACK, COLOR_CYAN);

    getmaxyx(stdscr, row, col);
    mainStartX = 0;
    mainStartY = 0;
    mainWidth = col;
    mainHeight = row - 3;

    footerStartX = 0;
    footerStartY = row-3;
    footerWidth = col; 
    footerHeight = 3;

    mw = create_newwin(mainHeight, mainWidth, mainStartY, mainStartX);
    fw = create_newwin(footerHeight, footerWidth, footerStartY, footerStartX);

    wattron(fw, COLOR_PAIR(CL_CONTROLS));
    mvwprintw(fw, 1, 1, " F1 ");
    wattroff(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, "Zaustavi program    ");
    wattron(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, " F2 ");
    wattroff(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, "Vrsta algoritma alokacije ");
    wattron(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, " F3 ");
    wattroff(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, "Kompakcija memorije");
    wattron(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, " F4 ");
    wattroff(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, "Ukljuci/iskljuci alokaciju");
    wattron(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, " F5 ");
    wattroff(fw, COLOR_PAIR(CL_CONTROLS));
    wprintw(fw, "Ukljuci/iskljuci LRU     ");
    wrefresh(fw);
    wrefresh(fw);
    
    while(1){
        wclear(mw);
        int len;
        read(input, &len, 4); 
        read(input, buffer, len);
        char type = buffer[0];
        char waiting = buffer[1];
        char flags = buffer[2];
        char unit = buffer[3];
        unsigned int pages = *((unsigned int*)(buffer + 4));
        unsigned int size = *((unsigned int*)(buffer + 8));
        unsigned int contiguous = *((unsigned int*)(buffer + 12));
        int fields = (len - 16) / 3;
        maxInRow = col / 17;
        maxInRow = maxInRow - 2;
        if(maxInRow == 0) maxInRow = 1;
        int displayRows = fields / (maxInRow * 8); 
        mvwprintw(mw, 1, 4, "Vrsta algoritma alokacije: %s", EF_NAMES[type]);
        wprintw(mw, "    Niti koje cekaju: %d", waiting);
        if(flags & F_ALLOCATING){
            wattron(mw, COLOR_PAIR(CL_YES));
            wprintw(mw, "    Alokacija aktivna  ");
            wattroff(mw, COLOR_PAIR(CL_YES));
        }else{
            wattron(mw, COLOR_PAIR(CL_NO));
            wprintw(mw, "    Alokacija neaktivna");
            wattroff(mw, COLOR_PAIR(CL_NO));
        }

        if(flags & F_COMPACTING){
            wattron(mw, COLOR_PAIR(CL_YES));
            wprintw(mw, "    Kompakcija aktivna  ");
            wattroff(mw, COLOR_PAIR(CL_YES));
        }else{
            wattron(mw, COLOR_PAIR(CL_NO));
            wprintw(mw, "    Kompakcija neaktivna");
            wattroff(mw, COLOR_PAIR(CL_NO));
        }

        if(flags & F_CULLING){
            wattron(mw, COLOR_PAIR(CL_YES));
            wprintw(mw, "    LRU aktivan  ");
            wattroff(mw, COLOR_PAIR(CL_YES));
        }else{
            wattron(mw, COLOR_PAIR(CL_NO));
            wprintw(mw, "    LRU neaktivan");
            wattroff(mw, COLOR_PAIR(CL_NO));
        }

        wprintw(mw, "       Preostalo: %d u 4K stranicama, oko: %d%c. Najduzi region je %d 4K stranice", pages, size, unit, contiguous);

        int rc = 0;
        int gc = 0;
        int crow = 5;
        wmove(mw, crow, 4);
        for(int i = 16; i < len;i += 3){
            if(i > 8192) break; 
            int t = buffer[i]; 
            int p = buffer[i+1];
            int s = buffer[i+2];
            int color;
            if(t == 0){
                color = CL_FREE;
                wattron(mw, COLOR_PAIR(color));
                wprintw(mw, "  ");
                wattroff(mw, COLOR_PAIR(color));
            }else{            
                color = (t == 1) ? CL_TAKEN : CL_HIGHLIGHT;
                wattron(mw, COLOR_PAIR(color));
                wprintw(mw, "%d%c", p, (s + 'A'));
                wattroff(mw, COLOR_PAIR(color));
            }
            gc++;
            if(gc == 8){
                wprintw(mw, " ");
                gc = 0;
                rc++;
                if(rc > maxInRow){
                    rc = 0;
                    crow += 2;
                    wmove(mw, crow, 4);
                }
            }
        }        
        wtimeout(mw, 0);
        int rez = wgetch(mw);
        if(rez == KEY_F(1)){
            write(output, "qq", 2);
            endwin();
            close(output);
            close(input);
            unlink(inputName);
            unlink(outputName);
            return 0;
        }else if(rez == KEY_F(2)){
            char b[2];
            b[0] = 'c';
            type = (type + 1) % EF_LENGTH;
            b[1] = (char)type;
            write(output, b, 2);
        }else if(rez == KEY_F(3)){
            write(output, "d2", 2);
        }else if(rez == KEY_F(4)){
            write(output, "a2", 2);
        }else if(rez == KEY_F(5)){
            write(output, "n2", 2);
        }
        else if(rez == KEY_RESIZE){
            row = LINES;
            col = COLS;
            resizeHandler();
        }
    }

    endwin();
    return 0;
}


WINDOW *create_newwin(int height, int width, int starty, int startx){	
    WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
    keypad(local_win, TRUE);
    wborder(local_win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
	return local_win;
}

void destroy_win(WINDOW *local_win){	
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}