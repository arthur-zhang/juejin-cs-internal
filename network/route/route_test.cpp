#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Trie树节点结构
struct TrieNode {
    TrieNode* children[2];  // 左右子树,0表示左子树,1表示右子树
    bool isLeaf;            // 是否是叶子节点
    string prefix;          // 前缀
    string nexthop;         // 下一跳地址

    TrieNode() {
        children[0] = children[1] = nullptr;
        isLeaf = false;
    }
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() {
        root = new TrieNode();
    }

    // 插入路由条目
    void insert(string ip, int prefixLen, string nexthop) {
        TrieNode* node = root;
        for (int i = 0; i < prefixLen; i++) {
            int idx = ip[i] - '0';
            if (node->children[idx] == nullptr) {
                node->children[idx] = new TrieNode();
            }
            node = node->children[idx];
            node->prefix = ip.substr(0, i+1);
        }
        node->isLeaf = true;
        node->nexthop = nexthop;
    }

    // 最长前缀匹配查找
    string search(string ip) {
        TrieNode* node = root;
        for (int i = 0; i < ip.length(); i++) {
            int idx = ip[i] - '0';
            if (node->children[idx] == nullptr) {
                break;
            }
            node = node->children[idx];
        }

        // 回溯查找最长匹配前缀
        while (!node->isLeaf) {
            node = node->children[0] ? node->children[0] : node->children[1];
        }
        return node->nexthop;
    }
};

int main() {
    Trie trie;

    // 构造路由表
    trie.insert("10000000", 1, "192.168.1.1");
    trie.insert("11000000", 2, "192.168.2.1");
    trie.insert("11000000", 3, "192.168.3.1");
    trie.insert("11100000", 3, "192.168.4.1");

    // 测试最长前缀匹配
    cout << trie.search("11000011") << endl;  // 输出: 192.168.3.1
    cout << trie.search("11010101") << endl;  // 输出: 192.168.2.1
    cout << trie.search("11100011") << endl;  // 输出: 192.168.4.1
    cout << trie.search("10011000") << endl;  // 输出: 192.168.1.1

    return 0;
}