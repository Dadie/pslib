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
#include <iostream>

// Own
#include <pslib/pslib_v1_0.h>

int main(int argc, char* argv[])
{

    auto psi = pslib::v1_0::psi_t();
    {
        psi.filename = "./test.save_and_load_samples_3gib.psi";
        psi.checksum = 0;
        psi.sampling_rate = 1000;  // 1000 Hz
        psi.sampling_count = 1500; // 1024 Samples

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

        auto psd_1 = pslib::v1_0::psd_t();
        {
            psd_1.id = 1;     // id needs to be > 1 to be valid
            psd_1.offset = 0; // Sum of data_count of each psd with a smaller id
            psd_1.data_count = 500; // Number of Samples in this psd file
            psd_1.event_count =
                0; // Number of events with event happend flag set
        }
        psi.psds.push_back(psd_1);

        auto psd_2 = pslib::v1_0::psd_t();
        {
            psd_2.id = 2; // id needs to be > 1 to be valid
            psd_2.offset =
                psd_1.data_count + 1; // Sum of data_count of each psd
                                      // with a smaller id
            psd_2.data_count = 500;   // Number of Samples in this psd file
            psd_2.event_count =
                0; // Number of events with event happend flag set
        }
        psi.psds.push_back(psd_2);

        auto psd_3 = pslib::v1_0::psd_t();
        {
            psd_3.id = 3; // id needs to be > 1 to be valid
            psd_3.offset = psd_2.offset + psd_2.data_count; // Sum of data_count
                                                            // of each psd with
                                                            // a smaller id
            psd_3.data_count = 500; // Number of Samples in this psd file
            psd_3.event_count =
                0; // Number of events with event happend flag set
        }
        psi.psds.push_back(psd_3);
    }

    // write psi to file
    pslib::v1_0::save_psi(psi, "./", "test.save_and_load_samples_3gib");

    // validate psi
    if (!pslib::v1_0::validate_psi(psi)) {
        std::cout << "Invalid PSI " << psi.filename << std::endl;
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
    pslib::v1_0::save_samples(samples, "./", "test.save_and_load_samples_3gib");

    auto loaded_psi =
        pslib::v1_0::load_psi("./test.save_and_load_samples_3gib.psi");
    if (loaded_psi != psi) {
        std::cout << "Loaded psi is not the same as generated psi "
                  << std::endl;
        return EXIT_FAILURE;
    }

    auto loaded_samples = pslib::v1_0::load_samples(loaded_psi);
    if (loaded_samples != samples) {
        std::cout << "Samples are not the same" << std::endl;
        std::cout << "samples.size()         " << samples.size() << std::endl;
        std::cout << "loaded_samples.size()  " << samples.size() << std::endl;
        std::cout << "samples.begin          " << samples.begin.count()
                  << std::endl;
        std::cout << "loaded_samples.begin() " << samples.begin.count()
                  << std::endl;
        std::cout << "samples.end            " << samples.end.count()
                  << std::endl;
        std::cout << "loaded_samples.end     " << samples.end.count()
                  << std::endl;
        for (size_t i = 0; i < samples.values.size(); ++i) {
            if (loaded_samples.values.at(i) != samples.values.at(i)) {
                std::cout << "First Value Diff Found on Index " << i
                          << std::endl;
                break;
            }
        }
        for (size_t i = 0; i < samples.events.size(); ++i) {
            if (loaded_samples.events.at(i) != samples.events.at(i)) {
                std::cout << "First Event Diff Found on Index " << i
                          << std::endl;
                break;
            }
        }

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
