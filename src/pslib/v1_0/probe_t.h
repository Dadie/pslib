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
#include "pslib/v1_0/probe_kind.h"

// StdLib
#include <cmath>
#include <cstdint>

namespace pslib::v1_0 {
    class probe_t {
        public:
        int64_t id;
        int64_t port;
        PROBE_KIND kind;

        double current_min;
        double current_max;
        double voltage_min;
        double voltage_max;
    };

    inline bool operator==(const probe_t& lhs, const probe_t& rhs)
    {
        if (lhs.id != rhs.id) {
            return false;
        }
        if (lhs.port != rhs.port) {
            return false;
        }
        if (lhs.kind != rhs.kind) {
            return false;
        }
        if (std::isnan(lhs.current_min) != std::isnan(rhs.current_min)) {
            return false;
        }
        if (std::isnan(lhs.current_max) != std::isnan(rhs.current_max)) {
            return false;
        }
        if (std::isnan(lhs.voltage_min) != std::isnan(rhs.voltage_min)) {
            return false;
        }
        if (std::isnan(lhs.voltage_max) != std::isnan(rhs.voltage_max)) {
            return false;
        }
        if (!std::isnan(lhs.current_min) &&
            (lhs.current_min > rhs.current_min ||
                lhs.current_min < rhs.current_min)) {
            return false;
        }
        if (!std::isnan(lhs.current_max) &&
            (lhs.current_max > rhs.current_max ||
                lhs.current_max < rhs.current_max)) {
            return false;
        }
        if (!std::isnan(lhs.voltage_min) &&
            (lhs.voltage_min > rhs.voltage_min ||
                lhs.voltage_min < rhs.voltage_min)) {
            return false;
        }
        if (!std::isnan(lhs.voltage_max) &&
            (lhs.voltage_max > rhs.voltage_max ||
                lhs.voltage_max < rhs.voltage_max)) {
            return false;
        }
        return true;
    }

    inline bool operator!=(const probe_t& lhs, const probe_t& rhs)
    {
        return !(lhs == rhs);
    }
}
