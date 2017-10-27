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
#include "pslib/v1_0/psi_t.h"

// StdLib
#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace pslib::v1_0 {
    inline bool validate_psi(const pslib::v1_0::psi_t& psi)
    {
        const auto& filename = psi.filename;
        boost::property_tree::ptree psi_xml;
        boost::property_tree::read_xml(filename, psi_xml);

        // Get PSI Version
        auto psi_version =
            psi_xml.get< std::string >("PSI.Version.<xmlattr>.Value");
        // Check PSI Version
        if (psi_version != "1.0") {
            throw std::runtime_error(
                "Wrong PSI Version. Expected 1.0 but got " + psi_version +
                " in " + filename);
        }

        // Get PSD Version
        auto psd_version =
            psi_xml.get< std::string >("PSI.PSD.Version.<xmlattr>.Value");
        // Check PSD Version
        if (psd_version != "1.0") {
            throw std::runtime_error(
                "Wrong PSD Version. Expected 1.0 but got " + psd_version +
                " in " + filename);
        }

        // Verify checksum for psi file
        // TODO!

        // Check SamplingRate
        if (psi.sampling_rate <= 0) {
            throw std::runtime_error("Invalid sampling rate of " +
                                     std::to_string(psi.sampling_rate) + " in" +
                                     filename);
        }
        if (psi.sampling_rate < 1000) {
            throw std::runtime_error("To low sampling rate (<1000Hz) of " +
                                     std::to_string(psi.sampling_rate) + " in" +
                                     filename);
        }

        // Check SamplingCount
        if (psi.sampling_count <= 0) {
            throw std::runtime_error("Invalid sampling count of " +
                                     std::to_string(psi.sampling_count) +
                                     " in" + filename);
        }

        // Check if expected and present Probes are equal
        int64_t psi_datastream_count =
            psi_xml.get< int64_t >("PSI.DataStream.<xmlattr>.Count");
        if (psi_datastream_count <= 0 ||
            size_t(psi_datastream_count) != psi.probes.size()) {
            throw std::runtime_error(
                "Expected " + std::to_string(psi_datastream_count) +
                " Probes but got " + std::to_string(psi.probes.size()) +
                " in " + filename);
        }

        // Check Probes
        std::vector< int64_t > ports;
        for (size_t i = 0; i < psi.probes.size(); ++i) {
            auto& probe = psi.probes[ i ];
            if (probe.id <= 0 || size_t(probe.id) != i + 1) {
                throw std::runtime_error("Probe No. " + std::to_string(i + 1) +
                                         " has an invalid id of " +
                                         std::to_string(probe.id) + " in " +
                                         filename);
            }
            if (probe.port <= 0) {
                throw std::runtime_error("Probe No. " + std::to_string(i + 1) +
                                         " has an invalid port of " +
                                         std::to_string(probe.port) + " in " +
                                         filename);
            }
            if (probe.kind != PROBE_KIND::ACM &&
                probe.kind != PROBE_KIND::STD) {
                throw std::runtime_error("Probe No. " + std::to_string(i + 1) +
                                         " has an unknown kind of " +
                                         std::to_string(probe.kind) + " in " +
                                         filename);
            }
            if (std::find(ports.begin(), ports.end(), probe.port) !=
                ports.end()) {
                throw std::runtime_error("Probe No. " + std::to_string(i + 1) +
                                         " uses a port already used in " +
                                         filename);
            }
            ports.push_back(probe.port);
        }

        // Check PSDFiles
        int64_t data_count = 0;
        for (size_t i = 0; i < psi.psds.size(); ++i) {
            auto& psd = psi.psds[ i ];
            if (psd.id <= 0 || size_t(psd.id) != i + 1) {
                throw std::runtime_error(
                    "PSDFile No. " + std::to_string(i + 1) +
                    " has an invalid id of " + std::to_string(psd.id) + " in " +
                    filename);
            }
            if (psd.offset < 0) {
                throw std::runtime_error(
                    "PSDFile No. " + std::to_string(psd.id) +
                    " has an invalid offset of " + std::to_string(psd.offset) +
                    " in " + filename);
            }
            if (psd.data_count < 0) {
                throw std::runtime_error(
                    "PSDFile No. " + std::to_string(psd.id) +
                    " has an invalid data count of " +
                    std::to_string(psd.data_count) + " in " + filename);
            }
            if (psd.event_count < 0) {
                throw std::runtime_error(
                    "PSDFile No. " + std::to_string(psd.id) +
                    " has an invalid event count of " +
                    std::to_string(psd.event_count) + " in " + filename);
            }
            if (i > 0 && psd.offset != data_count + 1) {
                throw std::runtime_error(
                    "PSDFile No. " + std::to_string(psd.id) + " with offset " +
                    std::to_string(psd.offset) +
                    " doesn't match with data count till this No. of " +
                    std::to_string(data_count) + " offset in " + filename);
            }
            data_count += psd.data_count;
        }

        return true;
    }
}
