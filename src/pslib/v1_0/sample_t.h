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
#include <boost/multi_array.hpp>

// Own
#include "pslib/v1_0/data_stream_t.h"
#include "pslib/v1_0/event_t.h"
#include "pslib/v1_0/psi_t.h"

// StdLib
#include <chrono>
#include <vector>

namespace pslib::v1_0 {
    class samples_t;

    class sample_t {
        friend samples_t;

        public:
        std::chrono::nanoseconds time;
        boost::multi_array_ref< const pslib::v1_0::data_stream_t, 1 > values;
        boost::multi_array_ref< const pslib::v1_0::event_t, 1 > events;

        private:
        sample_t(const psi_t& psi, const std::vector< data_stream_t >& val,
            const std::vector< event_t >& ev, size_t sample_idx,
            std::chrono::nanoseconds t)
            : time{ t }
            , values{ val.data() + (psi.probes.size() * sample_idx),
                boost::extents[ int64_t(psi.probes.size()) ] }
            , events{ ev.data() + ((psi.probes.size() + 1) * sample_idx),
                boost::extents[ int64_t(psi.probes.size()) + 1 ] }
        {
        }
    };

    inline bool operator==(const sample_t& lhs, const sample_t& rhs)
    {
        if (lhs.time != rhs.time) {
            return false;
        }
        if (lhs.values.size() != rhs.values.size()) {
            return false;
        }
        for (boost::multi_array_types::index i = 0;
             size_t(i) < lhs.values.size(); ++i) {
            if (lhs.values[ i ] != rhs.values[ i ]) {
                return false;
            }
        }
        if (lhs.events.size() != rhs.events.size()) {
            return false;
        }
        for (boost::multi_array_types::index i = 0;
             size_t(i) < lhs.events.size(); ++i) {
            if (lhs.events[ i ] != rhs.events[ int(i) ]) {
                return false;
            }
        }
        return true;
    }

    inline bool operator!=(const sample_t& lhs, const sample_t& rhs)
    {
        return !(lhs == rhs);
    }
}
