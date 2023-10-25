#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class TreeNode {
public:
    string name;                    // Name of the node
    vector<TreeNode*> children;     // Children of the node
    TreeNode(string n) : name(n) {} // Constructor
};

class Tree {
public:
    TreeNode* root;

    // 'string' for key, 'TreeNode*' for value -> 이름으로 노드를 찾기 위한 자료구조
    // nodes[name] : Tree 클래스 내에서 'name'을 이름으로 가지는 노드를 찾아서 반환
    map<string, TreeNode*> nodes;
    Tree() : root(nullptr) {}        // constructor

    // 'name' 노드를 생성하여, 'parentName' 노드 하위에 추가
    void addNode(string name, string parentName) {
        TreeNode* node = new TreeNode(name);
        nodes[name] = node;
        // 부모노드 파라미터가 비어있다면 해당 노드를 루트로, 아니면 부모노드의 'children' vector에 push_back()
        if (parentName.empty()) {
            root = node;
        } else {
            nodes[parentName]->children.push_back(node);
        }
    }
};

void parseDTS(Tree& tree, string filename) {
    ifstream file(filename);
    string line;
    string currentParent = "";
    while (getline(file, line)) {
        if (line.find("{") != string::npos) {
            // new node
            string name = line.substr(0, line.find(":"));
            tree.addNode(name, currentParent);
            currentParent = name;
        } else if (line.find("};") != string::npos) {
            // end of node
            currentParent = "";
        }
    }
}

int main(int argc, char* argv[]) {
    Tree tree;
    string input_file_path = "";
    string output_file_path = "";
    input_file_path = argv[1];
    

    if (argc != 3) {
        cout << "[-] Usage: ./parse_to_tree <input_file_path> <output_file_path>" << endl;
        exit(1);
    }
    
    parseDTS(tree, input_file_path);
    // do something with the tree
    return 0;
}

