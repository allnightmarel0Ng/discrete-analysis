#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <utility>
#include <cstdint>

#ifdef LAB3
#include "dmalloc.h"
#endif

template<typename Key, typename Value>
class AVL_tree
{
public:

    explicit AVL_tree():
        _root(nullptr)
    {

    }

public:

    virtual ~AVL_tree()
    {
        clear(_root);
    }

    AVL_tree(const AVL_tree<Key, Value> &other):
        _root(copy(other._root))
    {

    }

    AVL_tree &operator=(const AVL_tree<Key, Value> &other)
    {
        if (this != &other)
        {
            clear(_root);
            _root = copy(other._root);
        }

        return *this;
    }

    AVL_tree(AVL_tree<Key, Value> &&other) noexcept:
        _root(std::exchange(other._root, nullptr))
    {

    }

    AVL_tree &operator=(AVL_tree<Key, Value> &&other) noexcept
    {
        if (this != &other)
        {
            _root = std::exchange(other._root, nullptr);
        }

        return *this;
    }


public:

    void insert(const Key &key, const Value &value)
    {
        _root = insert(_root, key, value);
    }

    void remove(const Key &key)
    {
        _root = remove(_root, key);
    }

    Value &find(const Key &key) const
    {
        auto it = _root;
        while (it != nullptr)
        {
            if (key == it->key)
            {
                return it->value;
            }
            it = key < it->key ? it->left : it->right;
        }

        throw std::logic_error("NoSuchWord");
    }

public:

    void save(const std::string &path) const
    {
        std::ofstream ofs(path, std::ios_base::binary | std::ios_base::trunc);
        save(_root, ofs);
        ofs.close();
    }

    void load(const std::string &path)
    {
        node *new_root = nullptr;
        std::ifstream ifs(path, std::ios_base::binary);
        try
        {
            new_root = load(ifs);
            clear(_root);
            _root = new_root;
            ifs.close();
        }
        catch (const std::exception &exception)
        {
            clear(new_root);
            ifs.close();
            throw exception;
        }

    }

private:
    struct node
    {
        Key key;
        Value value;
        node *left, *right;
        int height;

        explicit node(const Key &key, const Value &value):
            key(key), value(value), left(nullptr), right(nullptr), height(1)
        {

        }
    };

    inline int height(node *node) const noexcept
    {
        return node != nullptr ? node->height : 0;
    }

    inline int balance_factor(node *node) const noexcept
    {
        return node != nullptr ? height(node->left) - height(node->right) : 0;
    }

    inline void update_height(node *node) noexcept
    {
        if (node != nullptr)
        {
            node->height = 1 + std::max(height(node->left), height(node->right));
        }
    }

    node *rotate_right(node *node) noexcept
    {
        auto tmp1 = node->left;
        auto tmp2 = tmp1->right;

        tmp1->right = node;
        node->left = tmp2;

        update_height(node);
        update_height(tmp1);

        return tmp1;
    }

    node *rotate_left(node *node) noexcept
    {
        auto tmp1 = node->right;
        auto tmp2 = tmp1->left;

        tmp1->left = node;
        node->right = tmp2;

        update_height(node);
        update_height(tmp1);

        return tmp1;
    }

    node *balance(node *node) noexcept
    {
        update_height(node);

        auto bf = balance_factor(node);
        if (bf == 2)
        {
            if (balance_factor(node->left) < 0)
            {
                node->left = rotate_left(node->left);
            }
            return rotate_right(node);
        }

        if (bf == -2)
        {
            if (balance_factor(node->right) > 0)
            {
                node->right = rotate_right(node->right);
            }
            return rotate_left(node);
        }

        return node;
    }

    node *insert(node *where, const Key &key, const Value &value)
    {
        if (where == nullptr)
        {
            return new node(key, value);
        }

        if (key < where->key)
        {
            where->left = insert(where->left, key, value);
        }
        else if (key > where->key)
        {
            where->right = insert(where->right, key, value);
        }
        else
        {
            throw std::logic_error("Exist");
        }

        return balance(where);
    }

    node *find_min(node *node) const noexcept
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        return node->left != nullptr ? find_min(node->left) : node;
    }

    node *remove_min(node *node) noexcept
    {
        if (node == nullptr)
        {
            return node;
        }

        if (node->left == nullptr)
        {
            return node->right;
        }

        node->left = remove_min(node->left);
        return balance(node);
    }

    node *remove(node *node, const Key &key)
    {
        if (node == nullptr)
        {
            throw std::logic_error("NoSuchWord");
        }

        if (key < node->key)
        {
            node->left = remove(node->left, key);
        }
        else if (key > node->key)
        {
            node->right = remove(node->right, key);
        }
        else
        {
            auto left = node->left;
            auto right = node->right;
            delete node;

            if (right == nullptr)
            {
                return left;
            }

            auto min = find_min(right);
            min->right = remove_min(right);
            min->left = left;

            return balance(min);
        }

        return balance(node);
    }

    void clear(node *node) noexcept
    {
        if (node != nullptr) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    node *copy(node *from)
    {
        node *new_node = new node(from->key, from->value);
        new_node->left = copy(from->left);
        new_node->right = copy(from->right);
        return new_node;
    }

    void save(node *node, std::ofstream &os) const
    {
        if (node == nullptr)
        {
            return;
        }

        if (typeid(Key) == typeid(std::string))
        {
            int size = node->key.size();
            os.write(reinterpret_cast<const char *>(&size), sizeof(int));
            os.write(node->key.c_str(), size);
        }
        else
        {
            os.write(reinterpret_cast<const char *>(&node->key), sizeof(Key));
        }
        os.write(reinterpret_cast<const char *>(&node->value), sizeof(Value));

        save(node->left, os);
        save(node->right, os);
    }

    node *load(std::ifstream &is)
    {
        if (is.eof())
        {
            return nullptr;
        }

        int size = 0;
        node *new_root = nullptr;

        while (is.read(reinterpret_cast<char*>(&size), sizeof(int)))
        {
            std::string key;
            key.resize(size);
            is.read(key.data(), size);
            unsigned long long value;
            is.read(reinterpret_cast<char*>(&value), sizeof(value));
            new_root = insert(new_root, key, value);
        }

        return new_root;
    }


private:
    node *_root;

};

int main()
{
#ifndef LAB3
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
#endif

    AVL_tree<std::string, std::uint64_t> tree;

    std::string command;
    std::string word;

    while (std::cin >> command)
    {
        if (command == "!")
        {
            std::string path;
            std::cin >> word >> path;
            if (word == "Save")
            {
                try
                {
                    tree.save(path);
                    std::cout << "OK" << std::endl;
                }
                catch (const std::exception &exception)
                {
                    std::cout << "ERROR:" << exception.what() << std::endl;
                }
            }
            else if (word == "Load")
            {
                try
                {
                    tree.load(path);
                    std::cout << "OK" << std::endl;
                }
                catch (const std::exception &exception)
                {
                    std::cout << "ERROR:" << exception.what() << std::endl;
                }
            }
        }
        else if (command == "-")
        {
            std::cin >> word;
            transform(word.begin(), word.end(), word.begin(), ::tolower);

            try
            {
                tree.remove(word);
                std::cout << "OK" << std::endl;
            }
            catch (const std::logic_error &exception)
            {
                std::cout << exception.what() << std::endl;
            }
        }
        else if (command == "+")
        {
            std::uint64_t value;
            std::cin >> word >> value;
            transform(word.begin(), word.end(), word.begin(), ::tolower);

            try
            {
                tree.insert(word, value);
                std::cout << "OK" << std::endl;
            }
            catch (const std::logic_error &exception)
            {
                std::cout << exception.what() << std::endl;
            }
        }
        else
        {
            transform(command.begin(), command.end(), command.begin(), ::tolower);
            std::uint64_t result;
            try
            {
                result = tree.find(command);
            }
            catch (const std::logic_error &exception)
            {
                std::cout << exception.what() << std::endl;
                continue;
            }
            std::cout << "OK: " << result << std::endl;
        }
    }

    return 0;
}