#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include <unordered_map>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <bitset>

using byte_t = unsigned char;

struct TreeNode
{
    byte_t ch;
    int freq;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

    TreeNode(byte_t ch, int freq) : ch(ch), freq(freq) {}

    TreeNode(byte_t ch, int freq, std::unique_ptr<TreeNode> l, std::unique_ptr<TreeNode> r) :
        ch(ch), freq(freq), left(std::move(l)), right(std::move(r)) {}
};

struct comparator
{
    bool operator () (const std::unique_ptr<TreeNode>& left, const std::unique_ptr<TreeNode>& right) const
    {
        return left->freq > right->freq;
    }
};

class HuffmanCanon
{
private:
    std::vector<std::unique_ptr<TreeNode>> m_tree;
    std::map<int, std::set<byte_t>> m_canonLens;
    std::unordered_map<byte_t, std::string> m_codes;
    std::string m_encodedData = "";

    void build_huffman_tree(const std::vector<byte_t> &data);
    void generate_code_lenghts(TreeNode *root, int codeLen);
    void get_canon_codes();

    void encode(const std::vector<byte_t> &data);
    
    HuffmanCanon() {};
    HuffmanCanon(const std::vector<byte_t> &data);
    HuffmanCanon(const std::map<int, std::set<byte_t>> &canonLens, const std::string &encodedData);

public:
    ~HuffmanCanon() {};

    static HuffmanCanon as_encoder(const std::vector<byte_t> &data);
    static HuffmanCanon as_decoder(const std::map<int, std::set<byte_t>> &canonLens, const std::string &encodedData);

    std::string decode() const;

    void print_codes() const;
    std::string encodedData() const { return m_encodedData; };
    const std::map<int, std::set<byte_t>>& canonLens() const { return m_canonLens; };
};
