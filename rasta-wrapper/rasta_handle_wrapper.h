//
// Created by erica on 26/05/2022.
//

#ifndef DRSS_MITM_RASTA_HANDLE_WRAPPER_H
#define DRSS_MITM_RASTA_HANDLE_WRAPPER_H
#include <rasta_new.h>
#include <scils.h>
namespace rasta{
    class rasta_wrapper;
    namespace sci_ls{
        class sci_ls_wrapper;
    }
    struct rasta_handle_wrapper{
        struct rasta_handle handle;
        scils_t *sci_ls_handle;
        rasta_wrapper *rasta_wrapper;
        sci_ls::sci_ls_wrapper *sci_ls_wrapper;
    };
}
#endif //DRSS_MITM_RASTA_HANDLE_WRAPPER_H
