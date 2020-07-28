//
// Created by Anders Cedronius on 2020-07-01.
//

#ifndef MPEGTS_UNIT_TEST_6_H
#define MPEGTS_UNIT_TEST_6_H

#include <iostream>
#include "mpegts/mpegts_demuxer.h"
#include "mpegts/mpegts_muxer.h"

class UnitTest6 {
public:
    bool runTest();
private:
    void muxOutputPts(SimpleBuffer &rTsOutBuffer);
    void muxOutputPtsDts(SimpleBuffer &rTsOutBuffer);
    void muxOutputPtsDtsPcr(SimpleBuffer &rTsOutBuffer);
    void dmxOutputPts(const TsFrame *pEs);
    void dmxOutputPtsDts(const TsFrame *pEs);
    void dmxOutputPcr(uint64_t lPcr);

    MpegTsDemuxer mDemuxer;
    MpegTsMuxer *mpMuxer = nullptr;
    bool mUnitTestStatus = true;
    bool mFrameInTransit = false;
    uint64_t pts = 0;
    uint64_t dts = 0;
    uint64_t pcr = 0;
};

#endif //MPEGTS_UNIT_TEST_6_H
