#pragma once

#include <stdint.h>
#include <memory>
#include <map>

#include "ts_packet.h"

class SimpleBuffer;
class TsFrame;

class MpegTsDemuxer
{
public:
    MpegTsDemuxer();
    virtual ~MpegTsDemuxer();

public:
    using pcr_callback_t = std::function<void(uint64_t)>;
    std::shared_ptr<const TsFrame> decode(SimpleBuffer& in, pcr_callback_t pcr_callback = 0);
    // stream, pid
    std::map<uint8_t, int> stream_pid_map;
    int pmt_id;

private:
    // pid, frame
    std::map<int, std::shared_ptr<TsFrame>> _ts_frames;
    int _pcr_id;
};

