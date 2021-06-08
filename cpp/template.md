
# 类模版不能有成员模版虚函数
```
// Type your code here, or load an example.
#include <stdio.h>
template <typename SerilizeType>
class A {
    public:
    inline void display(const SerilizeType &t) {
        printf("Hello world!", t);
    }
    
    //ok
    template <typename DeserilizeType>
    void change_type(const SerilizeType &t, const DeserilizeType& d) {
        d = dynamic_cast<DeserilizeType>(t);
    }

    //compile error
    template <typename DeserilizeType>
    virtual void convert(const SerilizeType &t, const DeserilizeType& d) {
            d = dynamic_cast<DeserilizeType>(t);
    }
};

int main() {
    return 0;
}
```