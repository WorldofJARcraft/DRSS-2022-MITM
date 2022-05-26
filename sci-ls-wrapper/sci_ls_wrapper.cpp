//
// Created by erica on 25/05/2022.
//

#include "sci_ls_wrapper.h"
#include <chrono>
#include <iostream>
#include <hexdump.h>
#include <unistd.h>
#include <future>

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
                                                               sci_ls_wrapper::getInstance()->getSignalAspect());
        if (code != SUCCESS) {
            error_abort("Returning the aspect status failed!");
        } else {
            std::cout << "Returned the aspect status successfully!" << std::endl;
        }
    }

    static void send_status_transmission_begin(scils_t *ls, char *receiver) {
        sci_return_code code = scils_send_status_begin(ls, receiver);
        if (code != SUCCESS) {
            error_abort("Sending the status transmission start failed!");
        } else {
            std::cout << "Sent the status transmission start message successfully" << std::endl;
        }
    }
    // the "Initialization" telegrams are called "Status" telegrams in this implementation
    // the interlocking sends the initialization request, we confirm that we started and finished our initialization
    static void onStatusRequestReceived(scils_t *ls, char *sender){
        sci_return_code code;
        std::cout << "Status request received from " << sender << std::endl;
        send_status_transmission_begin(ls, sender);

        code = scils_send_status_finish(ls,sender);
        if (code != SUCCESS) {
            error_abort("Sending the initialization complete message failed!");
        } else {
            std::cout << "Returned the initialization completed message successfully!" << std::endl;
        }
    }

    static const char * aspect_to_name(const scils_main signal_aspect){
        switch (signal_aspect) {
            case SCILS_MAIN_HP_0:
                return "red";
            case SCILS_MAIN_KS_2_WITH_ADDITIONAL_LIGHT:
                return "yellow";
            case SCILS_MAIN_KS_1:
                return "green";
            default:
                return "unknown";
        }
    }

    static void onShowSignalAspect(scils_t *ls, char *sender, scils_signal_aspect signal_aspect) {
        std::cout << "Received show signal aspect from sender " << sci_get_name_string(sender) << "\n";
        sci_ls_wrapper::getInstance()->setSignalAspect(signal_aspect);
        std::cout << "Returning the requested aspect status " << signal_aspect.main << "( " << aspect_to_name(signal_aspect.main) << ")" << std::endl;
        send_current_aspect(ls, sender);
    }

    static void send_signal_aspect(scils_t *ls, char *sender, scils_signal_aspect &signal_aspect);

    static void onSignalAspectStatusReceived(scils_t *ls, char *sender, scils_signal_aspect signal_aspect) {
        std::cout << "Received show signal aspect from sender " << sci_get_name_string(sender) << "\n";

        signal_aspect.main = SCILS_MAIN_KS_1;
        sci_ls_wrapper::getInstance()->setSignalAspect(signal_aspect);

        // do not overwhelm the signal with requests
        std::async(std::launch::async, [&ls, &sender, &signal_aspect] () {
            // Use sleep_for to wait specified time (or sleep_until).
            std::this_thread::sleep_for( std::chrono::milliseconds {SHOW_ASPECT_COMMAND_INTERVAL_MS});
            // Do whatever you want.
            send_signal_aspect(ls, sender, signal_aspect);
        } );

    }

    static void send_signal_aspect(scils_t *ls, char *sender, scils_signal_aspect &signal_aspect) {
        std::cout << "Setting the signal status to " << signal_aspect.main << "( " << aspect_to_name(signal_aspect.main) << ")" << std::endl;
        sci_return_code code = scils_send_show_signal_aspect(ls,sender,signal_aspect);
        if (code != SUCCESS) {
            error_abort("Sending show Signal aspect failed!");
        } else {
            std::cout << "Sent show signal aspect!" << std::endl;
        }
    }

    static void sendCompareVersionRequest(scils_t *ls, char *receiver){
        sci_return_code code = scils_send_version_request(ls,receiver,SCI_VERSION);
        if (code != SUCCESS) {
            error_abort("Sending compare version request failed!");
        } else {
            std::cout << "Sent compare version request!" << std::endl;
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
    }

    static void onCompareVersionResponseReceived(scils_t *ls, char *sender, unsigned char  version, sci_version_check_result result, unsigned char checksum_length,
                                                 unsigned char * checksum) {
        std::cout << "Received Version comparison response from sender " << sci_get_name_string(sender) << "for version "
                  << std::hex << version;
        std::cout << " and result: " << result << "\n";

        // signal expects us to send the initialization request (in this implementation the status request) now
        sci_return_code code = scils_send_status_request(ls, sender);
        if (code != SUCCESS) {
            error_abort("Sending the initialization request failed!");
        } else {
            std::cout << "Sending the initialization request succeeded!" << std::endl;
        }
    }

    static void onStatusFinishedReceived(scils_t *ls, char *sender){
        std::cout << "Status finished received from " << sender << std::endl;
        scils_signal_aspect  aspect = sci_ls_wrapper::getInstance()->getSignalAspect();
        send_signal_aspect(ls,sender,aspect);
    }

    sci_ls_wrapper::sci_ls_wrapper(struct rasta_handle *handle, const char *scils_id, const sci_ls_wrapper_operation_mode mode) {
        if(mode != SCI_LS_WRAPPER_MODE_SIGNAL){
            error_abort("Use the other constructor for signals!");
        }
        scils_handle = scils_init(handle, (char *) scils_id);
        if (!scils_handle) {
            error_abort("scils_init failed");
        }
        // functions required to simulate a signal
        scils_handle->notifications.on_show_signal_aspect_received = onShowSignalAspect;
        scils_handle->notifications.on_version_request_received = onCompareVersionRequestReceived;
        scils_handle->notifications.on_status_request_received = onStatusRequestReceived;


        // glue that binds RaSTA and SCI-LS together
        handle->notifications.on_receive = scils_on_receive;

        // red
        signal_aspect.main = SCILS_MAIN_HP_0;
        // the following ones adapted to trace as best as possible, will be overwritten by first status / request
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

    static std::shared_ptr<sci_ls_wrapper> instance = nullptr;

    std::shared_ptr<sci_ls_wrapper> sci_ls_wrapper::getInstance(struct rasta_handle *handle, const char *scils_id, const sci_ls_wrapper_operation_mode mode) {
        if(instance){
            return instance;
        }
        instance = std::make_shared<sci_ls_wrapper>(handle, scils_id, mode);
        return instance;
    }

    std::shared_ptr<sci_ls_wrapper> sci_ls_wrapper::getInstance(struct rasta_handle *handle, const char *scils_id, const sci_ls_wrapper_operation_mode mode, char *other_party_name, int other_party_id) {
        if(instance){
            return instance;
        }
        instance = std::make_shared<sci_ls_wrapper>(handle, scils_id, mode,other_party_name,other_party_id);
        return instance;
    }

    scils_signal_aspect sci_ls_wrapper::getSignalAspect() {
        return this->signal_aspect;
    }

    void sci_ls_wrapper::setSignalAspect(scils_signal_aspect aspect) {
        this->signal_aspect = aspect;
    }

    std::shared_ptr<sci_ls_wrapper> sci_ls_wrapper::getInstance() {
        return instance;
    }

    sci_ls_wrapper::sci_ls_wrapper(struct rasta_handle *handle, const char *scils_id,
                                   sci_ls_wrapper_operation_mode mode, char *other_party_scils_name, const int other_party_rasta_id) {
        if(mode != SCI_LS_WRAPPER_MODE_INTERLOCKING){
            error_abort("Use the other constructor for signals!");
        }
        scils_handle = scils_init(handle, (char *) scils_id);
        if (!scils_handle) {
            error_abort("scils_init failed");
        }
        // functions required to simulate an interlocking
        scils_handle->notifications.on_signal_aspect_status_received = onSignalAspectStatusReceived;
        scils_handle->notifications.on_version_response_received = onCompareVersionResponseReceived;
        scils_handle->notifications.on_status_finish_received = onStatusFinishedReceived;


        // glue that binds RaSTA and SCI-LS together
        handle->notifications.on_receive = scils_on_receive;

        // green
        signal_aspect.main = SCILS_MAIN_KS_1;
        // the following ones adapted to trace as best as possible, will be overwritten by first status / request
        signal_aspect.additional = SCILS_ADDITIONAL_OFF;
        signal_aspect.zs3 = SCILS_ZS3_OFF;
        signal_aspect.zs3v = SCILS_ZS3_OFF;
        signal_aspect.zs2 = SCILS_ZS2_LETTER_A;
        signal_aspect.zs2v = SCILS_ZS2_OFF;
        signal_aspect.deprecation_information = SCILS_DEPRECIATION_INFORMATION_NO_INFORMATION;
        signal_aspect.upstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION;
        signal_aspect.downstream_driveway_information = SCILS_DRIVE_WAY_INFORMATION_NO_INFORMATION;
        signal_aspect.dark_switching = SCILS_DARK_SWITCHING_SHOW;

        // so scils can resolve the other party
        scils_register_sci_name(scils_handle,other_party_scils_name,other_party_rasta_id);

        sendCompareVersionRequest(scils_handle,other_party_scils_name);

    }
} // sci_ls