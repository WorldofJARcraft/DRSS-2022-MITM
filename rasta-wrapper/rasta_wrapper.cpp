//
// Created by erica on 25/05/2022.
//

#include "rasta_wrapper.h"
#include <string>
#include <iostream>

#include <cstdio>

#define error_abort(message) fprintf(stderr,"Error: %s on %s line %d\n",message,__FILE__,__LINE__); abort()


namespace rasta {

    // borrowed from rasta_wrapper.cfg in rasta-protocol
    const int rasta_diag_window = 5000, rasta_max_packet = 3;
    const unsigned int md4_a = 0x67452301, md4_b=0xefcdab89, md4_c=0x98badcfe, md4_d=0x10325476;
    const int rasta_mwa = 10, rasta_send_max=10, rasta_t_h = 200, rasta_t_max=1700;


    void rasta_wrapper::setup_rasta_handle_connection(const std::string& server_ip, const int server_port, const std::string& own_ip, const int own_port, const unsigned long server_id, const unsigned long own_id, const unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result)){
        struct RastaIPData server_ip_config{};

        struct RastaIPData own_ip_config{};

        struct RastaConfigInfo config{};
        struct DictionaryArray array = allocate_DictionaryArray(10);
        struct logger_t logger = logger_init(LOG_LEVEL_DEBUG,LOGGER_TYPE_CONSOLE);

        if(server_ip.length() > sizeof(server_ip_config.ip)){
            error_abort("RaSTA Server IP is too long!");
        }

        if(own_ip.length() > sizeof(own_ip_config.ip)){
            error_abort("RaSTA Server IP is too long!");
        }

        memcpy(server_ip_config.ip, server_ip.data(), server_ip.length());
        server_ip_config.port = server_port;

        memcpy(own_ip_config.ip, own_ip.data(), own_ip.length());
        own_ip_config.port = own_port;

        config.general.rasta_id = own_id;
        config.general.rasta_network = rasta_network;

        // these are OUR interfaces
        config.redundancy.connections.count = 1;
        config.redundancy.connections.data=&own_ip_config;
        config.redundancy.crc_type = crc_init_opt_b();

        config.redundancy.n_deferqueue_size = 10;
        config.redundancy.n_diagnose = 0;
        config.redundancy.t_seq = 0;
        // no CRC
        config.redundancy.crc_type = crc_init_opt_a();

        config.sending.diag_window = rasta_diag_window;
        config.sending.max_packet = rasta_max_packet;
        config.sending.md4_a = md4_a;
        config.sending.md4_b = md4_b;
        config.sending.md4_c = md4_c;
        config.sending.md4_d = md4_d;
        // half checksum
        config.sending.md4_type = RASTA_CHECKSUM_8B;
        config.sending.mwa = rasta_mwa;
        config.sending.send_max = rasta_send_max;
        config.sending.t_h=rasta_t_h;
        config.sending.t_max = rasta_t_max;
        config.sending.sr_hash_algorithm = RASTA_ALGO_MD4;
        // not evaluated when MD4 is used
        config.sending.sr_hash_key = 0;

        sr_init_handle_manually(&signal_handle, config, array, logger);

        // register onReceive hook if defined
        signal_handle.notifications.on_receive = onReceive;

        sr_connect(&signal_handle, server_id, &server_ip_config);
    }

    rasta_wrapper::rasta_wrapper(const std::string& server_ip, const int server_port, const std::string& own_ip, const int own_port, const unsigned long server_id, const unsigned long own_id, const unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result)) {
        this->setup_rasta_handle_connection(server_ip,server_port,own_ip,own_port,server_id,own_id,rasta_network,onReceive);
    }

    rasta_wrapper::~rasta_wrapper() {
        for(int i = 0; i < rastalist_count(&this->signal_handle.connections); i++) {
            struct rasta_connection *connection = rastalist_getConnection(&this->signal_handle.connections,i);

            std::cout << "Disconnection from connection " << (i+1) << " with remote " << connection->remote_id << std::endl;
            sr_disconnect(&this->signal_handle, connection->remote_id);
        }
        sr_cleanup(&this->signal_handle);
    }

} // rasta