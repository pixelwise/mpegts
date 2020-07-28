//
// Created by Anders Cedronius on 2020-07-01.
//

#ifndef MPEGTS_UNIT_TEST_4_H
#define MPEGTS_UNIT_TEST_4_H

#include <iostream>
#include "mpegts/mpegts_demuxer.h"
#include "mpegts/mpegts_muxer.h"

class UnitTest4 {
public:
    bool runTest();
private:
    void muxOutput(SimpleBuffer &rTsOutBuffer);
    void dmxOutput(TsFrame *pEs);

    MpegTsDemuxer mDemuxer;
    int mFrameCounter = 1;
    bool mUnitTestStatus = true;
    bool mFrameInTransit = false;
};

#endif //MPEGTS_UNIT_TEST_4_H
