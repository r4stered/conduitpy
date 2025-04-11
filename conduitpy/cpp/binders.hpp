#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "conduit/ds_reader.h"
#include "conduit/pdp_reader.h"
#include "conduit/system_reader.h"
#include "conduit/wpilibio.h"
#include "conduit/conduit_schema_generated.h"

#include <string>

namespace nb = nanobind;

// Helper function to convert schema::Joystick to Python dictionary
inline nb::dict joystick_to_dict(const org::littletonrobotics::conduit::schema::Joystick* joystick) {
    nb::dict result;
    
    // Convert name to string (handle null termination properly)
    std::string name_str;
    for (size_t i = 0; i < joystick->name()->size() && joystick->name()->Data()[i] != 0; ++i) {
        name_str += static_cast<char>(joystick->name()->Data()[i]);
    }
    
    result["name"] = name_str;
    result["type"] = joystick->type();
    result["is_xbox"] = joystick->is_xbox();
    
    // Convert axes
    nb::list axes;
    for (int i = 0; i < joystick->axis_count(); i++) {
        axes.append(joystick->axis_values()->Data()[i]);
    }
    result["axes"] = axes;
    
    nb::list axis_types;
    for (int i = 0; i < joystick->axis_count(); i++) {
        axis_types.append(joystick->axis_types()->Data()[i]);
    }
    result["axis_types"] = axis_types;
    
    // Convert buttons
    uint32_t buttons_raw = joystick->buttons();
    nb::list buttons;
    for (int i = 0; i < joystick->button_count(); i++) {
        buttons.append((buttons_raw & (1 << i)) != 0);
    }
    result["buttons"] = buttons;
    
    // Convert POVs
    nb::list povs;
    for (int i = 0; i < joystick->pov_count(); i++) {
        povs.append(joystick->pov_values()->Data()[i]);
    }
    result["povs"] = povs;
    
    return result;
}

// Helper function to convert schema::DSData to Python dictionary
inline nb::dict dsdata_to_dict(const org::littletonrobotics::conduit::schema::DSData* ds_data) {
    nb::dict result;
    
    result["alliance_station"] = ds_data->alliance_station();
    
    // Convert event name to string
    std::string event_name;
    for (size_t i = 0; i < ds_data->event_name()->size() && ds_data->event_name()->Data()[i] != 0; ++i) {
        event_name += static_cast<char>(ds_data->event_name()->Data()[i]);
    }
    result["event_name"] = event_name;
    
    // Convert game specific message
    std::string game_specific_message;
    for (size_t i = 0; i < ds_data->game_specific_message_size(); ++i) {
        game_specific_message += static_cast<char>(ds_data->game_specific_message()->Data()[i]);
    }
    result["game_specific_message"] = game_specific_message;
    
    result["match_number"] = ds_data->match_number();
    result["replay_number"] = ds_data->replay_number();
    result["match_type"] = ds_data->match_type();
    result["control_word"] = ds_data->control_word();
    result["match_time"] = ds_data->match_time();
    
    // Convert joysticks
    nb::list joysticks;
    for (int i = 0; i < ds_data->joysticks()->size(); i++) {
        const org::littletonrobotics::conduit::schema::Joystick* joystick = reinterpret_cast<const org::littletonrobotics::conduit::schema::Joystick*>(&ds_data->joysticks()->Data()[i]);
        joysticks.append(joystick_to_dict(joystick));
    }
    result["joysticks"] = joysticks;
    
    return result;
}

// Helper function to convert schema::PDPData to Python dictionary
inline nb::dict pdpdata_to_dict(const org::littletonrobotics::conduit::schema::PDPData* pdp_data) {
    nb::dict result;
    
    result["handle"] = pdp_data->handle();
    result["channel_count"] = pdp_data->channel_count();
    result["type"] = pdp_data->type();
    result["module_id"] = pdp_data->module_id();
    result["faults"] = pdp_data->faults();
    result["sticky_faults"] = pdp_data->sticky_faults();
    result["temperature"] = pdp_data->temperature();
    result["voltage"] = pdp_data->voltage();
    
    // Convert channel currents
    nb::list channel_currents;
    for (int i = 0; i < pdp_data->channel_current()->size(); i++) {
        channel_currents.append(pdp_data->channel_current()->Data()[i]);
    }
    result["channel_currents"] = channel_currents;
    
    result["total_current"] = pdp_data->total_current();
    result["total_power"] = pdp_data->total_power();
    result["total_energy"] = pdp_data->total_energy();
    
    return result;
}

// Helper function to convert schema::CANStatus to Python dictionary
inline nb::dict canstatus_to_dict(const org::littletonrobotics::conduit::schema::CANStatus* can_status) {
    nb::dict result;
    
    result["percent_bus_utilization"] = can_status->percent_bus_utilization();
    result["bus_off_count"] = can_status->bus_off_count();
    result["tx_full_count"] = can_status->tx_full_count();
    result["receive_error_count"] = can_status->receive_error_count();
    result["transmit_error_count"] = can_status->transmit_error_count();
    
    return result;
}

// Helper function to convert schema::SystemData to Python dictionary
inline nb::dict systemdata_to_dict(const org::littletonrobotics::conduit::schema::SystemData* sys_data) {
    nb::dict result;
    
    result["fpga_version"] = sys_data->fpga_version();
    result["fpga_revision"] = sys_data->fpga_revision();
    
    // Convert serial number to string
    std::string serial_number;
    for (size_t i = 0; i < sys_data->serial_number_size(); ++i) {
        serial_number += static_cast<char>(sys_data->serial_number()->Data()[i]);
    }
    result["serial_number"] = serial_number;
    
    // Convert comments to string
    std::string comments;
    for (size_t i = 0; i < sys_data->comments_size(); ++i) {
        comments += static_cast<char>(sys_data->comments()->Data()[i]);
    }
    result["comments"] = comments;
    
    result["team_number"] = sys_data->team_number();
    result["fpga_button"] = sys_data->fpga_button();
    result["system_active"] = sys_data->system_active();
    result["browned_out"] = sys_data->browned_out();
    result["comms_disable_count"] = sys_data->comms_disable_count();
    result["rsl_state"] = sys_data->rsl_state();
    result["system_time_valid"] = sys_data->system_time_valid();
    result["voltage_vin"] = sys_data->voltage_vin();
    result["current_vin"] = sys_data->current_vin();
    result["user_voltage_3v3"] = sys_data->user_voltage_3v3();
    result["user_current_3v3"] = sys_data->user_current_3v3();
    result["user_active_3v3"] = sys_data->user_active_3v3();
    result["user_current_faults_3v3"] = sys_data->user_current_faults_3v3();
    result["user_voltage_5v"] = sys_data->user_voltage_5v();
    result["user_current_5v"] = sys_data->user_current_5v();
    result["user_active_5v"] = sys_data->user_active_5v();
    result["user_current_faults_5v"] = sys_data->user_current_faults_5v();
    result["user_voltage_6v"] = sys_data->user_voltage_6v();
    result["user_current_6v"] = sys_data->user_current_6v();
    result["user_active_6v"] = sys_data->user_active_6v();
    result["user_current_faults_6v"] = sys_data->user_current_faults_6v();
    result["brownout_voltage"] = sys_data->brownout_voltage();
    result["cpu_temp"] = sys_data->cpu_temp();
    
    // Convert CAN status
    result["can_status"] = canstatus_to_dict(&sys_data->can_status());
    result["epoch_time"] = sys_data->epoch_time();
    
    return result;
}

// Helper function to convert schema::CoreInputs to Python dictionary
inline nb::dict coreinputs_to_dict(const org::littletonrobotics::conduit::schema::CoreInputs* core_inputs) {
    nb::dict result;
    
    result["timestamp"] = core_inputs->timestamp();
    result["ds"] = dsdata_to_dict(&core_inputs->ds());
    result["pdp"] = pdpdata_to_dict(&core_inputs->pdp());
    result["sys"] = systemdata_to_dict(&core_inputs->sys());
    
    return result;
}

// Class wrappers for the C++ classes
class PyDsReader {
public:
    PyDsReader() : reader_() {}
    
    nb::dict read() {
        org::littletonrobotics::conduit::schema::DSData data;
        reader_.read(&data);
        return dsdata_to_dict(&data);
    }
    
private:
    DsReader reader_;
};

class PyPDPReader {
public:
    PyPDPReader() : reader_() {}
    
    nb::dict read() {
        org::littletonrobotics::conduit::schema::PDPData data;
        reader_.read(&data);
        return pdpdata_to_dict(&data);
    }
    
private:
    PDPReader reader_;
};

class PySystemReader {
public:
    PySystemReader() : reader_() {}
    
    nb::dict read() {
        org::littletonrobotics::conduit::schema::SystemData data;
        reader_.read(&data);
        return systemdata_to_dict(&data);
    }
    
private:
    SystemReader reader_;
};

class PyWPILibIO {
public:
    PyWPILibIO() {
        akit::conduit::wpilibio::make_buffer();
    }
    
    void start() {
        akit::conduit::wpilibio::start();
    }
    
    nb::dict capture_data() {
        akit::conduit::wpilibio::capture_data();
        org::littletonrobotics::conduit::schema::CoreInputs* data = static_cast<org::littletonrobotics::conduit::schema::CoreInputs*>(akit::conduit::wpilibio::shared_buf);
        return coreinputs_to_dict(data);
    }
    
    ~PyWPILibIO() {
        if (akit::conduit::wpilibio::shared_buf) {
            free(akit::conduit::wpilibio::shared_buf);
            akit::conduit::wpilibio::shared_buf = nullptr;
        }
    }
};