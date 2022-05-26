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
            /**
             * Constructor for an active version of the wrapper (i.e. signal) - initializes and sends connection request
             * @param server_ip other party IP, as string
             * @param server_port other party port
             * @param own_ip own IP, as string
             * @param own_port on port
             * @param server_id RaSTA ID of the other party
             * @param own_id own RaSTA ID
             * @param rasta_network RaSTA network identifiacation
             * @param onReceive optional method called when packets received
             */
            rasta_wrapper(const std::string& server_ip, int server_port, const std::string& own_ip, int own_port, unsigned long server_id, unsigned long own_id, unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result)=nullptr);
            /**
             * Constructor for a passive version of the wrapper that waits for connection requests
             * @param own_ip own IP, as string
             * @param own_port on port
             * @param own_id own RaSTA ID
             * @param rasta_network RaSTA network identifiacation
             * @param onReceive optional method called when packets received
             */
            rasta_wrapper(const std::string& own_ip, int own_port, unsigned long own_id, unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result)=nullptr);
            ~rasta_wrapper();
            std::shared_ptr<sci_ls::sci_ls_wrapper> register_scils_wrapper(const std::string& scils_id, sci_ls::sci_ls_wrapper_operation_mode mode, char *other_party_scils_name=nullptr, int other_party_rasta_id=0);
        private:
            struct rasta_handle signal_handle{};
            void setup_rasta_handle_active_connection(const std::string& server_ip, int server_port, const std::string& own_ip, int own_port, unsigned long server_id, unsigned long own_id, unsigned long rasta_network, void (*onReceive)(struct rasta_notification_result *result));

            void setup_rasta_handle_passive_connection(const std::string &basicString, int i, unsigned long i1,
                                                       unsigned long i2,
                                                       void (*pFunction)(rasta_notification_result *));
        };

} // rasta

#endif //DRSS_MITM_RASTA_WRAPPER_H
