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

Tree tree_qemu, tree_rev, tree_custom;

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
                // cout << "[-] Opening brace found at line " << lineNumber << endl;
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
            // cout << "[-] Closing brace found at line " << lineNumber << endl;
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

// void compareTree(Tree& tree1, Tree& tree2, string output_file_path) {
//     ofstream output_file(output_file_path);
//     if (!output_file.is_open()) {
//         cout << "[-] Failed to open output file" << endl;
//         exit(1);
//     }

//     // tree1의 모든 노드에 대해 반복
//     for (const auto& node : tree1.nodes) {
//         // tree2에 같은 이름의 노드가 없다면, tree1의 해당 노드를 출력
//         if (tree2.nodes.find(node.first) == tree2.nodes.end()) {
//             output_file << node.first << endl;
//             for (const auto& attr : node.second->attributes) {
//                 output_file << attr.first;
//                 if (!attr.second.empty()) {
//                     output_file << " = " << attr.second;
//                 }
//                 output_file << endl;
//             }
//             output_file << endl;
//         }
//     }
// }

void mergeNodes(TreeNode* node_qemu, TreeNode* node_rev, TreeNode* node_custom) {
    // Debug: Check if any node is null before merging
    if (!node_qemu) {
        cerr << "[-] node_qemu is null" << endl;
        exit(-1);
    }
    if (!node_rev) {
        cerr << "[-] node_rev is null" << endl;
        exit(-1);
    }
    if (!node_custom) {
        cerr << "[-] node_custom is null, initializing..." << endl;
        node_custom = new TreeNode(""); // Initialize with an empty name, or appropriate name
    }

    // Debugging: Print current node being processed
    cout << "[+] Merging node: " << node_qemu->name << endl;
    
    // Merge attributes
    for (auto& attr : node_qemu->attributes) {
        node_custom->attributes[attr.first] = attr.second;
    }
    for (auto& attr : node_rev->attributes) {
        if (node_custom->attributes.find(attr.first) == node_custom->attributes.end()) {
            // Only add if the attribute doesn't exist
            node_custom->attributes[attr.first] = attr.second;
        }
    }

    // Debugging: Print attributes merged
    cout << "    Attributes merged for node: " << node_qemu->name << endl;

    // Merge children
    for (auto* child_qemu : node_qemu->children) {
        // Check if this child exists in node_rev
        TreeNode* child_rev = nullptr;
        for (auto* child : node_rev->children) {
            if (child->name == child_qemu->name) {
                child_rev = child;
                break;
            }
        }

        TreeNode* child_custom = new TreeNode(child_qemu->name);

        // Debugging: Print child node being processed
        cout << "Processing child node: " << child_qemu->name << endl;

        node_custom->children.push_back(child_custom);

        if (child_rev) {
            // If the child exists in both, merge them
            mergeNodes(child_qemu, child_rev, child_custom);
        } else {
            // Otherwise, copy the tree_qemu child
            mergeNodes(child_qemu, child_qemu, child_custom);  // Passing the same node to copy it
        }
    }
}

// This function writes the tree to the output file in DTS format
void writeTreeToFile(TreeNode* node, ofstream& output_file, int depth = 0) {
    // Check if the file stream is in good state
    if (!output_file) {
        cerr << "File stream is in bad state!" << endl;
        return;
    }
    
    
    if (!node) return;

    // Indent the output based on the depth of the node
    string indent(depth * 2, ' ');

    // Write the node name with opening brace
    output_file << indent << node->name << " {" << "\n";

    // Debug: Print to console as well to verify
    cout << string(depth * 2, ' ') << node->name << " {" << endl;  // Example of output



    // Write the attributes
    for (const auto& attr : node->attributes) {
        output_file << indent << "  " << attr.first;
        if (!attr.second.empty()) {
            output_file << " = " << attr.second;
        }
        output_file << ";" << "\n";
    }

    // Recursively write the children
    for (TreeNode* child : node->children) {
        writeTreeToFile(child, output_file, depth + 1);
    }

    // Write closing brace with appropriate indentation
    output_file << indent << "};" << "\n";
}

void compareDTS(Tree& tree_qemu, Tree& tree_rev, string output_file_path) {
    // Debugging: Print message indicating the start of the merge process
    cout << "[-] Starting the merge process..." << endl;

    // Populate tree_custom with the merged nodes using the logic discussed previously
    mergeNodes(tree_qemu.root, tree_rev.root, tree_custom.root);

    // Debugging: Print message indicating the end of the merge process
    cout << "Merge process completed." << endl;

    // Now write the tree_custom to the output file
    ofstream output_file(output_file_path);
    if (!output_file.is_open()) {
        cerr << "Failed to open output file." << endl;
        return;
    }

    // Debugging: Print message indicating the start of the writing process
    cout << "Starting to write the merged tree to the output file..." << endl;

    // Write the merged tree to the output file
    writeTreeToFile(tree_custom.root, output_file);

    // Debugging: Print message indicating the end of the writing process
    cout << "Finished writing the merged tree to the output file." << endl;

    // Close the output file
    output_file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "[-] Usage: ./parse_to_tree <qemu_path> <rev3_path> <output_file_path>" << endl;
        exit(1);
    }

    // Tree tree_qemu, tree_rev, tree_custom;
    string input_qemu_path = "";
    string input_rev_path = "";
    string output_file_path = "";
    input_qemu_path = argv[1];
    input_rev_path = argv[2];
    output_file_path = argv[3];
    if (input_qemu_path.empty() || input_rev_path.empty() ||output_file_path.empty()) {
        cout << "[-] Usage: ./parse_to_tree <qemu_path> <rev3_path> <output_file_path>" << endl;
        exit(1);
    }
    
    parseDTS(tree_qemu, input_qemu_path);
    parseDTS(tree_rev, input_rev_path);
    // printTree(tree.root);
    compareDTS(tree_qemu, tree_rev, output_file_path);

    printTree(tree_custom.root);
    return 0;
}

