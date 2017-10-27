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
#include "pslib/v1_0/data_stream_t.h"
#include "pslib/v1_0/event_t.h"
#include "pslib/v1_0/psi_t.h"
#include "pslib/v1_0/sample_t.h"

// StdLib
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pslib::v1_0 {
    class sample_t;

    class samples_t {
        public:
        psi_t psi;
        std::chrono::nanoseconds begin;
        std::chrono::nanoseconds end;
        std::vector< data_stream_t > values;
        std::vector< event_t > events;

        public:
        inline samples_t(const psi_t& p, std::chrono::nanoseconds b,
            std::chrono::nanoseconds e)
        {
            this->psi = p;
            this->begin = b;
            this->end = e;
            this->values.reserve(
                size_t((e - b) / p.sampling_interval()) * p.probes.size());
            this->events.reserve(size_t((e - b) / p.sampling_interval()) *
                                 (p.probes.size() + 1));
        }

        inline size_t size() const
        {
            return this->values.size() / this->psi.probes.size();
        }

        inline sample_t at(size_t idx) const
        {
            auto idx_offset =
                this->begin.count() / this->psi.sampling_interval().count();
            auto time =
                (idx + size_t(idx_offset)) * this->psi.sampling_interval();
            return sample_t(this->psi, this->values, this->events, idx, time);
        }
    };

    inline bool operator==(const samples_t& lhs, const samples_t& rhs)
    {
        if (lhs.psi != rhs.psi) {
            return false;
        }
        if (lhs.begin != rhs.begin) {
            return false;
        }
        if (lhs.end != rhs.end) {
            return false;
        }
        if (lhs.values != rhs.values) {
            return false;
        }
        if (lhs.events != rhs.events) {
            return false;
        }
        return true;
    }

    inline bool operator!=(const samples_t& lhs, const samples_t& rhs)
    {
        return !(lhs == rhs);
    }
}
