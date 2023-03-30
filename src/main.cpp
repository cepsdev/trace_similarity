/*
MIT License

Copyright (c) 2023 Tomas Prerovsky

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

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

struct PoorMansSuffixTrie{

 static constexpr bool debug{false};

 struct Branch{
    int seq_start;
    int next_branch;
 };
 
 struct  __attribute__((packed)) Character{
    Character() = default;
    Character(char ch,int next):next{next},ch{ch},touched{},stamped{}{}
    int next; /* 'Tricky' encoding: 
     > 0 then next is a Character, 
     < 0 then next is a branch, 
     == 0 marks a leaf. (0 is always used by the starting branch)*/
    char ch;
    bool touched[2];
    bool stamped;
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
 
 vector<Node> trie;
 int cur_it = 0;
 
 PoorMansSuffixTrie() {
    trie.push_back(Node{});
 };

 int mk_char_node(char ch) {
    auto ofs = trie.size();
    trie.push_back(Node{ch});
    return ofs;
 }
 int mk_branch_node(int char_node_ofs) {
    auto ofs = trie.size();
    trie.push_back(Node{char_node_ofs});
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
    return trie[cur_it];
 }
 Node& node(int j) {
    return trie[j];
 }
 
 void stamp(bool value = true){
    node().character.stamped = value;
 }

bool touch(int which, bool value = true){
    auto t = node().character.touched[which];
    node().character.touched[which] = value;
    return t;
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
    PoorMansSuffixTrie suffix_trie;
    Logs logs;
    map<string,bool> on_off_options;
    map<string,string> param_options;

    set<string> on_off_options_names { {"dump_bag_of_words"} };
    vector<string> filepaths;

    auto print_usage = [&](ostream& os){
        os << "Usage: " << argv[0] << " file1 [file2 ...]";
        for(auto s : on_off_options_names) os << " [--"  << s << "]" ;
        os << '\n';
    };

    if (argc < 2) {
        print_usage(cerr);return 1;
    }

    for (auto args = argv; *++args;){
        //cout << *args << endl;
        string s{*args};
        if (s.substr(0,2) == "--"){
            auto it_on_off_opt = on_off_options_names.find(s.substr(2));
            auto it_param_opt = param_options.find(s.substr(2));

            if (it_on_off_opt != on_off_options_names.end()){

            } else if (it_param_opt != param_options.end()){

            } else { cerr << "***Error: Unknown option ["<< s <<"]. \n";print_usage(cerr); return 2;}
        }
        else filepaths.push_back(s);
    }

    if (filepaths.empty()) {
        cerr << "***Error: No log files specified.\n";print_usage(cerr); 
        return 3;
    }

    auto extract_states = [&suffix_trie,&logs] (istream& is, auto epilogue){
        for(string s;getline(is,s);){
            if (s.length() < 2) continue;
            auto last_pos{0};
            suffix_trie.start(s[0]);
            for(auto j{1}; j < s.length(); ++j){
                suffix_trie.step(s[j]);
                if (s[j] != '+' && s[j] != '-') continue;
                string state = s.substr(last_pos, j - last_pos + 1);
                epilogue();                
                for(last_pos = j+1;last_pos < s.length() && s[last_pos] == ' '; ++last_pos);
                if (last_pos < s.length()) suffix_trie.start(s[last_pos]);
                j = last_pos;
            }
        }
    } ;


    int no_of_states_in_query_log {};
    {
        ifstream is{filepaths[0]};
        extract_states(is,[&](){
            suffix_trie.stamp(); 
            no_of_states_in_query_log += !suffix_trie.touch(0) ? 1 : 0; 
        }); 
    }
    cout << no_of_states_in_query_log << "\n";

    for(size_t i = 2; i < filepaths.size(); ++i)
    {
        ifstream is{argv[i]};
        extract_states(is,[&](){}); 
    }
    //cout << logs << '\n';
}