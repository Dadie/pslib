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

// StdLib
#include <chrono>
#include <cstdlib>

// Own
#include <pslib/pslib_v1_0.h>

int main(int argc, char* argv[])
{

    auto psi = pslib::v1_0::psi_t();
    {
        psi.filename = "./test.save_and_load_samples_iter.psi";
        psi.checksum = 0;
        psi.sampling_rate = 1000;  // 1000 Hz
        psi.sampling_count = 1024; // 1024 Samples

        // Add Probe
        auto probe = pslib::v1_0::probe_t();
        {
            probe.id = 1;   // id needs to be > 1 to be valid
            probe.port = 1; // port needs to be > 0 and < 4 to be valid
            probe.kind = pslib::v1_0::PROBE_KIND::STD;

            // If no min/max current or voltage is known set them to NaN
            probe.current_min = std::numeric_limits< double >::quiet_NaN();
            probe.current_max = std::numeric_limits< double >::quiet_NaN();
            probe.voltage_min = std::numeric_limits< double >::quiet_NaN();
            probe.voltage_max = std::numeric_limits< double >::quiet_NaN();
        }
        psi.probes.push_back(probe);

        // Add PSDFiles
        // A .psd file has a size of 1GiB and can hold
        // Size of Sample = Number of Probes * 2 * 8 + Number of Probes * 2 +
        // Number of Probes
        // Maximum number of samples per psd = Size of Sample /
        // (1*1024*1024*1024)
        //
        // In this example:
        // Number of Probes = 1
        // Size of Sample = 19 (Byte)
        // Maximum number of samples per psd = ~17,695,128,917
        auto psd = pslib::v1_0::psd_t();
        {
            psd.id = 1;     // id needs to be > 1 to be valid
            psd.offset = 0; // Sum of data_count of each psd with a smaller id
            psd.data_count = 1024; // Number of Samples in this psd file
            psd.event_count = 0; // Number of events with event happend flag set
        }
        psi.psds.push_back(psd);
    }

    // write psi to file
    pslib::v1_0::save_psi(psi, "./", "test.save_and_load_samples_iter");

    // validate psi
    if (!pslib::v1_0::validate_psi(psi)) {
        return EXIT_FAILURE;
    }

    auto samples =
        pslib::v1_0::samples_t(psi, std::chrono::nanoseconds(0), psi.length());
    {
        for (size_t i = 0; i < psi.sampling_count; ++i) {
            for (size_t j = 0; j < psi.probes.size(); ++j) {
                auto ds = pslib::v1_0::data_stream_t();
                {
                    ds.current = double(i) / double(psi.sampling_count);
                    ds.voltage = double(psi.sampling_count) / double(i);
                }
                samples.values.push_back(ds);
            }
            for (size_t j = 0; j < (psi.probes.size() + 1); ++j) {
                auto e = pslib::v1_0::event_t();
                {
                    e.data = 0;
                }
                samples.events.push_back(e);
            }
        }
    }

    // write samples to psds
    pslib::v1_0::save_samples(samples, "./", "test.save_and_load_samples_iter");

    auto loaded_psi =
        pslib::v1_0::load_psi("./test.save_and_load_samples_iter.psi");
    if (loaded_psi != psi) {
        return EXIT_FAILURE;
    }

    auto loaded_samples = pslib::v1_0::load_samples(loaded_psi);
    auto it = samples.begin();
    for (auto s : loaded_samples) {
        if (s != *it) {
            return EXIT_FAILURE;
        }
        it++;
    }
    if (it != samples.end()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
