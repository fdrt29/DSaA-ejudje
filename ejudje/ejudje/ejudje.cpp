#include <string>

int main(int argc, char* argv[])
{
    // set_size N
    // push X, pop или print
    return 0;
}

class Stack
{
public:
    Stack(size_t size): raw_array(new std::string[size]())
    {
    }

    

private:
    std::string* raw_array;
};
