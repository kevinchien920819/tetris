#pragma comment(lib, "-lwinmm");
#pragma comment(lib, "WINMM.LIB")
#include <stdio.h>
#include <windows.h>
#include <MMsystem.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
//some element length set
#define HEIGHT 14
#define WIDTH 18
#define block_h 4
#define block_w 4
#define block_type_num 7
//kbhit ascii  set 
#define LEFT 75 
#define RIGHT 77 
#define UP 72  
#define DOWN 80
#define SPACE 32
#define p 112 
#define P 80 
#define ESC 27
#define R 82
#define r 114
//set some variable
int speed; //sleep speed
int level;  //level to change easy to hard
int level_goal; //change the level untill fufill level goal
int cnt; //store the level count
int score; //score now
int last_score; //last time play score 
int best_score; //best score

HANDLE hand;
int cur_x,cur_y;//the cursor now
//block now and next
int block_cur[block_h][block_w]={0};
int block_hold[block_h][block_w]={0};
int screen[HEIGHT][WIDTH]={0};
//7 type of block
int blockO[block_h][block_w]={
	{0,0,0,0},
	{0,1,1,0},
	{0,1,1,0},
	{0,0,0,0}
};
int blockL[block_h][block_w]={
	{0,1,0,0},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};
int blockJ[block_h][block_w]={
	{0,0,1,0},
	{0,0,1,0},
	{0,1,1,0},
	{0,0,0,0}
};
int blockI[block_h][block_w]={
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0}
};
int blockZ[block_h][block_w]={
	{0,0,0,0},
	{0,1,1,0},
	{1,1,0,0},
	{0,0,0,0}
};
int blockS[block_h][block_w]={
	{0,0,0,0},
	{1,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};
int blockT[block_h][block_w]={
	{0,0,0,0},
	{1,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};
enum block_type{
	O,J,L,I,Z,S,T
};
//the fuction use
void starttetris();
void endgame();
void setCursorVisible(int v);
void gotoxy(int x, int y);
static void set_timer(int t);
static void close_timer();
void printxy(char c, int x, int y);
void printScreen();
void printBlock(int block[][block_w],int x,int y);
void eraseBlock(int block[][block_w],int x,int y);
int get_block_y_fill(int row);
int get_block_x_fill(int col);
void get_block_left_right_margin(int *left_margin ,int *right_margin);
void get_block_top_button_margin(int *top_margin ,int *button_margin);
int isCollision(int x , int y);
void draw_map();
int is_line_fill(int h);
void clear_one_line(int h);
void status_reflection(int line);
void gameover();
void check_line();
void land_block();
void copy_block(int block_s[][block_w],int block_d[][block_w]);
void rotate_block();
void make_new_block();
void move_block_down();
void key_control();
void title();
void check_level();
void get_point(int line);
void play_sound();


UINT_PTR timerId;
/*cursor setting and timer set*/ 
void setCursorVisible(int v){//v = 1 will show cursor
 	CONSOLE_CURSOR_INFO info={1, v};//size1-100  v bool
 	SetConsoleCursorInfo(hand, &info);
}
//set current cursor
void gotoxy(int x, int y){
 	COORD pos={.X=x, .Y=y};
 	SetConsoleCursorPosition(hand, pos);
}
//timer set
static void set_timer(int t){
	KillTimer(NULL, timerId); //清除Timer
	timerId = SetTimer(NULL,0,t,NULL);
} 
static void close_timer(){
	KillTimer(NULL, timerId);
}

/*print out*/ 
//print c on specific x y 
void printxy(char c, int x, int y){
	gotoxy(x, y);
	printf("%c", c);
}
//print the map
void printScreen(){ 
	int i,j;
	for(i=0;i<WIDTH;i++){
		printxy('=',i+6,1);
		printxy('=',i+6,2+HEIGHT);
	}
	for(j=-1;j<HEIGHT+1;j++){
		printxy('|',5,j+2);	
		printxy('|',6+WIDTH,j+2);
	}
	for(i=0;i<WIDTH;i++){
		for(j=0;j<HEIGHT;j++){
			if(screen[j][i]==0){
				printxy(' ',i+6,j+2);	
			} 
			else{
				printxy('O',i+6,j+2);
			}
		}
	}

	
}
//print out block when blick array  = 1
void printBlock(int block[][block_w],int x,int y){
	int i,j;
	for(i=0;i<block_h;i++){
		for(j=0;j<block_w;j++){
			if(block[i][j]==1){
				printxy('O',x+6+j,y+2+i);
			} 
		}
	}
}
//set all block to empty char
void eraseBlock(int block[][block_w],int x,int y){
	int i,j;
	for(i=0;i<block_w;i++){
		for(j=0;j<block_h;j++){
			if(block[i][j]==1){
				printxy(' ',x+6+j,y+2+i);
			} 
		}
	}
}

/*collision and edge senesor*/
//check the horizon has block or not 
int get_block_y_fill(int row){
	int ans=0;
	for(int col=0;col<block_w;col++){
		if(block_cur[row][col]==1){
			ans=1;	
		 }
	}
	return ans;
}
//check the vertical has block or not 
int get_block_x_fill(int col){//判斷每個縱向有沒有方塊 
	int ans=0;
	 for(int row=0;row<block_w;row++){
	 	if(block_cur[row][col]==1){
	 		ans=1;	
		 }
	 }
	 return ans;
}

//check the white margin of left and right
void get_block_left_right_margin(int *left_margin ,int *right_margin){

	for(int col=0;col<2;col++){//判斷左邊 
		if(get_block_x_fill(col)==0){
			*left_margin=*left_margin+1;
		}
	}
	for(int col=2;col<4;col++){//判斷右邊 
		if(get_block_x_fill(col)==0){
			*right_margin=*right_margin+1;
		}
	}
}

//check the white margin of top and button
void get_block_top_button_margin(int *top_margin ,int *button_margin){
	for(int row=0;row<2;row++){//判斷上面
		if(get_block_y_fill(row)==0){
			*top_margin=*top_margin+1;
		}
	}
	for(int row=2;row<4;row++){//判斷下面 
		if(get_block_y_fill(row)==0){
			*button_margin=*button_margin+1;
		}
	}
}

//check the block current position if collision return 1 else 0
int isCollision(int x , int y){
	int top_margin=0,button_margin=0,left_margin=0,right_margin=0;
	
	get_block_top_button_margin(&top_margin,&button_margin);
	get_block_left_right_margin(&left_margin,&right_margin);
	
	if(x<0-left_margin || y<0-top_margin || x>WIDTH-block_w+right_margin || y>HEIGHT-block_h+button_margin){
		return 1;
	}
	//check the under have block or not if screen ==1 then means have block
	int w,h;
	for(w=0;w<block_w;w++){
		for(h=0;h<block_h;h++){
			if(block_cur[h][w]==1){
				if(screen[y+h][x+w]==1){
					return 1;
				}
			}
		} 
	} 
	return 0;
} 
/*map printting */
//draw out the hold page and info page
void draw_map(){ 
int y=3,x = 27;  

    gotoxy(x, y); printf(" LEVEL : %5d", level); 

    gotoxy(x, y+1); printf(" GOAL  : %5d", 10-cnt);

    gotoxy(x, y+2); printf("+-  H O L D  -+ ");

    gotoxy(x, y+3); printf("|             | ");

    gotoxy(x, y+4); printf("|             | ");

    gotoxy(x, y+5); printf("|             | ");

    gotoxy(x, y+6); printf("|             | ");

    gotoxy(x, y+7); printf("+-- -  -  - --+ "); 

    gotoxy(x, y+8); printf(" YOUR SCORE :");     

    gotoxy(x, y+9); printf("        %6d", score); 

    // gotoxy(x, y+10); printf(" LAST SCORE :");     

    // gotoxy(x, y+11); printf("        %6d", last_score);     

    gotoxy(x, y+12); printf(" BEST SCORE :");     

    gotoxy(x, y+13); printf("        %6d", best_score);     

    gotoxy(x, y+15); printf("  △   : Shift        SPACE : Hard Drop");     

    gotoxy(x, y+16); printf("◁   ▷ : Left / Right   P   : Pause   R   : Hold block");     

    gotoxy(x, y+17); printf("  ▽   : Soft Drop     ESC  : Quit");
}
//game over page 
void gameover(){
	system("cls");
	int x=5;
    int y=5;
	gotoxy(x,y+0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤"); 
	gotoxy(x,y+1); printf("▤                              ▤");
	gotoxy(x,y+2); printf("▤  +-----------------------+   ▤");
	gotoxy(x,y+3); printf("▤  |  G A M E  O V E R..   |   ▤");
	gotoxy(x,y+4); printf("▤  +-----------------------+   ▤");
	gotoxy(x,y+5); printf("▤   YOUR SCORE: %6d         ▤",score);
	gotoxy(x,y+6); printf("▤                              ▤");
	gotoxy(x,y+7); printf("▤                              ▤");
	gotoxy(x,y+8); printf("▤                              ▤");
	gotoxy(x,y+9); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
	last_score=score;
	if(score>best_score){
		FILE* file=fopen("score.dat", "wt");            
		gotoxy(x,y+6); printf("▤  ★★★ BEST SCORE! ★★★     ▤");

		if(file==0){
			gotoxy(0,0); 
			printf("FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
		}
		else{
			fprintf(file,"%d %d",last_score,score);
			fclose(file);
		}
	}
	printf("\n");
	system("taskkill /f /im Microsoft.Media.Player.exe ");
}
//title page 
void title(){
	int x=5;  
	int y=4; 
    gotoxy(x,y+0);printf("■□□□■■■□□■■□□■■");Sleep(100);
    gotoxy(x,y+1);printf("■■■□  ■□□    ■■□□■");Sleep(100);
    gotoxy(x,y+2);printf("□□□■              □■  ■");Sleep(100);
    gotoxy(x,y+3);printf("■■□■■  □  ■  □□■□□");Sleep(100);
    gotoxy(x,y+4);printf("■■  ■□□□■■■□■■□□");Sleep(100);
    gotoxy(x,y+5);printf("\n");Sleep(100);
    gotoxy(x+5,y+2);printf("T E T R I S");Sleep(100);
    gotoxy(x,y+9); printf("  △   : rotate");     
    gotoxy(x,y+10); printf("◁   ▷ : left/right");     
    gotoxy(x,y+11); printf("  ▽   : drop slowly");
    gotoxy(x,y+12); printf(" SPACE : drop instantly"); 
    gotoxy(x,y+13); printf("   P   : pause"); 
    gotoxy(x,y+14); printf("  ESC  : quit");
	gotoxy(x,y+15); printf("  R  : hold block");  
	gotoxy(x,y+16); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"); 
    gotoxy(x,y+17); printf("!BONUS FOR HARD DROPS / COMBOS!"); 
	gotoxy(x,y+18); printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
	printf("     ");
	system("pause");
 }


/*line and clear line */ 
//chech is the hole line is fill up if true return 1 else 0
int is_line_fill(int h){
    int w;
    for(w=0;w<WIDTH;w++){
        if(screen[h][w]==0){
            return 0;
        }        
    }
    return 1;
}
//clear the line h = the line going clear
void clear_one_line(int h){ 
	int w,row;	
	//copy the uper line down 
	for(row = h;row>0;row--){
		for(w=0;w<WIDTH;w++){
			screen[row][w] = screen[row-1][w];
		}
	}
	//set the top line(new one) to 0
	for(w=0;w<WIDTH;w++){
		screen[0][w] = 0;
	}
	
}
void check_line(){
    int total_line = 0; //the lune to clear 
    int h;
    //check the line
    for(h=cur_y;h<HEIGHT;h++)
    {
        if(is_line_fill(h)==1)
        {
            clear_one_line(h);
            printScreen();
            total_line++;
        }
    }
	status_reflection(total_line);
}
//combo check and reflection 
void status_reflection(int line){
    gotoxy(0,0);
	printf("                                                            ");
	switch (line){
	case 1:
		gotoxy(8,0);
		printf("get one line clear!!");
		Sleep(1000);
		system("cls");
		printScreen();
		printBlock(block_cur,cur_x,cur_y);
		cnt += line;
		draw_map();
		break;
	case 2:
		gotoxy(5,0);
		printf("excellent, two line are cleared");
		Sleep(1000);
		system("cls");
		printScreen();
		printBlock(block_cur,cur_x,cur_y);
		cnt += line;
		draw_map();
		break;
	case 3:
		gotoxy(0,0);
		printf("congradulation, three line are cleared, god like!");
		Sleep(1000);
		system("cls");
		printScreen();
		printBlock(block_cur,cur_x,cur_y);
		cnt += line;
		draw_map();

		break;
	if(line > 3){
		gotoxy(0,0);
		printf("OMG!!OMG!!, %d line are cleared, god like!",line);
		Sleep(1000);
		system("cls");
		printScreen();
		printBlock(block_cur,cur_x,cur_y);
		cnt += line;
		draw_map();
	}
	default:
		break;
	}
} 
void get_point(int line){
	score+=100*level*line; 
	gotoxy(27,12);
	// printf("        %6d", score);
}
//print out game over 

//land the block down
void land_block(){
    int w,h;
	for(w=0;w<block_w;w++){
		for(h=0;h<block_h;h++){
			if(block_cur[h][w]==1){
				 screen[cur_y+h][cur_x+w]=1;
			}
		}
	}
}

/*block control (copy、rotate、generate、move、hold ) */ 
//copy block_s to block_d
void copy_block(int block_s[][block_w],int block_d[][block_w]){
    int w,h;
    for(w=0;w<block_w;w++){
        for(h=0;h<block_h;h++){
            block_d[h][w] = block_s[h][w];
        }
    }
}
//rotate 90 degree
void rotate_block(){ 
    int temp[block_h][block_w] = {0};
    copy_block(block_cur,temp);
    int w,h;
    for(w=0;w<block_w;w++){
    	for(h=0;h<block_h;h++){
    		block_cur[h][w] = temp[w][block_h-1-h];
		}
	}
}
//generate a new block 
void make_new_block(){
    enum block_type type = (int)(rand()%block_type_num);
    //random generate
    switch(type){
        case O:
            copy_block(blockO,block_cur);
            break;
        case J:
            copy_block(blockJ,block_cur);
            break;
        case L:
            copy_block(blockL,block_cur);
            break;
        case I:
            copy_block(blockI,block_cur);
            break;
        case Z:
            copy_block(blockZ,block_cur);
            break;
        case S:
            copy_block(blockS,block_cur);
            break;
        case T:
            copy_block(blockT,block_cur);
            break;
        default:
            break;
	}
	//the position generate 
    cur_x = (WIDTH-block_w)/2;
    cur_y = 0;
    printBlock(block_cur,cur_x,cur_y);
}

//move the block
void move_block_down(){
    if(isCollision(cur_x,cur_y+1)!=1){
		eraseBlock(block_cur,cur_x,cur_y);
		cur_y++;
		printBlock(block_cur,cur_x,cur_y);
	} 
	else{
		land_block();//land
		check_line();
		make_new_block();//generate new block
		printBlock(block_cur,cur_x,cur_y);
		if(isCollision(cur_x,cur_y)==1){
			close_timer();
			gameover();
		}
	}
}
//check if there alreeady hold a block if true return 0 else 1
int  check_hold(){
	for(int i=0;i<block_w;i++){
		for(int j=0;j<block_h;j++){
			if(block_hold[i][j]!=0){
				return 0;//there is a block being holding
			} 
		}
	}
	return 1;//there is no block being holding
}
//hold the block if there have a block hold change two block else put it in hold and generate new block
void hold_block(int is_hold){
	if(is_hold == 1){
		eraseBlock(block_cur,cur_x,cur_y);
		copy_block(block_cur,block_hold);
		printBlock(block_hold,27,4);
		make_new_block();
	}
	else{
		int temp_block[block_w][block_h] = {0};
		copy_block(block_cur,temp_block);
		eraseBlock(block_cur,cur_x,cur_y);
		copy_block(block_hold,block_cur);
		eraseBlock(block_hold,27,4);
		copy_block(temp_block,block_hold);
		printBlock(block_hold,27,4);
		printBlock(block_cur,cur_x,cur_y);
	}
}
/*contriler */ 
void key_control(){
	MSG msg;
	while(1){
		if(PeekMessage(&msg,NULL,WM_TIMER,WM_TIMER,PM_REMOVE)!=0){
		    move_block_down();
		}
		if(kbhit()!=0){
			char ch=getch();
			switch(ch){
				case UP://up 
					eraseBlock(block_cur,cur_x,cur_y);
					rotate_block();
					//if collision rotate 3 time to get back the origin block
					if(isCollision(cur_x,cur_y)==1)//if collision
					{
						rotate_block();
						rotate_block();
						rotate_block();
					} 
					printBlock(block_cur,cur_x,cur_y);
					break;
				case DOWN://down
					if(isCollision(cur_x,cur_y+1)!=1){
						eraseBlock(block_cur,cur_x,cur_y);
						cur_y++;
						printBlock(block_cur,cur_x,cur_y);
					} 
					break;
				case LEFT://left
					if(isCollision(cur_x-1,cur_y)!=1){
						eraseBlock(block_cur,cur_x,cur_y);
						cur_x--;
						printBlock(block_cur,cur_x,cur_y);
					} 
					break;
				case RIGHT://right
					if(isCollision(cur_x+1,cur_y)!=1){
						eraseBlock(block_cur,cur_x,cur_y);
						cur_x++;
						printBlock(block_cur,cur_x,cur_y);
					} 
					break;
				case SPACE:
					while(isCollision(cur_x,cur_y+1)!=1){
						eraseBlock(block_cur,cur_x,cur_y);
						cur_y++;
						printBlock(block_cur,cur_x,cur_y);
						score += level;//the deep jump reward
						gotoxy(27,12);
						printf("        %6d", score);
					} 
					break;
				case p:
					system("cls");
					system("pause");
					system("cls");
					printScreen();
					printBlock(block_cur,cur_x,cur_y);
					break;
				case r:
					hold_block(check_hold());
					break;
				case ESC:
					system("cls");
					system("taskkill /f /im Microsoft.Media.Player.exe ");
					exit(0);
			}
		} 
	}
} 
//levle checker 
void check_level(){
	if(cnt >= 1){
		level+=1;
		cnt = 0;
		switch(level){
            case 2:
                speed=50;
                break;
            case 3:
                speed=25;
                break;
            case 4:
                speed=10;
                break;
            case 5:
                speed=5;
                break;
            case 6:
                speed=4;
                break;
            case 7:
                speed=3;
                break;
            case 8:
                speed=2;
                break;
            case 9:
                speed=1;
                break;
            case 10:
                speed=0;
                break;
        }
		gotoxy(27, 3); printf(" LEVEL : %5d", level); 
		gotoxy(27, 4); printf(" GOAL  : %5d", 10-cnt);
		set_timer(speed);
	}
	
}
//the originn setting 
void starttetris(){
	system("cls");
	FILE *file=fopen("score.dat", "rt");
    
    if(file==0){best_score=0;} 
    else {
        fscanf(file,"%d", &best_score);
        fclose(file); 
    }
	level=1;
    score=0;
    level_goal=20;
    cnt=0;
    speed=400;
	hand=GetStdHandle(STD_OUTPUT_HANDLE);

}
// void play_sound(){
// 	PlaySound("Tetris.wav" ,NULL, SND_LOOP | SND_ASYNC);
// }
/*calling main*/
int main(){
	// mciSendString("open tetris.mp3 alias bgm",NULL,0,NULL);
	// mciSendString("play bgm repeat",NULL,0,NULL);
	// play_sound();
	system("start tetris.mp3");
	starttetris();
	srand(time(NULL));
 	setCursorVisible(0); 
	set_timer(speed);
	while(1){
		title();
		system("cls");
		printScreen();
		draw_map();
		make_new_block();
		check_level();
		key_control();
	}
	
		
		system("pause");
 	return 0;
 }
