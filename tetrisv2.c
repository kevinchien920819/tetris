#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<time.h>
#include<conio.h>
#define HEIGHT 14
#define WIDTH 18
#define block_h 4
#define block_w 4
#define block_type_num 7

void init_game(){
    
}


HANDLE hand;
int cur_x,cur_y;
int block_cur[block_h][block_w] = {0};
int block_next[block_h][block_w] = {0};
int surface[HEIGHT][WIDTH] = {0};
int score = 0;
int timer = 800;
//the block set
enum block_type{O,J,L,I,Z,S,T };

//set block O,J,L,I,Z,S,T
int BlockO[block_h][block_w] = {
        {0,0,0,0},
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0}
    };

int BlockJ[block_h][block_w] = {
        {0,0,1,0},
        {0,0,1,0},
        {0,1,1,0},
        {0,0,0,0}
    };

int BlockL[block_h][block_w] = {
        {0,1,0,0},
        {0,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    };

int BlockI[block_h][block_w] = {
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0}
    };

int BlockZ[block_h][block_w] = {
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0}
    };

int BlockS[block_h][block_w] = {
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    };

int BlockT[block_h][block_w] = {
	    {0,0,0,0},
	    {1,1,1,0},
	    {0,1,0,0},
	    {0,0,0,0}
    };

/*cursor and timer*/

//let cursor invisible
void setCursorVisable(int v){
	CONSOLE_CURSOR_INFO cursor_info = {1,v};
	SetConsoleCursorInfo(hand,&cursor_info);
}
//set curor position 
void gotoxy(int x, int y){
 	COORD pos={.X=x, .Y=y};
 	SetConsoleCursorPosition(hand, pos);
}
//timer uint
UINT_PTR timerId;
//setting timer
static void set_timmer(int t){
    KillTimer(NULL,timerId);
    timerId = SetTimer(NULL,0,t,NULL);
}
//close timer
static void close_timer(){
    KillTimer(NULL,timerId);
}
/*print out*/

//printout char at setting position 
void printxy(char c,int x,int y ){
	gotoxy(x,y);
	printf("%c",c);
}

//print out the game surface
void print_surface(){
	//print out level
    for(int i=0;i<WIDTH;i++){
		printxy('=',i+6,1);
		printxy('=',i+6,2+HEIGHT);
	}
    //print out horizon
	for(int j=-1;j<HEIGHT+1;j++){
		printxy('|',5,j+2);	
		printxy('|',6+WIDTH,j+2);
	}
    //print out the game area and wait for the block
	for(int x = 0;x < WIDTH;x++){
		for(int y =0;y < HEIGHT;y++){
            //is block print 'O' not block print ' '
			if(surface[y][x]==0){
				printxy(' ',x+6,y+2);	
			}else{
				printxy('O',x+6,y+2);
			}
		}
	}
}
//print out the random block
void print_block(int block[][block_w],int x,int y){
	//check a block if is 1 means the block is fill
    for(int w = 0 ; w < block_w;w++){
		for(int h = 0 ; h < block_h;h++){
			if(block[h][w] == 1){
				printxy('O',x + w + 6, y + h + 2);
			}
		}
	}
    
}
void print_out_point(int score){
    //set a cursor to print the point 
    gotoxy(0,20);
    printf("score :%d\n",score); 
}
//remove char when the block  leave
void erase_block(int block[][block_w],int x,int y){
	for(int w = 0 ; w < block_w;w++){
		for(int h = 0 ; h < block_h;h++){
			if(block[h][w] == 1){
				printxy(' ',x + w + 6, y + h + 2 );
			}
		}
	}
}
void erase_next_block(int block[][block_w],int x,int y){
	for(int w = 0 ; w < block_w;w++){
		for(int h = 0 ; h < block_h;h++){
			if(block[h][w] == 1){
				printxy(' ',x + w + 6, y + h + 2 );
			}
		}
	}
}
void reveal_next_block(int block[][block_w],int x,int y){
    gotoxy(30,30);
    printf("next block:\n");
	for(int w = 0 ; w < block_w;w++){
		for(int h = 0 ; h < block_h;h++){
			if(block[h][w] == 1){
				printxy('O',x + w + 6, y + h + 2 );
			}
		}
	}
}

/*the edge sensor and colision */

//check out level has block or not
int get_block_y_fill(int row){
    //set a vatiavle call isfill if is fill change the value then return 
    int is_fill = 0;
    for(int col =0 ; col < block_w;col++){
        if(block_cur[row][col] == 1)
            is_fill = 1;
    }
    return is_fill;
}
//check out horizon has block or not
int get_block_x_fill(int col){
    //set a variable call isfill if is fill change the value then return 
    int is_fill = 0;
    for(int row =0 ; row < block_w;row++){
        if(block_cur[row][col] == 1)
            is_fill = 1;
    }
    return is_fill;
}
//the white margin(edge) problem 
void get_block_left_right_margin(int *left_margin,int *right_margin){
	//pending left
    for(int col=0;col<2;col++){
		if(get_block_x_fill(col)==0){
			*left_margin=*left_margin+1;
		}
	}
    //pending right
	for(int col=2;col<4;col++){ 
		if(get_block_x_fill(col)==0){
			*right_margin=*right_margin+1;
		}
	}
}

void get_block_top_buttom_margin(int *top_margin,int *buttom_margin){
	//pending top
    for(int row=0;row<2;row++){
		if(get_block_y_fill(row)==0){
			*top_margin=*top_margin+1;
		}
	}
    //pending buttom
	for(int row=2;row<4;row++){ 
		if(get_block_y_fill(row)==0){
			*buttom_margin=*buttom_margin+1;
		}
	}
}
//check out if there has collision edge or other block 
int isCollision (int x,int y){
    int left_margin = 0,right_margin = 0,top_margin = 0,buttom_margin = 0;
    
	get_block_top_buttom_margin(&top_margin,&buttom_margin);
    get_block_left_right_margin(&left_margin,&right_margin);
    
    if(x < 0-left_margin || x > WIDTH -block_w +right_margin || y < 0-top_margin || y  > HEIGHT- block_h+buttom_margin)
		return 1;

	for(int w = 0 ; w < block_w; w++){
		for(int h = 0 ; h < block_h; h++){
			if(block_cur[h][w] == 1){
		        if(surface[y + h][x + w] == 1)
                    return 1;    
			}
		}
	}
    return 0;
}


//check if the line fill all block
int is_line_fill(int h){
   for(int w = 0; w < WIDTH;w++){
       if(surface[h][w] == 0)
           return 0;
   } 
   return 1;
} 
//if the line is all filled then clear them and get one score
void clear_line(int h){
    for(int row = h; row > 0; row--){
        for(int w = 0; w < WIDTH;w++)
            surface[row][w] = surface[row -1][w];
    } 
    for(int w = 0; w < WIDTH;w++)
        surface[0][w] = 0;

}

void check_line(){
    int total_line = 0;
    for(int h = cur_y; h < HEIGHT; h++){
        if(is_line_fill(h) == 1){
            clear_line(h);
            print_surface();
            total_line ++;
        }
            
    }
    score += total_line;
}

void land_block(){
    for(int w = 0; w < block_w; w++){
        for(int h = 0; h < block_h; h++){
            if(block_cur[h][w] == 1){
                surface[cur_y+h][cur_x+w] = 1;
            }
        }
    }
}

void copy_block(int block_s[][block_w],int block_d[][block_w]){
    int w,h;
    for(w=0;w<block_w;w++){
        for(h=0;h<block_h;h++){
            block_d[h][w] = block_s[h][w];
        }
    }
}

//do rotate 
void rotate_block(){
    int temp[block_h][block_w] = {0};
    copy_block(block_cur,temp);
    for(int w = 0; w < block_w;w++)
        for(int h = 0; h < block_h;h++)
            block_cur[h][w] = temp[w][block_h - 1 -h];	
} 

void make_new_block(){
    enum block_type type = (int)(rand() % block_type_num);
    switch(type){
        case O:
            copy_block(BlockO,block_next);
            break;
        case J:
            copy_block(BlockJ,block_next);
            break;
        case L:
            copy_block(BlockL,block_next);
            break;
        case I:
            copy_block(BlockI,block_next);
            break;
        case Z:
            copy_block(BlockZ,block_next);
            break;
        case S:
            copy_block(BlockS,block_next);
            break;
        case T:
            copy_block(BlockT,block_next);
            break;
        default:
            break;
    }
    cur_x = (WIDTH - block_w) / 2;
    cur_y = 0;  
    
}
void make_first_block(){
    enum block_type type = (int)(rand() % block_type_num);
    switch(type){
        case O:
            copy_block(BlockO,block_cur);
            break;
        case J:
            copy_block(BlockJ,block_cur);
            break;
        case L:
            copy_block(BlockL,block_cur);
            break;
        case I:
            copy_block(BlockI,block_cur);
            break;
        case Z:
            copy_block(BlockZ,block_cur);
            break;
        case S:
            copy_block(BlockS,block_cur);
            break;
        case T:
            copy_block(BlockT,block_cur);
            break;
        default:
            break;
    }
    cur_x = (WIDTH - block_w) / 2;
    cur_y = 0;  
    print_block(block_cur,cur_x,cur_y);
}

void move_block_down(){
    make_new_block();
    erase_next_block(block_next,30,30);
    reveal_next_block(block_next,30,32);
    if(isCollision(cur_x,cur_y + 1) != 1){
        erase_block(block_cur,cur_x,cur_y);
        cur_y++;
        print_block(block_cur,cur_x,cur_y);
    }
    else{
        land_block();
        check_line();
        copy_block(block_next,block_cur);
        print_block(block_cur,cur_x,cur_y);
        if(isCollision(cur_x,cur_y) == 1){
            close_timer();
        }

    }
}



void key_control(){
	int ch;
    MSG messeage;
	while(1){
        if(PeekMessage(&messeage, NULL,WM_TIMER,WM_TIMER,PM_REMOVE) != 0){
            move_block_down();
        }
		if(kbhit() != 0){
			ch = getch();
			switch (ch){
                //space to move block quickly
                case 32:
                    for(int i = 0 ; i < HEIGHT - cur_y -5;i++){
                        if(isCollision(cur_x,cur_y+1) == 0){
                            erase_block(block_cur,cur_x,cur_y);
                            cur_y++;
                            print_block(block_cur,cur_x,cur_y);
                        }
                    }
                   
 
				case 72://up 
						erase_block(block_cur,cur_x,cur_y);
						rotate_block();
                        if(isCollision(cur_x,cur_y) == 1){
                            rotate_block();
                            rotate_block();
                            rotate_block();
                        }
						print_block(block_cur,cur_x,cur_y);
					break;
				case 80://down
					if(isCollision(cur_x,cur_y+1) == 0){
						erase_block(block_cur,cur_x,cur_y);
						cur_y++;
						print_block(block_cur,cur_x,cur_y);
					}
					break;
				case 75://left
					if(isCollision(cur_x-1,cur_y) == 0){
						erase_block(block_cur,cur_x,cur_y);
						cur_x--;
						print_block(block_cur,cur_x,cur_y);
					}
					break;
				case 77://right
					if(isCollision(cur_x+1,cur_y) == 0){
						erase_block(block_cur,cur_x,cur_y);
						cur_x++;
						print_block(block_cur,cur_x,cur_y);
					}
					break;
				default:
					break;
					// printf("%d else\n",ch);
			}
		}
	}
}
// void setConsorposition()
int main(){
    init_game();
    hand = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(NULL));
	setCursorVisable(0);//set cursor visible or not
	set_timmer(timer);

	print_surface();
    make_first_block();
    print_out_point(score);
    key_control();
    

	return 0;
	
}
