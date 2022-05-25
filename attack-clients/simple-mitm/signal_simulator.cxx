#include <string>
#include <iostream>
#include "rasta_wrapper.h"

#include "hexdump.h"

static const long TESTBED_RASTA_INTERLOCKING_ID=97;
static const long TESTBED_RASTA_SIGNAL_ID=98;
static const long TESTBED_RASTA_INTERLOCKING_NETWORK=0x43;

const std::string INTERLOCKING_IP = "10.43.17.207";
static const int TESTBED_RASTA_INTERLOCKING_PORT=9998;

const std::string OWN_IP = "10.42.2.38";
static const int OWN_PORT=8888;


static const int TESTBED_RASTA_INTERLOCKING_RaSTA_ID=97;

static void onReceiveInterlocking(struct rasta_notification_result *result){
    rastaApplicationMessage p;
    std::cout << "Received a message from interlocking!\n";

    p = sr_get_received_data(result->handle,&result->connection);


    std::cout << "Message has sender " << p.id << " and " << p.appMessage.length << " bytes of payload!\nMessage: \n";
    DumpHex(p.appMessage.bytes,p.appMessage.length);
}

int main() {
    char input;
    const rasta::rasta_wrapper interlocking_wrapper(INTERLOCKING_IP,TESTBED_RASTA_INTERLOCKING_PORT,OWN_IP,OWN_PORT,TESTBED_RASTA_INTERLOCKING_ID,TESTBED_RASTA_SIGNAL_ID,0,onReceiveInterlocking);
    std::cout << "Connection established, waiting for messages!\n";
    std::cout << "Press any key to terminate!" << std::endl;
    std::cin >> input;
    return 0;
}
