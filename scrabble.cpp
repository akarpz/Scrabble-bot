#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <ctime>
#include "stdlib.h"
#include <ctype.h>
#include <string.h>
#include <algorithm>
using namespace std;

class Move{
    public:
        int x;
        int y;
        int length;
        vector<string> words;
        bool dir;
        char letter;
        int anchor;
        
        Move(){
            x = -1;
            y = -1;
            length = -1;
            dir = -1;
            letter = 'a';
            anchor = -1;
        }
        
};

struct Trie {
    map<char, Trie*> children;
    bool isEOW;
    
    
    bool hasChild(char key){
        map<char, Trie*>::iterator result = children.find(key);
        return result != children.end();      
    }

    bool hasWord(char* input, int length){
        if (length == 0){
            return isEOW;
        }
        
        if (!hasChild(input[0])){
            return false;
        }
        
        return (children[input[0]])->hasWord(input+1, length-1);
    }
    
    void printAll(const string& prefix){
        if (isEOW){
            cout << prefix << endl;
        }
        
        for(map<char, Trie*>::iterator it = children.begin(); it != children.end(); it++){
            it->second->printAll(prefix + it->first);
        }
    }
    
    void getAllWords(const string& prefix, vector<string>& results){
        if (isEOW){
            results.push_back(prefix);
        }
        
        for(map<char, Trie*>::iterator it = children.begin(); it != children.end(); it++){
            it->second->getAllWords(prefix + it->first, results);
        }
    }
    
    void getRackWords(const string& prefix, map<char, int>& rack, vector<string>& results){
        if (isEOW){
            results.push_back(prefix);
        }
        
        for(map<char, Trie*>::iterator it = children.begin(); it != children.end(); it++){
            if (rack.find(it->first) != rack.end() && rack[it->first] > 0){
                    rack[it->first] -=1;
                    it->second->getRackWords(prefix + it->first, rack, results);
                    rack[it->first] +=1;
            }
        }
    }
    
    void insert(char* input, int length){
        if (length == 0){
            isEOW = true;
            return;
        }
        
        if (!hasChild(input[0])){
            Trie* newTrie = new Trie();
            children[input[0]] = newTrie;
        }

        (children[input[0]])->insert(input+1, length-1);
    }
    
    Trie(){
        isEOW = false;
    }
};

//Begin Placement Possibilities!

Trie readWords(){
    Trie trie;
    string line;
    ifstream myfile ("dictionary.txt");
      
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            char* word;
            word = new char[line.size()];
            for(int i=0 ; i<line.size() ; i++){
                word[i] = line[i];
            }
            trie.insert(word,line.size());
        }

        myfile.close();
    }
    return trie;
}



vector<vector<char> > playBoard;
void readTestBoard(string boardName){
    ifstream myfile (boardName.c_str());
    string line;
    int counter = 0;
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            if(counter!=0){
                vector<char> temp;
                for(int i = 0 ; i < 15 ; i++){
                    temp.push_back(line[i]);
                }
                playBoard.push_back(temp);
            }
            counter++;
        }
        myfile.close();
    }
}

map<char,int> rack;
void readRack(string boardName){
    ifstream myfile (boardName.c_str());
    string line;
    int counter = 0;
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            if(counter==0){
                for(int i=0 ; i<7 ; i++){
                    char letter = tolower(line[i]);
                    if(rack[letter] > 0){
                        int newVal = rack[letter]+1;
                        rack[letter] = newVal;
                    }
                    else{
                        rack[letter] = 1;
                    }
                }
            }
            counter++;
        }
        myfile.close();
    }
}



vector<vector<char> > readBoard(string boardName){
    ifstream myfile (boardName.c_str());
    string line;
    int counter = 0;
    vector<vector<char> > board;
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            if(counter!=0){
                vector<char> temp;
                for(int i = 0 ; i < 15 ; i++){
                    temp.push_back(line[i]);
                }
                board.push_back(temp);
            }
            counter++;
        }
        myfile.close();
    }
    
    return board;
}

int findLetter(char letter){
    
    char ref[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    
    for(int i = 0 ; i<26 ; i++){
        if(letter == ref[i]){ 
            return i;
        }
    }
}
bool isLetter(char letter){
    if(!(letter == '9' || letter == '4' || letter == '3' || letter == '2' || letter == '-')){
        return true;
    }
    return false;
}

int determineLength(vector<vector<char> > board,int x,int y,int dir){
    int count = 1;
    bool flag = true;

    if(dir==0){//vertical
        if(y==board.size()-1){
            flag = false;
            count--;
        }

        while(flag && !isLetter(board[y+count][x])){
            count++;
            if(count == 8 || ((count+y)==board.size()-1)){
                flag = false;
            }
        }
        if(count > 0 && isLetter(board[y+count][x])){
            count--;
        }
        if(count > 0 && (count+y)==board.size()-1){
            count++;
        }
        return count;
    }
    else if(dir == 1){//horizontal
        if(x==board[y].size()-1){
            flag = false;
            count--;
        }
        while(flag && !isLetter(board[y][x+count])){
            count++;
            if(count == 8 || count+x==board[y].size()-1){
                flag = false;
            }
        }
        if(count > 0 && isLetter(board[y][x+count])){
            count--;
        }
        if(count > 0 && count+x==board[y].size()-1){
            count++;
        }
        return count;
    }
    return count;
}

int determineDir(vector<vector<char> > board, int x, int y){
    int dir = -1;
    if(x == board[y].size()-1){
        if(!isLetter(board[y][x-1])){ //the horizontal is available
            dir = 1;
        }
    }
    else if(x == 0){
        if(!isLetter(board[y][x+1])){ //the horizontal is available
            dir = 1;
        }
    }
    else{
        if(!isLetter(board[y][x+1]) && !isLetter(board[y][x-1])){ //the horizontal is available
            dir = 1;
        }
    }
    
    if(y == board.size()-1){
        if(!isLetter(board[y-1][x])){ //the vertical is available
            dir = 0;
        }
    }
    else if(y == 0){
        if(!isLetter(board[y+1][x])){ //the vertical is available
            dir = 0;
        }
    }
    else{
        if(!isLetter(board[y+1][x]) && !isLetter(board[y-1][x])){ //the vertical is available
            dir = 0;
        }
    }
    return dir;
}
/*
vector<vector<int> > emptyGen(){
    
}
*/
vector<vector<int> > possibleMoves(int x, int y){
    //Deleted readBoard()
    vector<vector<char> >board = playBoard;
    int letter = findLetter(board[y][x]);
    int anchor = 1;
    int len;
    int dir;
    vector<int> temp;
    vector<vector<int> > ans;
    
    //Determine direction and length
    //dir = 0;
    dir = determineDir(board,x,y);
    //len = 2;
    len = determineLength(board,x,y,dir);
    
    if(dir!=-1 && len>1){    
        temp.push_back(x);
        temp.push_back(y);
        temp.push_back(letter);
        temp.push_back(anchor);
        temp.push_back(len);
        temp.push_back(dir);
        ans.push_back(temp);
        temp.clear();
    }
    
    int counter = 1;
    bool flag = true;
    if(dir == 0){//down
        if(y == 0){
            flag = false;
        }
        while(flag && !isLetter(board[y-counter][x])){
            if(y-counter-1 >= 0 && !isLetter(board[y-counter-1][x])){
                temp.push_back(x);
                temp.push_back(y-counter);
                temp.push_back(letter);
                temp.push_back(anchor+counter);
                if(len == 8){
                    temp.push_back(len);
                }
                else{
                    len++;
                    temp.push_back(len);
                }
                temp.push_back(dir);
                ans.push_back(temp);
                temp.clear();
            }
            
            counter++;
            if(counter==8 || (y - counter < 0)){
                flag = false;
            }
        }
    }
    else if(dir==1){
        if(x == 0){
            flag = false;
        }
        while(flag && !isLetter(board[y][x-counter])){
            if(x-counter-1 >= 0 && !isLetter(board[y][x-counter-1])){
                temp.push_back(x-counter);
                temp.push_back(y);
                temp.push_back(letter);
                temp.push_back(anchor+counter);
                if(len == 8){
                    temp.push_back(len);
                }
                else{
                    len++;
                    temp.push_back(len);
                }
                temp.push_back(dir);
                ans.push_back(temp);
                temp.clear();
            }
            
            counter++;
            if(counter==8 || (x - counter < 0)){
                flag = false;
            }
        }
    }
    return ans;
    
}
vector<vector<int> > findSpots(){
    
    /*
    returns a vector of ints in the format of
     {x,y,number corresponding to the letter (0=a, 25=z) 
          , space in the word the anchor is (if this is 2 and the letter is a, a possible word is cat)
          , max length of the word
          , down/right (down = 0 or right = 1)}
          
        BOARD FORMAT
        
            RIFOXOL
            9--2---9---2-NO
            -4---3---3JOLE-
            --4---2-2---I--
            2--4---2--MUD-2
            ----4----MA----
            -3---3-GROWTHS-
            --2-G-DIED--2U-
            9--CAVES---2-QI
            --2-ZA2-2---T-N
            -3--EN---3-BULK
            -T--R-----TAR--
            2HUES--2--EYNE2
            WAP---2-2---I--
            IT---3---3--P4-
            G--2---9---2SIB
            
    */
    vector<vector<int> > spots;
    vector<vector<char> > board;
    board = playBoard;
    for(int y = 0 ; y<15 ; y++){
        for(int x = 0 ; x<15 ; x++){
            vector<vector<int> > temp;
            if(isLetter(board[y][x])){
                temp = possibleMoves(x,y);
                for(int i=0 ; i<temp.size() ; i++){
                    spots.push_back(temp[i]);
                }
            }
        }
    }
    
    return spots;
    
}
//End Placement Possibilities

//Filtering Words Begin!
char makeLetter(int letter){
    char ref[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    return ref[letter];
}



vector<Move> possibleWords (vector<vector<int> > scrabbleLine , Trie dict){
   vector<Move> allWords;
   
   for (int i = 0; i < scrabbleLine.size() ; i++){
        Move move;
        vector<int> temp = scrabbleLine[i];
        
        move.x = temp.at(0);
        move.y = temp.at(1);
        move.letter = tolower(makeLetter(temp.at(2))); //I will write this in a seccond. It will make a char from the int given
        move.anchor = temp.at(3);
        move.length = temp.at(4);
        move.dir = temp.at(5);
        
        map<char,int> tempRack = rack;
        if(tempRack[move.letter] > 0){
            int newVal = tempRack[move.letter]+1;
            tempRack[move.letter] = newVal;
        }
        else{
            tempRack[move.letter] = 1;
        }

        vector<string> possibleWords;
        dict.getRackWords("",tempRack,possibleWords);
        
        int j = 0;
        
        while(j<possibleWords.size()){
            string word = possibleWords.at(j);
    
            if(word.size() < move.anchor){
                possibleWords.erase(possibleWords.begin()+j);
                j--;
            }
            else if(word.size()>move.length){
                possibleWords.erase(possibleWords.begin()+j);
                j--;
            }
            else{
                if(word.at(move.anchor-1) != move.letter){
                possibleWords.erase(possibleWords.begin()+j);
                j--;
                }
            }
            j++;
        }
        //cout << possibleWords.size() << endl;
        move.words = possibleWords;
        allWords.push_back(move);
   }
   return allWords;
}
//Filtering Words Done!

/*SCORING
*/
class Play{
    public:
        int zon;
        int vert;
        string word;
        bool dir;
        int score;
    
    Play(){
            zon = -1;
            vert = -1;
            dir = false;
            score = 0;
        }
};

struct CompareScore {
	bool operator ()(Play const & play1, Play const & play2){
		return play1.score < play2.score; 
	}
};

priority_queue<Play, vector<Play>, CompareScore> scorePQ;


//Map containing tile face and score
map<char, int> tileScores;
//Must to run to populate the map. Only needs to run once per mass score checking (once per turn)
void setScores(){
    tileScores['a'] = 1; tileScores['b'] = 3; tileScores['c'] = 3;
    tileScores['d'] = 2; tileScores['e'] = 1; tileScores['f'] = 4;
    tileScores['g'] = 2; tileScores['h'] = 4; tileScores['i'] = 1;
    tileScores['j'] = 8; tileScores['k'] = 5; tileScores['l'] = 1;
    tileScores['m'] = 3; tileScores['n'] = 1; tileScores['o'] = 1;
    tileScores['p'] = 3; tileScores['q'] = 10; tileScores['r'] = 1;
    tileScores['s'] = 1; tileScores['t'] = 1; tileScores['u'] = 1;
    tileScores['v'] = 4; tileScores['w'] = 4; tileScores['x'] = 8;
    tileScores['y'] = 4; tileScores['z'] = 10;
}
//2D vector that reads in a starting board to be able to reference the multiplier at any position    
vector<vector<char> > blankBoard;
//Needed to populate the vector from a text file
void readMods(){
    ifstream myfile ("BlankBoard.txt");
    string line;
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
                vector<char> temp;
                for(int i = 0 ; i < 15 ; i++){
                    temp.push_back(line[i]);
                }
                blankBoard.push_back(temp);
        }
        myfile.close();
    }
}


bool isNormal(int x, int y){
    return (blankBoard[y][x] == '-');

}
//Checks if letter is on a Triple Word Score
bool isTWS(int x, int y){
    return (blankBoard[y][x] == '9');

}
//Checks if letter is on a Double Word Score 
bool isDWS(int x, int y){
    return (blankBoard[y][x] == '4');

}
//Checks if letter is on a Triple Letter
bool isTL(int x, int y){
    return(blankBoard[y][x] == '3');
}
//Checks if letter is on a Double Letter
bool isDL(int x, int y){
    return(blankBoard[y][x] == '2');
}

bool inPlay(int x, int y){
    return (playBoard[y][x] > 64 && playBoard[y][x] < 91);
}

bool isBingo(int x, int y, string word){
    //Not true for all cases, but with our assumptions, these cover all Bingo cases
    return((word.size() == 8) || (!inPlay(7,7) && word.size() == 7));
}

char getLetter(int x, int y){
    return tolower(playBoard[y][x]);
}

int scoreBranch(int x, int y, bool dir, char letter){
    int X = x;
    int Y = y;
    int sum = 0;
    bool dws = false;
    bool tws = false;
    
    if(isDWS(x,y)){
        dws = true;
    }
    
    if(isTWS(x,y)){
        tws = true;
    }
    
    //Word goes horizontal, change y coord.
    if(dir){
        if( ((y - 1) > -1) && ((y + 1) < 15) && (inPlay(x,y - 1) + inPlay(x,y + 1))){
            
            if(isNormal(x,y) || isDWS(x,y) || isTWS(x,y)) {
                sum += tileScores[letter];
            }
            else if(isDL(x,y)){
                sum += tileScores[letter]*2;
            }
            else if(isTL(x,y)){
                sum += tileScores[letter]*3;
            }
        }
        while(((Y - 1) > -1) && inPlay(x,Y - 1)){
            Y = Y - 1;
            sum += tileScores[getLetter(x,Y)];
            
        }
        Y = y;
        while(((Y + 1) < 15) && inPlay(x,Y + 1)){
            Y = Y + 1;
            sum += tileScores[getLetter(x,Y)];
        }
        if(dws){
            sum = sum*2;
        }
        else if(tws){
            sum = sum*3;
        }
    }
    //else word goes vertical, change x coord.
    else{
        if( ( (x + 1) < 15 ) && ((x -1) > 0) && (inPlay(x - 1,y) || inPlay(x + 1,y))){
            if(isNormal(x,y)) {
                sum += tileScores[letter];
            }
            else if(isDL(x,y)){
                sum += tileScores[letter]*2;
            }
            else if(isTL(x,y)){
                sum += tileScores[letter]*3;
            }
        }
        while(((X - 1) > -1) && inPlay(X - 1,y)){
            X--;
            sum += tileScores[getLetter(X,Y)];
            
        }
        X = x;
        while(((X + 1) < 15) && inPlay(X + 1,y)){
            X++;
            sum += tileScores[getLetter(X,Y)];
            
        }
        if(dws){
            sum = sum*2;
        }
        else if(tws){
            sum = sum*3;
        }
    }
    return sum;
}

//Used to score a single word applying multipliers to each letter. ie: Can only be used on the main word being played, will not work for branches
int scoreWord(int x, int y, int dir, string word){
    int sum = 0;
    int outerSum =0;
    //Can be booleans because 2 DWS and TWS cannot be reached with a 7 letter word. 
    bool dws = false;
    bool tws = false;
    
    for(int i = 0; i < word.size(); i++){
        //dir = 1 shows that the word will be played horizontally
        if(dir){
            if(inPlay(x,y)){
                sum += tileScores[word[i]];
            }
            //Checks in order of probability to save time. 
            //i.e, it is most likely to be a normal tile so checking that first rather than having an else after all the other checks will save time.
            else{
                outerSum += scoreBranch(x,y,dir,word[i]);
                if(isNormal(x,y)){
                    sum += tileScores[word[i]];
                }
                else if(isDL(x,y)){
                    sum += tileScores[word[i]]*2;
                }
                else if(isDWS(x,y)){
                    dws = true;
                    sum += tileScores[word[i]];
                }
                else if(isTL(x,y)){
                    sum += tileScores[word[i]]*3;
                }
                else if(isTWS(x,y)){
                    tws = true;
                    sum += tileScores[word[i]];
                }
            }
            x++;
        }
        else{
            if(inPlay(x,y)){
                sum += tileScores[word[i]];
            }
            else{
                outerSum += scoreBranch(x,y,dir,word[i]);
                
                if(isNormal(x,y)){
                    sum += tileScores[word[i]];
                }
                else if(isDL(x,y)){
                    sum += tileScores[word[i]]*2;
                }
                else if(isDWS(x,y)){
                    dws = true;
                    sum += tileScores[word[i]];
                }
                else if(isTL(x,y)){
                    sum += tileScores[word[i]]*3;
                }
                else if(isTWS(x,y)){
                    tws = true;
                    sum += tileScores[word[i]];
                }
            }
            y++;
        }
    }
    //DWS is more likely so it should be check first. TWS and DWS cannot both be reached with a 7 letter tile play so if DWS is true, TWS will not be
    if(dws){
        sum = sum*2;
        
    }
    else if(tws){
        sum = sum*3;
    }
    sum = sum + outerSum;
    if(isBingo(x, y, word)){
        return (sum + 50);
    }
    return sum;
}

void scoreAll(vector<Move> moves){
    for(int i = 0; i < moves.size(); i++)
        for(int j = 0; j < moves[i].words.size(); j++){
            Play play;
            play.zon = moves[i].x;
            play.vert = moves[i].y;
            play.word = moves[i].words[j];
            play.dir = moves[i].dir;
            play.score = scoreWord(moves[i].x,moves[i].y,moves[i].dir,moves[i].words[j]);
            scorePQ.push(play);
        }   
    }
    
void printBoard(){
    for(int i = 0; i < 15; i++){
        for(int j = 0; j < 15; j++){
            cout << getLetter(j,i) << " ";
        }
        cout << endl;
    }
}

void printTopBoard(int x, int y, int dir, string word){
    char lets[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O'};
    for(int i = 0; i < 15; i++){
        if(i == 0){
            cout << "     ";
            for(int j = 0; j < 15; j++){
                cout << lets[j]<< " ";
            }
            cout << endl<<endl;
        }
        cout << lets[i] << "    ";
        for(int j = 0; j < 15; j++){
            
            if(dir){
                if((i == y) && (j >= x) && (j < (x+word.size()))){
                    cout << (char) toupper(word[(j-x)]) << " ";
                }
                else{
                    cout << getLetter(j,i) << " ";
                }
            }
            else if(!dir){
                if((j == x) && (i >= y) && (i < (y+word.size()))){
                    cout << (char) toupper(word[(i-y)]) << " ";
                }
                else{
                    cout << getLetter(j,i) << " ";
                }
            }
            
            }
        cout << endl;
    }
}

void popTop(){
    string top = scorePQ.top().word;
    char lets[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O'};
    cout << endl<<endl<< "Here is the board with the top scoring move played"<<endl<<endl;
    printTopBoard(scorePQ.top().zon, scorePQ.top().vert, scorePQ.top().dir, top);
    cout << endl<< "Here are the 20 top scoring moves:"<<endl;
    cout << endl;
    transform(top.begin(), top.begin() + 1, top.begin(), ::toupper);
    cout << "Word 1: " << top << " is worth "<<  scorePQ.top().score <<" points starting at ("<< lets[scorePQ.top().zon] << ", "<<  lets[scorePQ.top().vert] << ") going ";
    if(scorePQ.top().dir){
        cout << "Right "<< endl;
    }
    else {
        cout << "Down " << endl;
    }
    scorePQ.pop();
    
}

void popTop20(){
    
    popTop();
    cout << endl;
    for(int i = 0; i < 19; i++){
        string top = scorePQ.top().word;
    char lets[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O'};
    transform(top.begin(), top.begin() + 1, top.begin(), ::toupper);
    cout << "Word "<< i+2<< ": "<< top << " is worth "<<  scorePQ.top().score <<" points starting at ("<< lets[scorePQ.top().zon] << ", "<<  lets[scorePQ.top().vert] << ") going ";
    if(scorePQ.top().dir){
        cout << "Right "<< endl;
    }
    else {
        cout << "Down " << endl;
    }
    cout << endl;
    scorePQ.pop();
    }
}


string checked(int x, int y, bool dir, char letter){
    int X = x;
    int Y = y;
    string answer = "";
    //Word goes horizontal, change y coord.
    if(dir){
        while(((Y - 1) > -1) && inPlay(x,Y - 1)){
            Y = Y - 1;
        }
        if(inPlay(x,Y)){
            answer += tolower(getLetter(x,Y));
        }
        else if(Y == y){
            answer += letter;
        }
        while((Y < 14) && (inPlay(x,Y + 1) || ((Y + 1) == y))){
            Y = Y + 1;
            if(Y == y){
                answer += letter;
            }else{
            answer += tolower(getLetter(x,Y));
            }
        }
        if(answer.size() == 1){
            answer = "cat";
        }
        return answer;
    }
    //else word goes vertical, change x coord.
    else{
        while(((X - 1) > -1) && inPlay(X - 1,y)){
            X = X - 1;
        }
        if(inPlay(X,y)){
            answer += tolower(getLetter(X,y));
        }else if(X == x){
            answer += letter;
        }
        while(( X < 14) && (inPlay(X + 1,y) || ((X + 1) == x))) {
             X = X + 1;
             if(X == x){
                answer += letter;
            }else{
            answer += tolower(getLetter(X,y));
            }
        }
        if(answer.size() == 1){
            answer = "cat";
        }
        return answer;
    }
}

vector<Move> checkWords(vector<Move> moves, Trie trie){
    
    for(int i = 0; i < moves.size(); i++){
        int x = moves[i].x;
        int y = moves[i].y;
        int right = moves[i].dir;
        int up = !moves[i].dir;
        for(int j = 0; j < moves[i].words.size(); j++){
            
             for(int k = 0; k < moves[i].words[j].size(); k++){
                if((x + k*right) < 15 && ((y + k*up) < 15)){
                if(!trie.hasWord( (char*)checked(x + k*right, y + k*up, right, moves[i].words[j][k]).c_str(), checked(x + k*right, y + k*up, right, moves[i].words[j][k]).size())){
                    moves[i].words[j] = "";
                    break;
                }
                }
            }
        }   
    }
    return moves;
}


void runBoard(string boardName){
    setScores();
    readTestBoard(boardName);
    readMods();
    readRack(boardName);
    Trie root;
    string temp = "";
    vector<string> results;
    root = readWords();
    vector< vector <int> > test;
    test = findSpots();
    vector<Move> ans = possibleWords(test,root);
    scoreAll(checkWords(ans, root));
    cout <<endl<< " Here is the given rack of tiles"<<endl;
    cout <<endl<< "               ";
    string line;
    ifstream myfile (boardName.c_str());
    if (myfile.is_open()){
        getline (myfile,line);
        cout << line;
        myfile.close();
    }
    
    popTop20();
}


/*END OF SCORING*/





int main() {

    runBoard("LiveBoard.txt");
    // setScores();
    // readTestBoard("Board2.txt");
    // readMods();
    // readRack("Board2.txt");
    // cout << scoreWord(9,13,1,"inapt");

}
