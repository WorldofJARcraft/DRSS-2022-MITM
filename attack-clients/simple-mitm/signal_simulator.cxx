#include <string>
#include <iostream>
#include <unistd.h>
#include "rasta_wrapper.h"

#include "hexdump.h"

static const long TESTBED_RASTA_INTERLOCKING_ID=97;
static const long TESTBED_RASTA_SIGNAL_ID=98;

const std::string INTERLOCKING_IP = "10.43.17.207";
static const int TESTBED_RASTA_INTERLOCKING_PORT=9998;

const std::string OWN_IP = "10.42.2.38";
static const int OWN_PORT=8888;

const std::string OWN_SCI_ID="SIGNAL";


int main() {
    char input;
    // active wrapper, opens a connection to the interlocking
    rasta::rasta_wrapper interlocking_wrapper(INTERLOCKING_IP, TESTBED_RASTA_INTERLOCKING_PORT, OWN_IP, OWN_PORT, TESTBED_RASTA_INTERLOCKING_ID, TESTBED_RASTA_SIGNAL_ID, 0);
    // version of the SCI-LS wrapper that simulates a signal
    std::shared_ptr<rasta::sci_ls::sci_ls_wrapper> scils_wrapper = interlocking_wrapper.register_scils_wrapper(OWN_SCI_ID,rasta::sci_ls::SCI_LS_WRAPPER_MODE_SIGNAL);
    sleep(10);
    if(!interlocking_wrapper.is_connected()){
        std::cerr << "ERROR: Interlocking did not connect!" << std::endl;
        return 1;
    }
    std::cout << "Connection established, waiting for messages!\n";
    std::cout << "Press any key to terminate!" << std::endl;
    std::cin >> input;
    return 0;
}
