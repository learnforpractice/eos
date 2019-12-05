#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace boost;
using namespace std;

namespace mytest {

using boost::signals2::signal;

struct ClassA
{
    signal<void ()>    SigA;
    signal<void (int)> SigB;
};

struct ClassB
{
    void PrintFoo()      { cout << "Foo" << endl; }
    void PrintInt(int i) { cout << "Bar: " << i << endl; }
};

}

using namespace mytest;

int main(int argc, char**argv)
{
    ClassA a;
    ClassB b, b2;
    cout<<"hello,world"<<endl;
    a.SigA.connect(bind(&ClassB::PrintFoo, &b));
    a.SigB.connect(bind(&ClassB::PrintInt, &b,  _1));
    a.SigB.connect(bind(&ClassB::PrintInt, &b2, _1));
    cout<<"before SigA"<<endl;
    a.SigA();
    cout<<"end SigA"<<endl;

    cout<<"before SigB"<<endl;
    a.SigB(4);
    cout<<"end SigB"<<endl;
    return 0;
}

/*
int init_unit_test_suite(int argc, char**argv) {
    cout<<"hello,world"<<endl;
    return main(argc, argv);
}
*/