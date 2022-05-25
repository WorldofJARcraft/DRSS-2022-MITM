//
// Created by erica on 25/05/2022.
//

#ifndef DRSS_MITM_SCI_LS_WRAPPER_H
#define DRSS_MITM_SCI_LS_WRAPPER_H
#include <memory>
#include <rasta_new.h>
#include <scils.h>

namespace rasta::sci_ls {

        class sci_ls_wrapper {
        public:
            static sci_ls_wrapper& getInstance(struct rasta_handle *handle, const char *scils_id);
            static sci_ls_wrapper& getInstance();
            ~sci_ls_wrapper();
            scils_signal_aspect getSignalAspect();
            void setSignalAspect(scils_signal_aspect aspect);
        private:
            sci_ls_wrapper(struct rasta_handle *handle, const char *scils_id);
            scils_signal_aspect signal_aspect{};
        };

    } // sci_ls

#endif //DRSS_MITM_SCI_LS_WRAPPER_H
