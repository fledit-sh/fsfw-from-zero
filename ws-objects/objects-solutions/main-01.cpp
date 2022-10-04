#include "fsfw/objectmanager.h"
#include <iostream>
#include <iomanip>

using namespace std;

class MySystemObject: public SystemObject {
public:
    MySystemObject(): SystemObject(0x10101010, false) {}
    ReturnValue_t initialize() override {
        cout << "MySystemObject::initialize: Custom init" << endl;
        return returnvalue::OK;
    }
};

int main() {
    auto* mySysObj = new MySystemObject();
    cout << "Object ID: " << setfill('0') << hex << "0x" << setw(8) <<
        mySysObj->getObjectId() << endl;
    mySysObj->initialize();
    delete mySysObj;
    return 0;
}
