#include <iostream>

#include "fsfw/serviceinterface.h"
#include "FSFWConfig.h"

using namespace std;


#if FSFW_CPP_OSTREAM_ENABLED == 1
ServiceInterfaceStream sif::debug("DEBUG", false);
ServiceInterfaceStream sif::info("INFO", false);
ServiceInterfaceStream sif::warning("WARNING", false);
ServiceInterfaceStream sif::error("ERROR", false, true, true);
#endif

int main() {
    cout << "hello world!" << endl;

}