//
// Created by erica on 25/05/2022.
//

#ifndef DRSS_MITM_SCI_LS_WRAPPER_H
#define DRSS_MITM_SCI_LS_WRAPPER_H
#include <memory>
#include <rasta_new.h>
#include <scils.h>

namespace rasta::sci_ls {
        typedef enum{
            SCI_LS_WRAPPER_MODE_SIGNAL,
            SCI_LS_WRAPPER_MODE_INTERLOCKING
        } sci_ls_wrapper_operation_mode;
        class sci_ls_wrapper {
        public:
            static std::shared_ptr<sci_ls_wrapper> getInstance(struct rasta_handle *handle, const char *scils_id, sci_ls_wrapper_operation_mode mode);
            static std::shared_ptr<sci_ls_wrapper> getInstance(rasta_handle *handle, const char *scils_id, sci_ls_wrapper_operation_mode mode,
            char *other_party_name, int other_party_id);
            static std::shared_ptr<sci_ls_wrapper> getInstance();

            sci_ls_wrapper(struct rasta_handle *handle, const char *scils_id,sci_ls_wrapper_operation_mode mode);
            sci_ls_wrapper(struct rasta_handle *handle, const char *scils_id,sci_ls_wrapper_operation_mode mode, char *other_party_scils_name, int other_party_rasta_id);
            ~sci_ls_wrapper();
            scils_signal_aspect getSignalAspect();
            void setSignalAspect(scils_signal_aspect aspect);
        private:
            scils_signal_aspect signal_aspect{};
        };

    } // sci_ls

#endif //DRSS_MITM_SCI_LS_WRAPPER_H
