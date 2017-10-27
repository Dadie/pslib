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
#include <stdexcept>
#include <string>
#include <vector>

namespace pslib::v1_0 {
    inline pslib::v1_0::psi_t load_psi(const std::string& filename)
    {

        auto psi = pslib::v1_0::psi_t();
        {
            psi.filename = filename;
        }

        boost::property_tree::ptree psi_xml;
        boost::property_tree::read_xml(filename, psi_xml);

        // Get Checksum
        auto psi_checksum =
            psi_xml.get< std::string >("PSI.Checksum.<xmlattr>.Value");
        try {
            psi.checksum =
                boost::lexical_cast< uint32_t >(psi_checksum.c_str() + 2);
        }
        catch (boost::bad_lexical_cast&) {
            throw std::runtime_error(
                "Invalid Checksum Value " + psi_checksum + " in " + filename);
        }

        // Get SamplingRate (and transform it from kHz to Hz)
        psi.sampling_rate =
            psi_xml.get< uint64_t >(
                "PSI.Measurement.SamplingRate.<xmlattr>.Value") *
            1000;

        // Get SamplingCount
        psi.sampling_count = psi_xml.get< uint64_t >(
            "PSI.Measurement.SamplingCount.<xmlattr>.Value");

        // Get all inner DataStreams (Probes)
        for (auto& v : psi_xml.get_child("PSI.DataStream")) {
            // Skip all non DataStream Elements
            if (v.first != "DataStream") {
                continue;
            }
            auto probe = pslib::v1_0::probe_t();
            {
                double nan = std::numeric_limits< double >::quiet_NaN();
                probe.id = v.second.get< int64_t >("<xmlattr>.Id", -1);
                probe.port = v.second.get< int64_t >("<xmlattr>.ProbeID", -1);
                probe.kind = PROBE_KIND(
                    v.second.get< int64_t >("<xmlattr>.ProbeKind", -1));

                probe.voltage_min =
                    v.second.get< double >("<xmlattr>.voltageMin", nan);
                probe.voltage_max =
                    v.second.get< double >("<xmlattr>.voltageMax", nan);
                probe.current_min =
                    v.second.get< double >("<xmlattr>.currentMin", nan);
                probe.current_max =
                    v.second.get< double >("<xmlattr>.currentMax", nan);
            }
            psi.probes.push_back(std::move(probe));
        }

        // Get all PSDFiles
        for (auto& v : psi_xml.get_child("PSI.PSD")) {
            // Skip all non PSDFile Elements
            if (v.first != "PSDFile") {
                continue;
            }
            auto psd = pslib::v1_0::psd_t();
            {
                psd.id = v.second.get< int64_t >("<xmlattr>.Id", -1);
                psd.offset = v.second.get< int64_t >("<xmlattr>.Offset", -1);
                psd.data_count =
                    v.second.get< int64_t >("<xmlattr>.DataCount", -1);
                psd.event_count =
                    v.second.get< int64_t >("<xmlattr>.EventCount", -1);
            }
            psi.psds.push_back(std::move(psd));
        }

        if (!pslib::v1_0::validate_psi(psi)) {
            throw std::runtime_error("Invalid PSI v1.0 file " + filename);
        }

        return psi;
    }
}
