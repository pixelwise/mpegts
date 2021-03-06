#pragma once

#include <map>
#include <stdint.h>

#include "ts_packet.h"

class SimpleBuffer;
class TsFrame;

class MpegTsMuxer
{
public:

    MpegTsMuxer();
    ~MpegTsMuxer();
    void encode(const TsFrame& frame, const std::map<uint8_t, int>& stream_pid_map, uint16_t pmt_pid, SimpleBuffer& sb);
    void create_pcr(SimpleBuffer& sb, uint64_t pcr = 0);
    void create_null(SimpleBuffer& sb);

private:
    
    void create_pat(SimpleBuffer& sb, uint16_t pmt_pid, uint8_t cc);
    void create_pmt(SimpleBuffer& sb, const std::map<uint8_t, int>& stream_pid_map, uint16_t pmt_pid, uint8_t cc);
    void create_pes(const TsFrame& frame, SimpleBuffer& sb);
    uint8_t get_cc(uint32_t with_pid);
    bool should_create_pat();

    std::map<uint32_t, uint8_t> _pid_cc_map; 
};

