#include <iostream>
#include <array>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <fstream>
#include <vector>
using namespace std;

// Map Detail
const uint32_t ROWS = 28 , COLS = 36;
const uint32_t size = ROWS * COLS;
#define cord(x,y) (x * COLS + y)

// Game Class
class pacman{
    array<int,size> map;
    char up,down,left,right;
    int n_gift,n_enemy,highscore;
    int score,level;
    vector<int*> giftpos;                                // Giftpos keep track of gifts
    struct body{
        int x,y;
    };
    string playername;
    body pac,*enemy;
    bool gameover;
    enum material{
        EMPTY,
        FOOD,
        PACMAN,
        ENEMY,
        WALL,
        GIFT
    };
    public:
    pacman(int ngift = 6,int nenemy = 5):up('W'),down('S'),left('A'),right('D'),gameover(0),score(0),n_gift(ngift),n_enemy(nenemy),level(1){
        fill(map.begin(),map.end(),FOOD);
        pac.x = 6;
        pac.y = 7;
        highscore = 0;
        enemy   = new body[n_enemy];
    }
    void level_increament(){
        system("CLS");
        n_gift +=1;
        n_enemy+=1;
        reset();
        highscore +=50;  // As reward
        score += highscore;
        level +=1;
    }

    // Clearing Screen
    void clearscreen()
    {
            HANDLE hOut;
            COORD Position;
            hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            Position.X = 0;
            Position.Y = 0;
            SetConsoleCursorPosition(hOut, Position);

    }
    bool init_required_file(){
        ifstream file("level");
        if(!file)
            return false;
        file.close();
        return true;
    }

    void wait(){
        clock_t wait;
        wait = 0.1f * CLOCKS_PER_SEC + clock();
        while(clock()<wait){}
    }
    
    // On movement Based Changes
    void move(){
        // Alternate name
        int &x = pac.x;
        int &y = pac.y;

        // if user pressed key
        if(kbhit()){
         
        // Record Keys in Uppercase
        char key = getch();
        key = toupper(key);

        // If wall found oldpos helps to put pacman back to previous position
        int oldpos_x = x;
        int oldpos_y = y;

        // Old Position Where Pacman Was is Empty Now
        map[ cord(x,y) ] = EMPTY;

        // Checking Keys
        if(key == left)
            y = (y-1+COLS)%COLS;
        else if(key == right)
            y = (y+1)%COLS;
        else if(key == up)
            x = (x-1+ROWS)%ROWS;
        else if(key == down)
            x = (x+1)%ROWS;

        // Check new position for gift or enemy or Wall
        switch( map[cord(x,y)] ){
            case ENEMY: gameover = 1;  break;
            case GIFT:  score   +=10;
                        map[cord(x,y)] = PACMAN; break;
            case FOOD:  score   +=3;
                        map[cord(x,y)] = PACMAN; break;
            case WALL:  x = oldpos_x;
                        y = oldpos_y;
                        map[cord(x,y)] = PACMAN; break;
            default:    map[cord(x,y)] = PACMAN; break;
        }

        }
    }
    void drawline(char sym){
        int i = 0;
        cout<<' ';
        while(i++<=COLS+3) cout<<sym;
        cout<<'\n';
    }
    void scores(){
        if(score > highscore)
        highscore = score;
        cout<<"\n > "<<getplayername()<<" Scores : "<<score<<"\n > High Score :"<<highscore;
    }
    int getlevel() const{
        return level;
    }
    void enemymove(){
        uint32_t i(0);
        while(i<n_enemy){

        // Random Move of enemy from up,down,left,right
        int pos = rand()%4;

        // oldpos stores prev position in case If Wall or Gift Found In Its Position
        int oldpos_x = enemy[i].x;
        int oldpos_y = enemy[i].y;

        // Coordinates change
        switch(pos){
        case 0: enemy[i].y = (enemy[i].y - 1 + COLS)%COLS;
            break;
        case 1: enemy[i].x = (enemy[i].x - 1 + ROWS)%ROWS;
            break;
        case 2: enemy[i].y = (enemy[i].y +1) % COLS;
            break;
        case 3: enemy[i].x = (enemy[i].x +1) % ROWS;
            break;
        }

        // Check New Position For Wall , Pacman , Food or gift
        switch(map[cord(enemy[i].x,enemy[i].y)]){
            case PACMAN: gameover = 1; break;
            case GIFT  : case WALL: enemy[i].x = oldpos_x;
                                    enemy[i].y = oldpos_y;
                                    map[cord(enemy[i].x,enemy[i].y)] = ENEMY; break;
            default: map[cord(oldpos_x,oldpos_y)]     = EMPTY;
                     map[cord(enemy[i].x,enemy[i].y)] = ENEMY; break;

        }
        ++i;
        }
    }

    // Setups Wall , Enemy , gift and get user info
    void getplayerinfo(){
        cout<<"\n > Enter Player Name : ";
        getline(cin,playername);
        matsetups();
    }

    // return Name of player
    string getplayername() const{
        return playername;
    }

    // Return gameover check
    bool game_over () const{
        return gameover;
    }

    void draw(){
        drawline(219);
        for(int x = 0;x<ROWS;x++){
            cout<<' '<<char(219)<<' ';
            for(int y=0;y<COLS;y++){
                switch(map[cord(x,y)]){
                case ENEMY:cout<<char(2);break;
                case GIFT:cout<<'$';break;
                case PACMAN:cout<<'*';break;
                case FOOD:cout<<'.';break;
                case WALL:cout<<char(177);break;
                default:cout<<' ';
                }
            }
            cout<<' '<<char(219)<<'\n';
        }
        drawline(219);
    }
    void info(){
        cout<<"\n 1. ( "<<char(2)<<" ) Denotes Enemy"
              "\n 2. ( $ ) Denotes Gift give 10 score and will have to eat all to Win"
              "\n 3. ( . ) Denotes Food Give 3 score"
              "\n 4.Keys ( W = up ) , ( S = Down ) , ( A = left ) , ( D = Right )\n";
    }

    // Setups Materials Such as gift , wall and enemy
    void matsetups(){
        srand(time(NULL));
        int i = 0;
        
        //  n_enemy position initialization
        while(i < n_enemy){
            enemy[i].x = rand()%ROWS;
            enemy[i].y = rand()%COLS;
            map[cord(enemy[i].x,enemy[i].y)] = ENEMY;
            ++i;
        }
        
        // n_gift Position Initialization
        i = 0;
        while(i < n_gift){
            int *ptr = &map[(rand()%size)];
            giftpos.push_back(ptr);
            *ptr = GIFT;
            ++i;
        }

        // Walls Placement COLS = 36 ROWS = 28




    }
    void mazecoord(){
        ifstream file("level");
        char ch;
        for(int i = 0;i<ROWS;i++){
            file>>ch;
            for(int j = 0;j<COLS;j++)
            {   char p;
                file>>p;
                map[cord(i,j)]  = (p-'0');
            }

            file>>ch;
        }
        file.close();
    }

    // Check If any gift let in map
    bool giftfound(){
        uint32_t i(0);
        while(i < n_gift){
            if(*giftpos.at(i)==GIFT)
                return true;
            ++i;
        }
        return false;
    }
    void write_info_to_file(){
        ofstream file("Scores");
        file<<highscore;
        file.close();
    }
    void readscore(){
        ifstream file("Scores");
        if(!file) highscore = 0;
        else
            file>>highscore;
        file.close();
    }

    // resets game in case if user lose
    void reset(){
         gameover = 0;
         mazecoord();
         pac.x = pac.y = 0;
         score = 0;
         level = 1;
         delete []enemy;
         giftpos.erase(giftpos.begin(),giftpos.end());
         enemy = new body[n_enemy];
         matsetups();
    }
    
    // Re-run Game acc. to users choice
    bool rerun() {
        if(game_over()){
            system("CLS");
            char ch;
            while(true){
                cout<<"\n\n\t GAME OVER\n\t > Press Y to continue and N to quit : ";
                cin>>ch;
                if(toupper(ch) == 'N') return 0;
                else if(toupper(ch) == 'Y'){
                    reset();
                    system("CLS");
                    break;
                    }
                else{
                    cin.clear();
                    cin.ignore(32767, '\n');
                    continue;
                }
            }
        }
        return 1;
    }
    ~pacman(){
        delete []enemy;
    }

};

int main(){

    SetConsoleTitle("PACMAN");
    // Object Can accept arguement 1.No.of gift , 2.No.Of enemy
    pacman game;

    // Setups materials and get Player info and read last highscore
    game.getplayerinfo();
    game.readscore();
    if(!game.init_required_file()){
        cout<<"Error missing file";
        return -1;
    }
    game.reset();
    system("CLS");

    // Main Routine
    while(true){
        cout<<"\n\t\t < PACMAN GAME > \n \t Level : "<<game.getlevel()<<"\n\n";
        game.draw();
        game.info();
        game.scores();
        game.move();
        game.enemymove();
        game.clearscreen();

        // If Pacman Collected all gifts he Wins and level increamented
        if(!game.giftfound()){
            system("CLS");
            cout<<"\n\t Hurray "<<game.getplayername()<<" Wins :) \n\n  > Press Key To Start Next Level ";
            cin.get();
            game.level_increament();
            continue;
        }

        // If user wants to continue
        if(!game.rerun()) break;
    }

    // Write score to file
    game.write_info_to_file();
    game.clearscreen();

    return 0;
}
