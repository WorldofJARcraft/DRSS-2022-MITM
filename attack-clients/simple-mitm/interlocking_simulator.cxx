#include <string>
#include <iostream>
#include <unistd.h>
#include "rasta_wrapper.h"

#include "hexdump.h"

static const long TESTBED_RASTA_INTERLOCKING_ID=97;
static const long TESTBED_RASTA_SIGNAL_ID=98;

const std::string SIGNAL_IP = "10.42.0.109";
static const int TESTBED_RASTA_SIGNAL_PORT=8888;

const std::string OWN_IP = "10.42.2.38";
static const int OWN_PORT=9998;


const std::string OWN_SCI_ID="INTERLOCKING";
const std::string SIGNAL_SCI_ID="SIGNAL";

int main() {
    char input;
    rasta::rasta_wrapper interlocking_wrapper(OWN_IP, OWN_PORT, TESTBED_RASTA_INTERLOCKING_ID, 0);
    sleep(10);
    std::shared_ptr<rasta::sci_ls::sci_ls_wrapper> scils_wrapper = interlocking_wrapper.register_scils_wrapper(OWN_SCI_ID,rasta::sci_ls::SCI_LS_WRAPPER_MODE_INTERLOCKING,(char *) SIGNAL_SCI_ID.data(),TESTBED_RASTA_SIGNAL_ID);

    std::cout << "Connection established, waiting for messages!\n";
    std::cout << "Press any key to terminate!" << std::endl;
    std::cin >> input;
    return 0;
}
