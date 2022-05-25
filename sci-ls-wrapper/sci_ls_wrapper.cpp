//
// Created by erica on 25/05/2022.
//

#include "sci_ls_wrapper.h"

#include <iostream>
#include <hexdump.h>

#define error_abort(message) fprintf(stderr,"Error: %s on %s line %d\n",message,__FILE__,__LINE__); abort()


namespace rasta::sci_ls {
    scils_t *scils_handle;

    static void scils_on_receive(struct rasta_notification_result *result) {
        if (!scils_handle) {
            error_abort("SCI-LS was not initialized!");
        }
        rastaApplicationMessage message = sr_get_received_data(result->handle, &result->connection);

        std::cout << "Application message: \n";
        DumpHex(message.appMessage.bytes, message.appMessage.length);

        scils_on_rasta_receive(scils_handle, message);
    }

    static void send_current_aspect(scils_t *ls, char *sender) {
        sci_return_code code = scils_send_signal_aspect_status(ls, sender,
                                                               sci_ls_wrapper::getInstance().getSignalAspect());
        if (code != SUCCESS) {
            error_abort("Returning the aspect status failed!");
        } else {
            std::cout << "Returned the aspect status successfully!" << std::endl;
        }
    }

    static void onShowSignalAspect(scils_t *ls, char *sender, scils_signal_aspect signal_aspect) {
        std::cout << "Received show signal aspect from sender " << sci_get_name_string(sender) << "\n";
        sci_ls_wrapper::getInstance().setSignalAspect(signal_aspect);
        std::cout << "Returning the requested aspect status " << signal_aspect.main;
        send_current_aspect(ls, sender);
    }

    static void send_status_transmission_begin(scils_t *ls, char *receiver) {
        sci_return_code code = scils_send_status_begin(ls, receiver);
        if (code != SUCCESS) {
            error_abort("Sending the status transmission start failed!");
        } else {
            std::cout << "Sent the status transmission start message successfully" << std::endl;
        }
    }

    static void onCompareVersionRequestReceived(scils_t *ls, char *sender, unsigned char version) {
        std::cout << "Received Version comparison request from sender " << sci_get_name_string(sender) << "for version "
                  << std::hex << version;
        std::cout << "\n";

        // adapted to the format from wireshark
        // according to the specification, the checksum should be one byte with 0xff, but the lab system seems to deviate
        sci_return_code code = scils_send_version_response(ls, sender, 0, SCI_VERSION_CHECK_RESULT_VERSIONS_ARE_EQUAL,
                                                           0,
                                                           reinterpret_cast<unsigned char *>(sender));
        if (code != SUCCESS) {
            error_abort("Returning the version response failed!");
        } else {
            std::cout << "Returned the version response successfully!" << std::endl;
        }
        // after versions match, the interlocking expects our status
        send_status_transmission_begin(ls, sender);
        send_current_aspect(ls,sender);
    }

    sci_ls_wrapper::sci_ls_wrapper(struct rasta_handle *handle, const char *scils_id) {
        scils_handle = scils_init(handle, (char *) scils_id);
        if (!scils_handle) {
            error_abort("scils_init failed");
        }
        scils_handle->notifications.on_show_signal_aspect_received = onShowSignalAspect;
        scils_handle->notifications.on_version_request_received = onCompareVersionRequestReceived;
        handle->notifications.on_receive = scils_on_receive;

        // RED
        signal_aspect.main = SCILS_MAIN_HP_0;
        // the following ones adapted to trace as best as possible
        signal_aspect.additional = SCILS_ADDITIONAL_OFF;
        signal_aspect.zs3 = SCILS_ZS3_OFF;
        signal_aspect.zs3v = SCILS_ZS3_OFF;
        signal_aspect.zs2 = SCILS_ZS2_LETTER_A;
        signal_aspect.zs2v = SCILS_ZS2_OFF;
        signal_aspect.deprecation_information = SCILS_DEPRECIATION_INFORMATION_NO_INFORMATION;
        signal_aspect.upstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION;
        signal_aspect.downstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION;
        signal_aspect.dark_switching = SCILS_DARK_SWITCHING_SHOW;

    }

    sci_ls_wrapper::~sci_ls_wrapper() {
        scils_cleanup(scils_handle);
    }

    sci_ls_wrapper &sci_ls_wrapper::getInstance(struct rasta_handle *handle, const char *scils_id) {
        static sci_ls_wrapper instance(handle, scils_id);
        return instance;
    }

    scils_signal_aspect sci_ls_wrapper::getSignalAspect() {
        return this->signal_aspect;
    }

    void sci_ls_wrapper::setSignalAspect(scils_signal_aspect aspect) {
        this->signal_aspect = aspect;
    }

    sci_ls_wrapper &sci_ls_wrapper::getInstance() {
        return sci_ls_wrapper::getInstance(nullptr, nullptr);
    }
} // sci_ls