#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <list>
#include <algorithm>
using namespace std;

//variable declearations
string algorithm; //Algo type
int memorySize; //Size of memory  use for Algo
string fileName; //Name of file
vector<string>inputCommands; //vector containing all command read for input file
int lookahead; //look ahead variable

void readFile(string algo,string memsize,string fname){
    //variables for reading
    algorithm = algo;
    memorySize = atoi(memsize.c_str());
    fileName = fname;
    string inpStr;
    ifstream inpfile;
    inpfile.open(fname);
    //open file and push the commands into vectors
    while(!inpfile.eof()){
        getline(inpfile,inpStr,' ');
        inputCommands.push_back(inpStr);
    }
}

//predictor is used to look ahead and remove the necessary number
int predict(vector<string>&cmd,vector<int>&fr, int number, int index){
    int far = 0;
    int loc = index;
    int p = index;
    for(int j = 0; j<fr.size();j++){ //loop for size of memory
        for(int i = index; i<cmd.size();i++){ //loop head x
            if(i < p+lookahead){ //run while smaller than look ahead
                if(cmd[i].substr(2,2) == "n"){
                    far = atoi(cmd[i].substr(0,1).c_str());
                } 
            }

        }
    }
    return far;
} 

int predict2(vector<string>&cmd,vector<int>&fr, int number, int index){
    int far = 0;
    int loc = index;
    int p = index;
    for(int j = 0; j<fr.size();j++){ //loop for size of memory
        for(int i = index; i<cmd.size();i++){ //loop head x
                if(fr[j] != atoi(cmd[i].substr(0,1).c_str())){
                    far = j;
                } 

        }
    }
    return far;
} 

void pagedOPT(){
    cout<<"Running OPT"<<endl;
    cout<<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<endl;
    int delays; //delay counter
    int pageFaults; //page fault counter
    int pageReference; //page reference counter
    vector<int>OPT; //queue for OPT
    queue<int>nobandQ; //queue for no banwidth
    set<int> s; //set to check if number is in buffer
    //reading file and parsing inputs
    for(int i = 0; i < inputCommands.size();i++){
        int number;
        string waitsignal;
        //parse inputs into two varibles
        number = atoi(inputCommands[i].substr(0,1).c_str());
        waitsignal = inputCommands[i].substr(2,2);
        cout<<number<<" : "<<waitsignal<<": ";
        if(waitsignal == "a"){
            if(OPT.size() < memorySize){
                //if there is space in buffer we add it in
                if(nobandQ.size() > 0){
                    if(s.find(nobandQ.front()) == s.end() ){
                        s.insert(nobandQ.front());
                        cout<<"Put Page "<< nobandQ.front() <<" into the memory."<<endl;
                        pageFaults++;
                        OPT.push_back(nobandQ.front());
                        nobandQ.pop();
                    }
                }
                //if there is space, we add it if number is not there in the buffer
                else if (s.find(number) == s.end()){
                    s.insert(number);
                    cout<<"Put Page "<< number <<" into the memory."<<endl;
                    pageFaults++;
                    OPT.push_back(number);
            
                }
                //contnue if there is number is already there
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                }
            }
            //if the buffer is full, we perform OPT removal
            else if(OPT.size() == memorySize){
                pageReference++;
                int j = predict(inputCommands,OPT,number,i);
                if( j!=0){
                    if(s.find(j) == s.end() ){
                    //predict is return the location of the replacement
                    int t = predict2(inputCommands,OPT,number,i);
                    //cout<<"J: "<<j<<endl;
                    cout<<"Remove Page "<<OPT[t]<< " from the memory."<<endl;
                    s.erase(OPT[t]);
                    OPT[t] = j;
                    s.insert(j);
                    //this will put the new page into memory
                    cout<<"Put Page "<< j <<" into the memory"<<endl;
                    pageFaults++;
                    }
                }
                else{
                    cout<<"Found Page "<<number<<" in memory."<<endl;
                }
            }
        }
        else if (waitsignal == "n"){
            //there is no bandwidth
            cout<<"Tries to put Page "<< number<< " into the memory, but air bandwidth is not available."<<endl;
            int j = predict(inputCommands,OPT,number,i);
            if( j!=0){
                if(s.find(j) == s.end() ){
                //predict is return the location of the replacement
                if(OPT.size()<memorySize){
                    s.erase(j);
                    OPT[1] = j;
                    s.insert(j);
                }
                else{
                    int t = predict2(inputCommands,OPT,number,i);
                    //cout<<"J: "<<j<<endl;
                    cout<<"Remove Page "<<OPT[t]<< " from the memory."<<endl;
                    s.erase(OPT[t]);
                    OPT[t] = j;
                    s.insert(j);
                    //this will put the new page into memory
                    cout<<"Put Page "<< j <<" into the memory"<<endl;

                }
                pageFaults++;
                }
            }
            if(s.find(number) == s.end() ){
                //adds to fifo queue
                nobandQ.push(number);
                delays++;
            }
            else{
                //does not add to queue if found
                cout<<"Found Page "<<number<<" in memory, so not queued."<<endl;
            }
            
        }

    }
    cout<<"Report:"<<endl;
    cout<<"The total number of page references: "<<pageReference<<endl;
    cout<<"The number of page faults: "<<pageFaults<<endl;
    cout<<"The number of delayed page replacements due to unavailable air bandwidth: "<<delays<<endl;
    cout<<"::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"<<endl;
}

int main(int argc, char *argv[]){
    string a;
    string b;
    string c;
    a = argv[1];
    b = argv[2];
    lookahead = atoi(argv[3]);
    c = argv[4];
    readFile(a,b,c);
    pagedOPT();
    return 0;
}