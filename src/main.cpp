#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;



struct Logs{
    vector<int> v;
    void push(int i){
        v.push_back(i);
    }
    vector<int> const & data() const {
        return v;
    }
};

struct PoorMansSuffixTree{
 
 struct Branch{
    int seq_start;
    int next_branch;
 };
 
 struct Character{
    char ch;
    int next;
 };

 struct Node{
  Node() {
    branch = Branch{0,0};
  }
  Node(char ch) {
    character = Character{ch,0};
  }
  union {
    Branch branch;
    Character character;
  };
 };
 
 vector<Node> tree;
 int cur_it = 0;
 int last_it = 0;
 bool cur_it_branch{true};
 bool last_it_branch{true};
 
 PoorMansSuffixTree() {
    tree.push_back(Node{});
 };

 int mk_char_node(char ch) {
    auto ofs = tree.size();
    tree.push_back(Node{ch});
    return ofs;
 }
 void start(char ch){
    cur_it = 0;
    last_it = 0;
    auto& branch = node(0);
    if (!branch.branch.seq_start){
        branch.branch.seq_start = mk_char_node(ch);
        cur_it = branch.branch.seq_start;
    } else {
        bool found{};
        for(;!(found = node(node(cur_it).branch.seq_start).character.ch == ch);){
            if (node(cur_it).branch.next_branch)
             cur_it = node(cur_it).branch.next_branch;
            break;
        }
        if (found) cur_it = node(cur_it).branch.seq_start;

    }
 }

 Node& node() {
    return tree[cur_it];
 }
 Node& node(int j) {
    return tree[j];
 }
 
 Node& node_before() {
    return tree[last_it];
 }

 void step(char ch){
    //INVARIANT cur_t points to a Character
    auto& n{node()};
    if (n.character.next > 0 && node(n.character.next).character.ch == ch);
     //cur_it =  
 }

 int code(){
    return cur_it;
 }
};

ostream& operator << (ostream& os, Logs const & logs){
    for(auto i : logs.data())
    {
        if (!i) os << '\n' << '\n';
        else os << i << ' ';
    }
    return os;
}

int main(int argc, char** argv){
    PoorMansSuffixTree suffix_tree;
    Logs logs;

    for(auto i = 1; i < argc; ++i)
    {
        ifstream is{argv[i]};
        for(string s;getline(is,s);){
            if (s.length() < 2) continue;
            auto last_pos{0};
            suffix_tree.start(s[0]);
            for(auto j{1}; j < s.length(); ++j){
                suffix_tree.step(s[j]);
                if (s[j] != '+' && s[j] != '-') continue;
                string state = s.substr(last_pos, j - last_pos + 1);
                logs.push(suffix_tree.code());                
                //cout << state << endl;
                for(last_pos = j+1;last_pos < s.length() && s[last_pos] == ' '; ++last_pos);
                if (last_pos < s.length()) suffix_tree.start(s[last_pos]);
            }
            logs.push(0);
        }
    }
    cout << logs << '\n';

}