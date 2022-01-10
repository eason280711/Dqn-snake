#include<bits/stdc++.h>
#include<windows.h>
#include <conio.h>
#include <fstream>
#include <process.h>
#define rate 0
using namespace std;
string s = "                ";
vector<vector<int>> vec = {{9,7},{4,6},{7,10},{8,7},{9,12},{9,5},{6,12},{12,9},{5,9},{3,5},{8,4},{4,12},{5,10},{5,1},{8,9},{2,7},{11,9},{10,9},{10,7},{2,5},{7,7},{3,10},{4,6},{8,6},{6,8},{10,6},{11,8},{11,5},{10,5},{10,4}};
class game
{
private:
    bool mode;
    struct cell
    {
        pair<int,int> loc;
        string type = "cell";
        int score =0,hunger = 100,color = 240;
    };
    struct object
    {
        pair<int,int> loc;
        string type;
        object(int a,int b,string c){
            loc.first = a,loc.second = b,type = c;
        }
    };
    struct node{
        int x,y;
        float reward;
        node(int a,int b,float c){
            x = a;
            y = b;
            reward = c;
        }
    };
    cell c;
    int len;
    int limit,EAT,foodc,last_step;
    char b = 254;
    vector<vector<int>> board;
    deque<pair<int,int>> trail;
    vector<vector<float>> rate_;
    pair<int,int> foodloc;
    HANDLE han;
    CONSOLE_CURSOR_INFO info;
public:
    void SetColor(int color = 7){SetConsoleTextAttribute(han,color);}
    game(int l,int w){
        board = vector<vector<int>>(l,vector<int>(w,7));
        c.loc.first = l/2;
        c.loc.second = w/2;
        for(int i = 0;i < board.size();i++)
        {
            board[i][board[0].size()-1] = 8;
            board[board[0].size()-1][i] = 8;
            board[i][0] = 8;
            board[0][i] = 8;
        }

        mode = false;
        EAT = 0;
        foodc = 0;
        len = 2;
        last_step = 0;
        limit = 9999;
        han = GetStdHandle(STD_OUTPUT_HANDLE);
        info.dwSize = 100;
        info.bVisible = FALSE;
        SetConsoleCursorInfo(han, &info);
        SetConsoleOutputCP(437);
        rate_ = vector<vector<float>>(l,vector<float>(w,0.0));
    };
    void BFS(vector<vector<float>> &rate_,int x,int y,float reward)
    {
        queue<node> BFS;
        for(int i = 0;i < rate_.size();i++)
        {
            for(int j = 0;j < rate_[0].size();j++)
            {
                rate_[i][j] = 0.0;
            }
        }
        BFS.emplace(node(x,y,reward));
        while(!BFS.empty())
        {
            if(BFS.front().reward*0.95 > 0.08)
            {
                if(!(BFS.front().x+1 < 1 || BFS.front().x+1 >= 17 || BFS.front().y < 1 || BFS.front().y >= 17))
                    if(rate_[BFS.front().x+1][BFS.front().y] == 0.0)
                        BFS.emplace(node(BFS.front().x+1,BFS.front().y,BFS.front().reward*0.95));

                if(!(BFS.front().x-1 < 1 || BFS.front().x-1 >= 17 || BFS.front().y < 1 || BFS.front().y >= 17))
                    if(rate_[BFS.front().x-1][BFS.front().y] == 0.0)
                        BFS.emplace(node(BFS.front().x-1,BFS.front().y,BFS.front().reward*0.95));

                if(!(BFS.front().x < 1 || BFS.front().x >= 17 || BFS.front().y+1 < 1 || BFS.front().y+1 >= 17))
                    if(rate_[BFS.front().x][BFS.front().y+1] == 0.0)
                        BFS.emplace(node(BFS.front().x,BFS.front().y+1,BFS.front().reward*0.95));

                if(!(BFS.front().x < 1 || BFS.front().x >= 17 || BFS.front().y-1 < 1 || BFS.front().y-1 >= 17))
                    if(rate_[BFS.front().x][BFS.front().y-1] == 0.0)
                        BFS.emplace(node(BFS.front().x,BFS.front().y-1,BFS.front().reward*0.95));
            }
            rate_[BFS.front().x][BFS.front().y] = BFS.front().reward;
            BFS.pop();
        }
        return;
    }
    int coutc(int a = 7)
    {
        SetColor(a);
        cout << b;
        SetColor();
    }
    void printB()
    {
        //for(int i = 0;i <= 15;i++) cout << endl;
        for(int i = 0;i < board.size();i++)
        {
            //cout << s;
            for(int j = 0;j < board[0].size();j++)
            {
                if(i != c.loc.first || j != c.loc.second) coutc(board[i][j]);
                else coutc(c.color);
            }
            cout << endl;
        }
    }
    bool check(int x,int y)
    {
        if(x == c.loc.first && y == c.loc.second) return false;
        for(int i = -2;i <= 2;i++)
        {
            for(int j = -2;j <= 2;j++)
            {
                if(x+i < 0 || x+i >= board.size() || y+j < 0 || y+j >= board[0].size()) continue;
                if(x+i == c.loc.first && y+j == c.loc.second) return false;
                if(board[x+i][y+j] == 12 || board[x+i][y+j] != 7) return false;
            }

        }
        return true;
    }
    void randomfood(int a = 1)
    {
        int x,y;
        srand(time(NULL));
        while(a--)
        {
            do
            {
                x = rand() % board.size();
                y = rand() % board[0].size();
            }while(!check(x,y));
            board[x][y] = 252;
            foodloc.first = x;
            foodloc.second = y;
        }
        BFS(rate_,foodloc.first,foodloc.second,0.2);
        return;
    }
    void food()
    {
        board[vec[foodc][0]][vec[foodc][1]] = 252;
        foodloc.first = vec[foodc][0];
        foodloc.second = vec[foodc][1];
        foodc++;
        BFS(rate_,foodloc.first,foodloc.second,0.2);
        if(foodc >= 30) foodc = 0;
    }
    void eat(int x,int y)
    {
        int count_ = 32;
        SetConsoleCursorPosition(han,{4,board.size()+6});
        cout << "Eating: ";
        for(int i = 0;i < 10;i++) coutc(12);
        while(1)
        {
            //Sleep(50*rate);
            SetConsoleCursorPosition(han,{count_,board.size()+6});
            if(count_ >= 12)
            {
                cout << "  ";
                count_ -= 2;
            }
            else break;
        }
        SetConsoleCursorPosition(han,{4,board.size()+6});
        cout << "                                   ";
        c.hunger += 15;
        board[x][y] = 7;
        SetConsoleCursorPosition(han,{y*2,x});
        coutc(board[x][y]);
        if(limit)
        {
            food();
            SetConsoleCursorPosition(han,{0,0});
            printB();
            limit -= 1;
        }
        len += 1;
        return;
    }
    deque<object> dectect()
    {
        deque<object> ret;
        for(int i = 0;i < board.size();i++)
        {
            for(int j = 0;j < board[0].size();j++)
            {
                if(board[i][j] == 8)
                    ret.push_back(object(i,j,"wall"));
                if(board[i][j] == 252)
                    ret.push_front(object(i,j,"food"));
            }
        }
        return ret;
    }
    void step_gene(string d,string dir,int eat_t)
    {
        SetConsoleCursorPosition(han,{4,board.size()+8});
        int gate = 0;
        string content;
        string step = "step_",txt = ".txt";
        vector<vector<int>> dec = board;
        dec[c.loc.first][c.loc.second] = 28;
        char buffer[20];
        int nums;
        //content +=  ste+"\n";
        /*
        content +=  to_string(c.loc.first) +","+to_string(c.loc.second)+" \n";
        content +=  to_string(c.hunger)+" \n";
        while(!dec.empty())
        {
            if(dec.front().type == "wall" && !gate){
                content += "\n";
                gate = 1;
                break;
            }
            content += to_string(dec.front().loc.first) +","+to_string(dec.front().loc.second)+" ";
            dec.pop_front();
        }
        */
        //content +=  "\n"+to_string(c.score)+" \n";
        content +=  to_string(board.size())+","+to_string(board[0].size())+" \n";
        for(int i = 0;i < dec.size();i++)
        {
            for(int j = 0;j < dec[0].size();j++)
            {
                int a = dec[i][j];
                if(a == 8) a = 120;
                if(a == 252) a = 200;
                if(a == 28) a = 50;
                if(j != dec.size() -1) content +=  to_string(a) +",";
                else content +=  to_string(a);
            }
            content +=  "\n";
        }
        content += d +" \n";
        content += dir;
        content += " \n"+to_string(len+1);
        content += " \n"+to_string(eat_t);
        //content +=  to_string(c.hunger) +" \n";
        fstream newFile("nums.txt");
        newFile.getline(buffer,20);
        string s(buffer);
        nums = atoi(s.c_str());
        newFile.close();

        ofstream nFile;
        string name = step + to_string(nums) + txt;
        nFile.open(name.c_str());
        nFile << content.c_str();
        nFile.close();
        string move_ = "move "+ name +" step_gene > nul";
        system(move_.c_str());

        ofstream reFile;
        reFile.open("nums.txt",ios::trunc);
        s = to_string(nums+1);
        reFile << s.c_str();
        reFile.close();

        ofstream seFile;
        seFile.open("step.txt",ios::trunc);
        s = to_string(0);
        seFile << s.c_str();
        seFile.close();

        return;
    }
    int select_by_txt()
    {
        char buffer[100] = {0};
        FILE* newFile;
        newFile = fopen("step.txt", "r");
        //while(newFile.is_open()){Sleep(10);}
        while(!feof(newFile))
            fread(buffer, sizeof(buffer), 1, newFile);
        fclose(newFile);
        string s(buffer);
        if(s == "" || s == "0")
        {
            Sleep(1);
            return select_by_txt();
        }
        Sleep(10);
        return atoi(s.c_str());
    }
    int distance_(int a,int b,int c,int d)
    {
        return ((a-c)*(a-c)) + ((b-d)*(b-d));
    }
    void execute()
    {
        COORD loc = {c.loc.second*2,c.loc.first};
        SetConsoleCursorPosition(han,loc);
        step_gene("0","0,0,0,0",0);
        float dis = rate_[c.loc.first][c.loc.second];
        BFS(rate_,foodloc.first,foodloc.second,0.2);
        int eat_t = 0;
        while(1)
        {
            COORD loc = {c.loc.second*2,c.loc.first};
            SetConsoleCursorPosition(han,loc);
            int ch = 0,con;
            int tar = 0,time_ = 3;
            if(mode)
            {
                while(time_ --)
                {
                    if(_kbhit())
                    {
                        ch = getch();
                        tar = 1;
                    }
                    else if(!tar)
                    {
                        ch = last_step;
                    }
                    Sleep(10);
                }
                if(!ch) ch = getch();
                
                //ch = getch();
            }
            else{
                /*cin >> con;
                if(con != 1)
                    system("taskkill /f /im Untitled1.exe");*/
                ch = select_by_txt();
                Sleep(10);
            }
            //if(abs(ch - last_step) == 8 ||abs(ch - last_step) == 2 ) ch = last_step;
            if(ch == 224) continue;
            if(ch == 43) break;
            //if(c.hunger < 0) ch = 27;
            if (ch == 27){break;} //esc
            if(last_step == 72 && ch == 80) ch = last_step;
            if(last_step == 80 && ch == 72) ch = last_step;
            if(last_step == 75 && ch == 77) ch = last_step;
            if(last_step == 77 && ch == 75) ch = last_step;
            if(ch == 72){
                if(c.loc.first-1 < 0 || board[c.loc.first-1][c.loc.second] == 240 || board[c.loc.first-1][c.loc.second] == 8 || eat_t > 80)
                {
                    //c.hunger -= 4;
                    step_gene("-10","1,0,0,0",eat_t);
                    SetConsoleCursorPosition(han,{4,board.size()+2});
                    cout << "score: "<< setw(4)  << c.score << endl;
                    SetConsoleCursorPosition(han,{4,board.size()+4});
                    cout << "hunger: " << setw(4) << c.hunger << endl;
                    break;
                }
                //coutc(board[c.loc.first][c.loc.second]);
                board[c.loc.first][c.loc.second] = 240;
                trail.push_front({c.loc.first,c.loc.second});
                if(board[c.loc.first-1][c.loc.second] == 8) c.loc.first += board.size()-3;
                else c.loc.first -= 1;
                last_step = 72;
                //eat_t++;
                //c.hunger -= 2;
            }            //�W
            if(ch == 80){
                if(c.loc.first+1 >= board.size() || board[c.loc.first+1][c.loc.second] == 240 || board[c.loc.first+1][c.loc.second] == 8 || eat_t > 80)
                {
                    //c.hunger -= 4;
                    step_gene("-10","0,1,0,0",eat_t);
                    SetConsoleCursorPosition(han,{4,board.size()+2});
                    cout << "score: "<< setw(4)  << c.score << endl;
                    SetConsoleCursorPosition(han,{4,board.size()+4});
                    cout << "hunger: " << setw(4) << c.hunger << endl;
                    break;
                }
                //coutc(board[c.loc.first][c.loc.second]);
                board[c.loc.first][c.loc.second] = 240;
                trail.push_front({c.loc.first,c.loc.second});
                if(board[c.loc.first+1][c.loc.second] == 8) c.loc.first -= board.size()-3;
                else c.loc.first += 1;
                last_step = 80;
                //eat_t++;
                //c.hunger -= 2;
            }            //�U
            if(ch == 75){
                if(c.loc.second-1 < 0 || board[c.loc.first][c.loc.second-1] == 240 || board[c.loc.first][c.loc.second-1] == 8 || eat_t > 80)
                {
                    //c.hunger -= 4;
                    step_gene("-10","0,0,1,0",eat_t);
                    SetConsoleCursorPosition(han,{4,board.size()+2});
                    cout << "score: "<< setw(4)  << c.score << endl;
                    SetConsoleCursorPosition(han,{4,board.size()+4});
                    cout << "hunger: " << setw(4) << c.hunger << endl;
                    break;
                }
                //coutc(board[c.loc.first][c.loc.second]);
                board[c.loc.first][c.loc.second] = 240;
                trail.push_front({c.loc.first,c.loc.second});
                if(board[c.loc.first][c.loc.second-1] == 8) c.loc.second += board.size()-3;
                else c.loc.second -= 1;
                last_step = 75;
                //eat_t++;
                //c.hunger -= 2;
            }            //��
            if(ch == 77){
                if(c.loc.second+1 >= board[0].size() || board[c.loc.first][c.loc.second+1] == 240 || board[c.loc.first][c.loc.second+1] == 8 || eat_t > 80)
                {
                    //c.hunger -= 4;
                    step_gene("-10","0,0,0,1",eat_t);
                    SetConsoleCursorPosition(han,{4,board.size()+2});
                    cout << "score: "<< setw(4)  << c.score << endl;
                    SetConsoleCursorPosition(han,{4,board.size()+4});
                    cout << "hunger: " << setw(4) << c.hunger << endl;
                    break;
                }
                //coutc(board[c.loc.first][c.loc.second]);
                board[c.loc.first][c.loc.second] = 240;
                trail.push_front({c.loc.first,c.loc.second});
                if(board[c.loc.first][c.loc.second+1] == 8) c.loc.second -= board.size()-3;
                else c.loc.second += 1;
                last_step = 77;
                //eat_t++;
                //c.hunger -= 2;
            }            //�k
            if(trail.size() > len)
            {
                loc = {trail.back().second*2,trail.back().first};
                SetConsoleCursorPosition(han,loc);
                board[trail.back().first][trail.back().second] = 7;
                coutc(board[trail.back().first][trail.back().second]);
                trail.pop_back();
            }
            if(board[c.loc.first][c.loc.second] == 240) break;
            loc = {c.loc.second*2,c.loc.first};
            SetConsoleCursorPosition(han,loc);
            coutc(c.color);
            Sleep(10*rate);
            float a = 0;
            float temp = rate_[c.loc.first][c.loc.second];
            if(temp < dis) a = -0.8 * dis;
            else a = temp;
            dis = temp;
            if(board[c.loc.first][c.loc.second] == 252)
            {
                eat(c.loc.first,c.loc.second);
                a = 1;
                dis = 0.0;
                eat_t = 0;
            }
            if(eat_t > 40 && eat_t % 3 == 0) a = -0.02;
            if(last_step == 72) step_gene(to_string(a),"1,0,0,0",eat_t);
            if(last_step == 80) step_gene(to_string(a),"0,1,0,0",eat_t);
            if(last_step == 75) step_gene(to_string(a),"0,0,1,0",eat_t);
            if(last_step == 77) step_gene(to_string(a),"0,0,0,1",eat_t);
            SetConsoleCursorPosition(han,{4,board.size()+2});
            cout << "score: "<< setw(4)  << c.score << endl;
            SetConsoleCursorPosition(han,{4,board.size()+4});
            cout << "hunger: " << setw(4) << c.hunger << endl;

        }
        SetConsoleCursorPosition(han,{4,board.size()+6});
        cout << "GAME OVER" << endl;
        return;
    }
};

int main()
{
    while(1)
    {
        system("cls");
        game test(18,18);
        test.food();
        test.printB();
        test.execute();
        Sleep(15);
    }

    return 0;
}

//os.system("cd C:/Users/user/Desktop/DQN/env && Untitled1.exe")

//