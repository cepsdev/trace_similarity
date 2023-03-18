#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
struct Logs{


};

int main(int argc, char** argv){
    for(auto i = 1; i < argc; ++i)
    {
        ifstream is{argv[i]};
        for(string s;getline(is,s);){
            auto last_pos{0};
            for(auto j{0}; j < s.length(); ++j){
                if (s[j] != '+' && s[j] != '-') continue;
                string state = s.substr(last_pos, j - last_pos + 1);
                cout << state << endl;
                for(last_pos = j+1;last_pos < s.length() && s[last_pos] == ' '; ++last_pos);
            }
        }
    }
}