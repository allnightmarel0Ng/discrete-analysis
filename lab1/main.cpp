#include <cstdlib>
#include <stdexcept>
#include <cstring>

namespace lab {
    template<typename T>
    class vector;

    struct pair;

    constexpr std::size_t MAX_KEY = 65536;
    constexpr std::size_t MAX_STRING_LENGTH = 64;
}

struct lab::pair
{
    int key;
    char value[MAX_STRING_LENGTH + 1];
};

template<typename T>
class lab::vector
{
public:
    vector():
        _size(0), _capacity(2)
    {
        _data = static_cast<T *>(std::malloc(sizeof(T) * _capacity));
        if (_data == nullptr)
        {
            throw std::runtime_error("Allocation error");
        }
    }

public:
    virtual ~vector()
    {
        free(_data);
    }

public:
    void push_back(const T &value)
    {
        _data[_size++] = value;
        if (_size == _capacity)
        {
            _capacity *= 2;
            auto tmp = static_cast<T *>(std::realloc(_data, sizeof(T) * _capacity));
            if (tmp == nullptr)
            {
                throw std::runtime_error("Allocation error");
            }
            _data = tmp;
        }
    }

    T &operator[](std::size_t index) const
    {
        if (_data == nullptr || index >= _size)
        {
            throw std::logic_error("Invalid index");
        }
        return _data[index];
    }

    std::size_t size() const noexcept
    {
        return _size;
    }

    T *&data() noexcept
    {
        return _data;
    }

private:
    T *_data;
    std::size_t _size;
    std::size_t _capacity;
};

void counting_sort(lab::vector<lab::pair> &pairs) {
    int count[lab::MAX_KEY] = {0};

    for (int i = 0; i < pairs.size(); ++i) {
        count[pairs[i].key]++;
    }

    for (int i = 1; i < lab::MAX_KEY; ++i) {
        count[i] += count[i - 1];
    }

    auto sorted = static_cast<lab::pair *>(malloc(pairs.size() * sizeof(lab::pair)));
    if (sorted == nullptr)
    {
        throw std::runtime_error("Allocation error");
    }

    for (int i = pairs.size() - 1; i >= 0; --i) {
        int key = pairs[i].key;
        sorted[count[key] - 1] = pairs[i];
        count[key]--;
    }

    free(pairs.data());
    pairs.data() = sorted;
}

int main() {
    lab::vector<lab::pair> pairs;
    char line[256];

    while (fgets(line, sizeof(line), stdin)) {
        if (strlen(line) == 0 || line[0] == '\n') continue;

        char* tab_pos = strchr(line, '\t');
        if (tab_pos != nullptr) {
            *tab_pos = '\0';

            int key = atoi(line);

            char* value = tab_pos + 1;
            std::size_t value_len = strlen(value);

            if (value[value_len - 1] == '\n') {
                value[value_len - 1] = '\0';
                value_len--;
            }

            lab::pair pair;
            pair.key = key;

            strncpy(pair.value, value, lab::MAX_STRING_LENGTH);
            pair.value[lab::MAX_STRING_LENGTH] = '\0';
            for (int i = value_len; i < lab::MAX_STRING_LENGTH; ++i) {
                pair.value[i] = '\0';
            }

            pairs.push_back(pair);
        }
    }

    counting_sort(pairs);
    for (std::size_t i = 0; i < pairs.size(); i++)
    {
        printf("%d\t%s\n", pairs[i].key, pairs[i].value);
    }

    return 0;
}