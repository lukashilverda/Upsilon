// Should work for the N0120, config is not different from the N0100/N0110.


#include <drivers/reset.h>

namespace Ion {
namespace Device {
namespace Reset {

void coreWhilePlugged() {
  core();
}

}
}
}
