/*******************************************************************************
#  Copyright (C) 2022 Advanced Micro Devices, Inc
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#
*******************************************************************************/

#include "benchmark.hpp"
using namespace benchmark;

inline void bench_sendrcv(int *buf0, int *buf1, ap_uint<32> count, ap_uint<32> rank, STREAM<command_word> &cmd_to_cclo,
                   STREAM<command_word> &sts_from_cclo, ap_uint<32> comm_adr, ap_uint<32> dpcfg_adr) {
    accl_hls::ACCLCommand accl(cmd_to_cclo, sts_from_cclo, comm_adr, dpcfg_adr, 0, 0);
    if (rank == 0) {
        accl.send(count, 0, 1, buf0);
        accl.recv(count, 1, 1, buf1);
    } else if (rank == 1) {
        accl.recv(count, 0, 0, buf1);
        accl.send(count, 1, 0, buf1);
    }
}

void benchmark_krnl(
    int *buf0,
    int *buf1,
    int *buf2,
    ap_uint<32> count,
    ap_uint<32> rank,
    ap_uint<32> world_size,
    unsigned int command,
    //parameters pertaining to CCLO config
    ap_uint<32> comm_adr,
    ap_uint<32> dpcfg_adr,
    //streams to and from CCLO
    STREAM<command_word> &cmd_to_cclo,
    STREAM<command_word> &sts_from_cclo,
    STREAM<stream_word> &data_to_cclo,
    STREAM<stream_word> &data_from_cclo
){
#pragma HLS INTERFACE s_axilite port=count
#pragma HLS INTERFACE s_axilite port=rank
#pragma HLS INTERFACE s_axilite port=world_size
#pragma HLS INTERFACE s_axilite port=command
#pragma HLS INTERFACE s_axilite port=comm_adr
#pragma HLS INTERFACE s_axilite port=dpcfg_adr
#pragma HLS INTERFACE m_axi port=buf0 offset=slave
#pragma HLS INTERFACE m_axi port=buf1 offset=slave
#pragma HLS INTERFACE m_axi port=buf2 offset=slave
#pragma HLS INTERFACE axis port=cmd_to_cclo
#pragma HLS INTERFACE axis port=sts_from_cclo
#pragma HLS INTERFACE axis port=data_to_cclo
#pragma HLS INTERFACE axis port=data_from_cclo
#pragma HLS INTERFACE s_axilite port=return
    //set up interfaces

    switch (static_cast<BenchmarkCommand>(command)) {
        case roundtriptime:
            bench_sendrcv(buf0, buf1, 1U, rank, cmd_to_cclo, sts_from_cclo, comm_adr, dpcfg_adr);
            break;
        case sendrcv:
            bench_sendrcv(buf0, buf1, count, rank, cmd_to_cclo, sts_from_cclo, comm_adr, dpcfg_adr);
            break;
    }
}
