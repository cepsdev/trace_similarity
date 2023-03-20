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

 static constexpr bool debug{false};

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
  Node(int seq) {
    branch = Branch{seq,0};
  }  
  union {
    Branch branch;
    Character character;
  };
 };
 
 vector<Node> tree;
 int cur_it = 0;
 
 PoorMansSuffixTree() {
    tree.push_back(Node{});
 };

 int mk_char_node(char ch) {
    auto ofs = tree.size();
    tree.push_back(Node{ch});
    return ofs;
 }
 int mk_branch_node(int char_node_ofs) {
    auto ofs = tree.size();
    tree.push_back(Node{char_node_ofs});
    return ofs;
 }
 
 void start(char ch, int where = 0){
    if (debug)cout << "start(" << ch << "," << where << ")\n";
    cur_it = where;
    
    if (!node(cur_it).branch.seq_start){
        if (debug)cout << "start(" << ch << "," << where << "): seq_start == 0\n";
        node(cur_it).branch.seq_start = mk_char_node(ch);
        if (debug)cout << "start(" << ch << "," << where << "): branch.branch.seq_start == "<<node(cur_it).branch.seq_start<<"\n";
        cur_it = node(cur_it).branch.seq_start;
    } else {
        if (debug)cout << "start(" << ch << "," << where << "): seq_start != 0\n";
        bool found{};
        auto last_it = cur_it;
        for(; !(found = (node(node(cur_it).branch.seq_start).character.ch == ch));){
             last_it = cur_it;
             cur_it = node(cur_it).branch.next_branch;
             if (!cur_it) break;
        }
        if (found) {cur_it = node(cur_it).branch.seq_start;return;}
        //INVARIANT cur_it points to end of branch list
        auto seq = mk_char_node(ch);
        node(last_it).branch.next_branch = mk_branch_node(seq);
        cur_it = seq;
    }
 }

 Node& node() {
    return tree[cur_it];
 }
 Node& node(int j) {
    return tree[j];
 }
 
 
 void step(char ch){
    //INVARIANT cur_t points to a Character
    if (debug)cout << "cur_it=" << cur_it << ")\n";
    
    if (node().character.next){
        if (node().character.next > 0) /*Next one is also a character*/
        {
            if (node(node().character.next).character.ch == ch) {
                cur_it = node().character.next; return; 
            }
            /*We need to insert a branch*/
            auto seq = mk_char_node(ch);
            auto branch_a = mk_branch_node(node().character.next);
            auto branch_b = mk_branch_node(seq);
            node(branch_a).branch.next_branch = branch_b; 
            node().character.next = -branch_a;
            cur_it = seq;
        } else 
            start(ch,-node().character.next);
    } else /*End of character chain*/      
       cur_it = node().character.next = mk_char_node(ch);
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
                cout << state << " --> " << suffix_tree.code() <<  endl;
                for(last_pos = j+1;last_pos < s.length() && s[last_pos] == ' '; ++last_pos);
                if (last_pos < s.length()) suffix_tree.start(s[last_pos]);
                j = last_pos;
            }
            logs.push(0);
        }
    }
    //cout << logs << '\n';

}