```
#include <string>
#include <iostream>
class Graph {
public:
    void set_type();
};

class VariantType {
public:
    // if we need a constexpr object, we can user constexpr var
    // e.g. constexpr VariantType(std::string &type) : _type_name(type) {};

    /* default ctor(syncthesized default constructor). */
    VariantType() = default;
    /* normal ctor. */
    // explicit only affect 1 parameter ctor. ctor with mutiple parameter unable to do type converion.
    // exlicit ctot cant't appear outside the class.
    explicit VariantType(std::string &type) : _type_name(type) {};
    /* delegating ctor. */
    explicit VariantType(std::string &type) : VariantType(type) {};

    /* setter. */
    void set_type(::std::string type_name) {
        this->_type_name = type_name;
    }

    /* getter. const member function. */
    ::std::string get_type() const {
        return this->_type_name;
    }

    // change mutable member 
    // mutable member can be change even if in the const function
    void set_ext_info(::std::string ext_info) const{
        this->ext_info = ext_info;
    }

    // friend function can read the private member of the target class
    // declare in the class only to indicate the privalege to access to private member
    // must declare out of the class (some compiler don't need)
    friend  ::std::istream &read(::std::istream &is, VariantType &type);
    friend  ::std::ostream &print(::std::ostream &os, VariantType &type);

    // friend class 
    // can modify all the member, but can't transiable.
    friend class Graph;
    friend Graph::set_type();

    //mutable extra info about the class, can be changed at any time.
    // even pass by value/reference by an const member function
    mutable std::string ext_info;

    //static only appear once
    /* static count of num. */
    static int get_cnt();

    // can be initilized inside class by const/constexpr e.g. staic int cnt = 100;
    static int          cnt; 

private:
    /* type name. */
    ::std::string _type_name;

    /* type emum. */
    int           _number{0}; //initilized 
};

// implement outside the class.
int VariantType::get_cnt() {
    return cnt;
}

int VariantType::cnt  = 0;

//out of class member class
/* istream. */
::std::istream &read(::std::istream &is, VariantType &lhs) {
    is  >> lhs.get_type() >> 100;
    return is;
}





int main(int argc, char* argvp[]) {
    VariantType my_type;
    my_type.set_type("hello");
    std::cout << my_type.get_type() << std::endl;
    return 0;
}
```