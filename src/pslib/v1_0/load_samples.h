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

// Own
#include "pslib/v1_0/psd_filename.h"
#include "pslib/v1_0/psd_t.h"
#include "pslib/v1_0/psi_t.h"
#include "pslib/v1_0/samples_t.h"

// StdLib
#include <chrono>
#include <fstream>
#include <vector>

namespace pslib::v1_0 {
    inline samples_t load_samples(const psi_t& psi,
        std::chrono::nanoseconds begin = std::chrono::nanoseconds(0),
        std::chrono::nanoseconds end = std::chrono::nanoseconds(-1))
    {
        if (end <= std::chrono::nanoseconds(-1)) {
            end = psi.length();
        }
        auto samples = pslib::v1_0::samples_t(psi, begin, end);

        std::chrono::nanoseconds current = std::chrono::nanoseconds(0);
        for (const auto& psd : psi.psds) {
            auto psd_filename = pslib::v1_0::psd_filename(psi, psd);
            // Open psd file to read data
            std::ifstream psd_ifstream(psd_filename, std::ios::binary);

            const size_t probe_count = psi.probes.size();
            auto data_count = psd.data_count;
            while (psd_ifstream.good() && data_count--) {
                if (current < begin) {
                    psd_ifstream.ignore(std::streamsize(
                        sizeof(pslib::v1_0::data_stream_t) * probe_count +
                        sizeof(pslib::v1_0::event_t) * (probe_count + 1)));
                }
                else if (current > end) {
                    return samples;
                }
                else {
                    // Read DataStream of each probe
                    for (size_t i = 0; i < probe_count; ++i) {
                        auto data_stream = pslib::v1_0::data_stream_t();
                        psd_ifstream.read(
                            reinterpret_cast< char* >(&data_stream),
                            sizeof(data_stream));
                        samples.values.push_back(std::move(data_stream));
                    }
                    // Read events of each probe and the global event
                    for (size_t i = 0; i < (probe_count + 1); ++i) {
                        auto event = pslib::v1_0::event_t();
                        psd_ifstream.read(
                            reinterpret_cast< char* >(&event), sizeof(event));
                        samples.events.push_back(std::move(event));
                    }
                }
                current += psi.sampling_interval();
            }
        }
        return samples;
    }
}
