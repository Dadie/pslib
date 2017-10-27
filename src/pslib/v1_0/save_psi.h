/**
 * Copyright (c) 2017, Daniel "Dadie" Korner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. The source code and binary must not be used for military purposes
 *
 * THIS SOFTWARE IS PROVIDED BY Daniel "Dadie" Korner ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Daniel "Dadie" Korner BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once

// Ext
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// Own
#include "pslib/v1_0/probe_kind.h"
#include "pslib/v1_0/probe_t.h"
#include "pslib/v1_0/psd_t.h"
#include "pslib/v1_0/psi_t.h"
#include "pslib/v1_0/validate_psi.h"

// StdLib
#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace pslib::v1_0 {

    inline void save_psi(const pslib::v1_0::psi_t& psi,
        const std::string& directory, const std::string& base_name)
    {
        std::string filename = directory + "/" + base_name + ".psi";
        boost::property_tree::ptree psi_xml;

        psi_xml.add("PSI.Version.<xmlattr>.Value", "1.0");

        std::stringstream cksum_hexsstr;
        cksum_hexsstr << "0x";
        cksum_hexsstr << std::hex;
        uint32_t cksum = psi.checksum;
        for (size_t i = 0; i < 8; ++i) {
            cksum_hexsstr << (cksum & 0xF);
            cksum = cksum >> 4;
        }
        std::string checksum_value;
        cksum_hexsstr >> checksum_value;
        psi_xml.add("PSI.Checksum.<xmlattr>.Value", checksum_value);
        psi_xml.add("PSI.Measurement.SamplingRate.<xmlattr>.Value",
            psi.sampling_rate / 1000);
        psi_xml.add("PSI.Measurement.SamplingCount.<xmlattr>.Value",
            psi.sampling_count);

        auto& data_stream_element = psi_xml.add("PSI.DataStream", "");
        data_stream_element.add("<xmlattr>.Count", psi.probes.size());
        for (auto& probe : psi.probes) {
            auto& ds_element = data_stream_element.add("DataStream", "");
            ds_element.add("<xmlattr>.Id", probe.id);
            ds_element.add("<xmlattr>.ProbeID", probe.port);
            ds_element.add("<xmlattr>.ProbeKind", probe.kind);
            if (!std::isnan(probe.voltage_min)) {
                ds_element.add("<xmlattr>.voltageMin", probe.voltage_min);
            }
            if (!std::isnan(probe.voltage_max)) {
                ds_element.add("<xmlattr>.voltageMax", probe.voltage_max);
            }
            if (!std::isnan(probe.current_min)) {
                ds_element.add("<xmlattr>.currentMin", probe.current_min);
            }
            if (!std::isnan(probe.current_max)) {
                ds_element.add("<xmlattr>.currentMax", probe.current_max);
            }
        }

        auto& psd_element = psi_xml.add("PSI.PSD", "");
        psd_element.add("<xmlattr>.Count", psi.psds.size());
        psd_element.add("Version.<xmlattr>.Value", "1.0");
        for (auto& psd : psi.psds) {
            auto& psdfile_element = psd_element.add("PSDFile", "");
            psdfile_element.add("<xmlattr>.Id", psd.id);
            psdfile_element.add("<xmlattr>.Offset", psd.offset);
            psdfile_element.add("<xmlattr>.DataCount", psd.data_count);
            psdfile_element.add("<xmlattr>.EventCount", psd.event_count);
        }

        boost::property_tree::write_xml(filename, psi_xml);
    }
}
