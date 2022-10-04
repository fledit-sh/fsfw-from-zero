#include "fsfw/objectmanager.h"
#include <iostream>
#include <iomanip>

using namespace std;

class MySystemObject: public SystemObject {
public:
    MySystemObject(): SystemObject(0x10101010) {}
    ReturnValue_t initialize() override {
        cout << "MySystemObject::initialize: Custom init" << endl;
    }
};

int main() {
    new MySystemObject();
    auto* objManager = ObjectManager::instance();
    objManager->initialize();
    auto* mySysObj = objManager->get<MySystemObject>(0x10101010);
    cout << "Object ID: " << setfill('0') << hex << "0x" << setw(8) <<
        mySysObj->getObjectId() << endl;
    return 0;
}
