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
    map<string, string> attributes;  // Store attributes here
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

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    if (start == string::npos || end == string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

void parseDTS(Tree& tree, string filename) {
    ifstream file(filename);
    string line;
    int depth = 0; // 현재 깊이를 나타내는 변수
    vector<string> parentStack; // 부모 노드들의 스택을 담는 string 형태의 벡터 선언
    int lineNumber = 0; // 추가: 현재 줄 번호를 추적하기 위한 변수

    while (getline(file, line)) {
        lineNumber++; // 줄 번호 증가

        // 빈 줄이면 무시하고 다음 줄로 넘어갑니다.
        if (line.empty()) {
            continue;
        }

        int leadingSpaces = 0; // 라인 앞의 공백 문자 개수
        while (leadingSpaces < line.size() && line[leadingSpaces] == '\t') {
            leadingSpaces++;
        }
        
        size_t equalPos = line.find("=");
        if (equalPos != string::npos) {
            string key = trim(line.substr(0, equalPos));
            string value = trim(line.substr(equalPos + 1));
            if (value.back() == ';') {
                value.pop_back(); // Remove trailing semicolon
            }
            if (!parentStack.empty()) {
                tree.nodes[parentStack.back()]->attributes[key] = value;
            }
        } else if (line.find("{") != string::npos) {
                cout << "[-] Opening brace found at line " << lineNumber << endl;
            // 여는 중괄호를 발견했을 때의 들여쓰기 검사
            if (depth != leadingSpaces) {
                cerr << "[-] Incorrect indentation detected at line " << lineNumber << ": \"" << line << "\" | leadingspaces : " << leadingSpaces << " | depth : " << depth << endl;
                exit(1);
            }
            depth++;    // 이제 leadingspaces + 1 == depth

            string name = trim(line.substr(0, line.find("{")));
            string parentName = parentStack.empty() ? "" : parentStack.back();
            tree.addNode(name, parentName);
            parentStack.push_back(name);
        } else if (line.find("};") != string::npos) {
            cout << "[-] Closing brace found at line " << lineNumber << endl;
            depth--;
            if (depth != leadingSpaces) {
                cerr << "[-] Incorrect indentation detected at line " << lineNumber << endl;
                exit(1);
            }
            if (!parentStack.empty()) {
                parentStack.pop_back();
            }
        } else if (line.back() == ';') {
            // Handle the case where there is no '=' sign, and line ends with a semicolon
            string key = trim(line.substr(0, line.size() - 1)); // Remove trailing semicolon
            if (!parentStack.empty()) {
                // Assign an empty string or some other default value as the attribute value
                tree.nodes[parentStack.back()]->attributes[key] = "";
            }
        }
    }
}

void printTree(TreeNode* node, int depth = 0) {
    if (!node) return;

    // depth만큼 들여쓰기를 추가
    for (int i = 0; i < depth; i++) {
        cout << "  "; // 2개의 공백 문자로 들여쓰기
    }

    cout << node->name << endl; // 현재 노드의 이름 출력

    // 현재 노드의 모든 속성에 대해 출력
    for (const auto& attr : node->attributes) {
        for (int i = 0; i <= depth; i++) {
            cout << "  ";
        }
        cout << attr.first; // key 출력
        if (!attr.second.empty()) { // value가 비어 있지 않다면 key와 value 출력
            cout << " = " << attr.second;
        }
        cout << endl;
    }

    // 현재 노드의 모든 자식 노드에 대해 재귀적으로 printTree 호출
    for (TreeNode* child : node->children) {
        printTree(child, depth + 1);
    }
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "[-] Usage: ./parse_to_tree <input_file_path> <output_file_path>" << endl;
        exit(1);
    }

    Tree tree;
    string input_file_path = "";
    string output_file_path = "";
    input_file_path = argv[1];
    output_file_path = argv[2];
    if (input_file_path.empty() || output_file_path.empty()) {
        cout << "[-] Usage: ./parse_to_tree <input_file_path> <output_file_path>" << endl;
        exit(1);
    }
    
    parseDTS(tree, input_file_path);
    printTree(tree.root);
    return 0;
}

