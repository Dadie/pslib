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
#include "pslib/v1_0/probe_t.h"
#include "pslib/v1_0/psd_t.h"

// StdLib
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace pslib::v1_0 {
    class psi_t {
        public:
        std::string filename;

        uint32_t checksum;
        uint64_t sampling_rate; // (in Hz)
        uint64_t sampling_count;
        std::vector< pslib::v1_0::probe_t > probes;
        std::vector< pslib::v1_0::psd_t > psds;

        public:
        // Return the sampling interval in nano seconds
        inline std::chrono::nanoseconds sampling_interval() const
        {
            return std::chrono::nanoseconds(
                1000 * 1000 * 1000 / this->sampling_rate);
        }

        inline std::chrono::nanoseconds length() const
        {
            return this->sampling_interval() * this->sampling_count;
        }
    };

    inline bool operator==(const psi_t& lhs, const psi_t& rhs)
    {
        if (lhs.checksum != rhs.checksum) {
            return false;
        }
        if (lhs.sampling_rate != rhs.sampling_rate) {
            return false;
        }
        if (lhs.sampling_count != rhs.sampling_count) {
            return false;
        }
        if (lhs.probes != rhs.probes) {
            return false;
        }
        if (lhs.psds != rhs.psds) {
            return false;
        }
        return true;
    }

    inline bool operator!=(const psi_t& lhs, const psi_t& rhs)
    {
        return !(lhs == rhs);
    }
}
