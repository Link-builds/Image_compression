#include "CHuffmanCanon.h"

void HuffmanCanon::build_huffman_tree(const std::vector<byte_t> &data)
{
    std::unordered_map<byte_t, int> freqs;
    for (byte_t c : data)
        freqs[c]++;

    for (auto& x : freqs)
        m_tree.push_back(std::make_unique<TreeNode>(x.first, x.second));

    std::make_heap(m_tree.begin(), m_tree.end(), comparator{});

    while (m_tree.size() > 1)
    {
        std::pop_heap(m_tree.begin(), m_tree.end(), comparator{});
        auto left = std::move(m_tree.back());
        m_tree.pop_back();

        std::pop_heap(m_tree.begin(), m_tree.end(), comparator{});
        auto right = std::move(m_tree.back());
        m_tree.pop_back();

        int sum = left->freq + right->freq;

        m_tree.push_back(std::make_unique<TreeNode>('\0', sum, std::move(left), std::move(right)));
        std::push_heap(m_tree.begin(), m_tree.end(), comparator{});
    }
}

void HuffmanCanon::generate_code_lenghts(TreeNode *root, int codeLen)
{
    if (root == nullptr)
        return ;

    if (root->left == nullptr && root->right == nullptr)
    {
        m_canonLens[codeLen == 0 ? 1 : codeLen].insert(root->ch);
        return ;
    }

    generate_code_lenghts(root->left.get(), codeLen + 1);
    generate_code_lenghts(root->right.get(), codeLen + 1);
}

void HuffmanCanon::get_canon_codes()
{
    int code = 0;
    int currLen = 0;
    int nextLen = 0;

    for (auto i = m_canonLens.begin(); i != m_canonLens.end(); i++)
    {
        std::set<byte_t> s = i->second;

        currLen = i->first;

        for (auto j = s.begin(); j != s.end(); j++)
        {
            // std::cout << *j << ": " << std::bitset<32>(code).to_string().substr(32 - currLen, 32) << '\n';

            m_codes[*j] = std::bitset<32>(code).to_string().substr(32 - currLen, 32);

            if (next(j) != s.end() || next(i) == m_canonLens.end())
                nextLen = currLen;
            else
                nextLen = next(i)->first;

            code = (code + 1) << (nextLen - currLen);
        }
    }
}

void HuffmanCanon::encode(const std::vector<byte_t> &data)
{
    for (byte_t bit : data)
        m_encodedData += m_codes[bit];

    m_tree.clear();
}

std::unique_ptr<DecodeNode> HuffmanCanon::build_decode_tree(const std::unordered_map<byte_t, std::string> &codes) const
{
    auto root = std::make_unique<DecodeNode>();

    for (const auto &x : codes)
    {
        DecodeNode *curr = root.get();

        for (const auto &bit : x.second)
        {
            auto &next = (bit == '0') ? curr->left : curr->right;

            if (!next) next = std::make_unique<DecodeNode>();
            curr = next.get();
        }

        curr->isLeaf = true;
        curr->ch = x.first;
    }

    return root;
}

std::string HuffmanCanon::decode() const
{
    if (m_codes.empty()) return "";

    auto root = build_decode_tree(m_codes);

    std::string res;

    const DecodeNode *curr = root.get();

    for (const auto bit : m_encodedData)
    {
        curr = (bit == '0') ? curr->left.get() : curr->right.get();

        if (curr->isLeaf)
        {
            res += static_cast<char>(curr->ch);
            curr = root.get();
        }
    }

    return res;
}

HuffmanCanon::HuffmanCanon(const std::vector<byte_t> &data)
{
    if (!data.empty())
    {
        build_huffman_tree(data);
        generate_code_lenghts(m_tree.front().get(), 0);
        get_canon_codes();
        encode(data);
    }
}

HuffmanCanon::HuffmanCanon(const std::map<int, std::set<byte_t>> &canonLens, const std::string &encodedData) :
        m_canonLens(canonLens), m_encodedData(encodedData)
{
    get_canon_codes();
}

HuffmanCanon HuffmanCanon::as_encoder(const std::vector<byte_t> &data)
{
    if (data.empty()) return HuffmanCanon();

    return HuffmanCanon(data);
}

HuffmanCanon HuffmanCanon::as_decoder(const std::map<int, std::set<byte_t>> &canonLens, const std::string &encodedData)
{
    return HuffmanCanon(canonLens, encodedData);
}

void HuffmanCanon::print_codes() const
{
    if (m_codes.empty())
        std::cout << "Empty input!\n";
    else
        for (const auto& x : m_codes)
            std::cout << x.first << ' ' << x.second << '\n';
}