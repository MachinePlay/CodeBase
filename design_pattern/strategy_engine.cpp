#include <iostream>
#include <string>
#include <exception>
#include <unordered_map>

/* thing type */
enum DeviceType {
    MON = 0,
    TUE = 1,
    WES = 2,
    THU = 3,
};


/**
 * @class BaseHandler
 * implement this Handler to do your job
 */
class BaseHandler {
public:
    /* ctor. */
    BaseHandler(){};

    /* handler execute implement. */
    virtual bool execute() {
        std::cout << "default" << std::endl;
        return true;
    }
    
    /* dtor. */
    virtual ~BaseHandler(){};
private:
    /* none-copy ctor*/
    BaseHandler(const BaseHandler &rhs) = delete;

    /* none-assign ctor. */
    BaseHandler& operator=(const BaseHandler &rhs) = delete;

    std::unordered_map<int64_t, std::string> event_day;
};

/*
 * Monday Handler
 */
class MondayHandler : public BaseHandler {
    /* DoMondayThing */
    virtual bool execute() {
        dothing1();
        dothing2();
        dothing3();
        return true;
    }
    void dothing1() { std::cout << "1" << std::endl; };
    void dothing2() { std::cout << "2" << std::endl; };
    void dothing3() { std::cout << "3" << std::endl; };
};

/*
 * Tuesday Handler
 */
class TuesdayHandler : public BaseHandler {
    /* DoTuesdayThing */
    virtual bool execute() {
        dothing1();
        dothing2();
        dothing3();
        return true;
    }
    void dothing1() { std::cout << "1" << std::endl; };
    void dothing2() { std::cout << "2" << std::endl; };
    void dothing3() { std::cout << "3" << std::endl; };
};

/*
 * Wednesday Handler
 */
class WednesdayHandler : public BaseHandler {
    /* DoWednesdayThing */
    virtual bool execute() {
        dothing1();
        dothing2();
        dothing3();
        return true;
    }
    void dothing1() { std::cout << "1" << std::endl; };
    void dothing2() { std::cout << "2" << std::endl; };
    void dothing3() { std::cout << "3" << std::endl; };
};

/*
 * Thursday Handler
 */
class ThursdayHandler : public BaseHandler {
    /* TuesdayThing */
    virtual bool execute() {
        dothing1();
        dothing2();
        dothing3();
        return true;
    }
    void dothing1() { std::cout << "1" << std::endl; };
    void dothing2() { std::cout << "2" << std::endl; };
    void dothing3() { std::cout << "3" << std::endl; };
};

/*
 * @class HandlerFactory
 * implement this Handler to do your job
 */
class HandlerFactory {
public:
BaseHandler* getHandler(const DeviceType & e) {
        switch(e) {
            case MON: return  new MondayHandler();
            case TUE: return  new TuesdayHandler();
            case WES: return  new WednesdayHandler();
            case THU: return  new ThursdayHandler();
            default: return  new BaseHandler();
        }
    }
};

class ExeEngine {
public:
    void do_exe(BaseHandler* handler) {
        handler->execute();
        delete handler;
    }
};






int main() {
    ExeEngine engine;
    HandlerFactory factory;
    BaseHandler* handler = nullptr;
    for (int i = 0; i < 5; ++i) {
        handler = factory.getHandler(static_cast<DeviceType>(i));
        if(handler) {
            engine.do_exe(handler);
        }
    }
    return 0;
}


