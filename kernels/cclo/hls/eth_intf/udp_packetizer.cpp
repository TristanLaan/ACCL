/*******************************************************************************
#  Copyright (C) 2021 Xilinx, Inc
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
# *******************************************************************************/

#include "eth_intf.h"
#include <iostream>
#include <sstream>

using namespace std;

void udp_packetizer(
    STREAM<stream_word > & in,
    STREAM<stream_word > & out,
    STREAM<eth_header> & cmd,
    STREAM<ap_uint<32> > & sts,
    unsigned int max_pktsize
) {
#pragma HLS INTERFACE axis register both port=in
#pragma HLS INTERFACE axis register both port=out
#pragma HLS INTERFACE axis register both port=cmd
#pragma HLS INTERFACE axis register both port=sts
#pragma HLS INTERFACE s_axilite port=max_pktsize
#pragma HLS INTERFACE s_axilite port=return

unsigned const bytes_per_word = DATA_WIDTH/8;

//read commands from the command stream
eth_header cmdword = STREAM_READ(cmd);
int bytes_to_process = cmdword.count + bytes_per_word;

unsigned int pktsize = 0;
int bytes_processed  = 0;

while(bytes_processed < bytes_to_process){
#pragma HLS PIPELINE II=1
	stream_word outword;
	outword.dest = cmdword.dst;
	// Always set TKEEP to -1 to prevent problem with RoCE kernel. This will mean
	// that packets will contain at max bytes_per_word-1 extra bytes of unused
	// padding.
	outword.keep = -1;
	//if this is the first word, put the count in a header
	if(bytes_processed == 0){
        outword.data(DATA_WIDTH-1, HEADER_LENGTH) = 0;
        outword.data(HEADER_LENGTH-1,0) = (ap_uint<HEADER_LENGTH>)cmdword;
	} else {
		outword.data = STREAM_READ(in).data;
	}
	//signal ragged tail
	int bytes_left = (bytes_to_process - bytes_processed);
	if(bytes_left < bytes_per_word){
		bytes_processed += bytes_left;
	}else{
		bytes_processed += bytes_per_word;
	}
	pktsize++;
	//after every max_pktsize words, or if we run out of bytes, assert TLAST
	if((pktsize == max_pktsize) || (bytes_left <= bytes_per_word)){
		outword.last = 1;
		pktsize = 0;
	}else{
		outword.last = 0;
	}
	//write output stream
	STREAM_WRITE(out, outword);
}
//acknowledge that message_seq has been sent successfully
ap_uint<32> outsts;
STREAM_WRITE(sts, cmdword.seqn);
}
