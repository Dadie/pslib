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
#include <boost/filesystem.hpp>

// Own
#include "pslib/v1_0/samples_t.h"

// StdLib
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace pslib::v1_0 {
    inline void save_samples(const pslib::v1_0::samples_t& samples,
        const std::string& directory, const std::string& base_name)
    {
        if (samples.begin_time > std::chrono::nanoseconds(0)) {
            // TODO: Better Error
            throw std::runtime_error("Given pslib::v1_0::sample_t doesn't "
                                     "begin on 0 and cant be saved to .psd "
                                     "files");
        }
        if (samples.end_time != samples.psi.length()) {
            // TODO: Better Error
            throw std::runtime_error("Given pslib::v1_0::sample_t doesn't end "
                                     "as specified in psi element");
        }

        for (auto& psd : samples.psi.psds) {
            std::string psd_filename = directory + "/" + base_name + "_" +
                                       std::to_string(psd.id) + ".psd";
            std::ofstream psd_file(psd_filename);
            // TODO: Error if ostream cant be opend
            size_t writen_bytes = 0;
            for (size_t i = 0; i < size_t(psd.data_count); ++i) {
                auto index = i + (psd.offset > 0 ? size_t(psd.offset - 1) : 0);
                auto s = samples.at(index);
                for (auto& ds : s.values) {
                    psd_file.write(
                        reinterpret_cast< const char* >(&ds), sizeof(ds));
                    writen_bytes += sizeof(ds);
                }
                for (auto& e : s.events) {
                    psd_file.write(
                        reinterpret_cast< const char* >(&e), sizeof(e));
                    writen_bytes += sizeof(e);
                }
            }
            psd_file.flush();
            psd_file.close();

            // If less then 1 GiB of data was writen, resize psd file to 1 GiB
            if (writen_bytes < (1ul * 1024ul * 1024ul * 1024ul)) {
                boost::filesystem::resize_file(
                    psd_filename, 1ul * 1024ul * 1024ul * 1024ul);
            }
        }
    }
}
