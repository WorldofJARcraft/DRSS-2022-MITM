//
// Created by erica on 25/05/2022.
//

#ifndef DRSS_MITM_RASTA_WRAPPER_H
#define DRSS_MITM_RASTA_WRAPPER_H

#include "rasta_new.h"
#include "sci_ls_wrapper.h"
#include <string>
#include <memory>

namespace rasta{

        class rasta_wrapper {
        public:
            rasta_wrapper(const std::string& server_ip, int server_port, const std::string& own_ip, int own_port, unsigned long server_id, unsigned long own_id, unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result)=nullptr);
            ~rasta_wrapper();
            rasta::sci_ls::sci_ls_wrapper& register_scils_wrapper(const std::string& scils_id);
        private:
            struct rasta_handle signal_handle{};
            void setup_rasta_handle_connection(const std::string& server_ip, int server_port, const std::string& own_ip, int own_port, unsigned long server_id, unsigned long own_id, unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result));
        };

} // rasta

#endif //DRSS_MITM_RASTA_WRAPPER_H
