#include <iostream>
#include "fsfw/objectmanager.h"

#include <iomanip>

using namespace std;

class MySystemObject: public SystemObject {
public:
    MySystemObject(): SystemObject(0x10101010, false) {}
    ReturnValue_t initialize() override {
        cout << "MySystemObject::initialize: Custom init" << endl;
    }
};

int main() {
    auto* mySysObj = new MySystemObject();
    cout << "Object ID: " << setfill('0') << setw(8) << hex << "0x" << 
        mySysObj->getObjectId() << endl;
    mySysObj->initialize();
    delete mySysObj;
}
