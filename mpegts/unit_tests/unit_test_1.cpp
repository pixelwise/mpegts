//
// Created by Anders Cedronius on 2020-07-01.
//


//This unit test is

//1. Sending a vector of bytes from 1 byte to TEST_VECTOR_SIZE (4000) bytes
//The vector is a linear counting uint8_t that is multiplexed and demuxed
//The result is compared against the expected length and vector linearity

//2. Verify the PTS and DTS values (They are set to the packet size and packet number)

//3. Verify the muxer is spiting out a modulo 188 size packets

//4. Every tenth frame is set to random_access_indicator. Verify the correctness after muxing demuxing

//This unit test is sending each TS to the demuxer meaning 188 bytes at a time


#include "unit_test_1.h"

//AAC (ADTS) audio
#define TYPE_AUDIO 0x0f
//H.264 video
#define TYPE_VIDEO 0x1b
//Audio PID
#define AUDIO_PID 257
//Video PID
#define VIDEO_PID 256
//PMT PID
#define PMT_PID 100

//Test Vector size
#define TEST_VECTOR_SIZE 4000

void UnitTest1::dmxOutput(const TsFrame& frame){

    //Is the frame correctly marked as random_access_indicator
    if ((mFrameCounter%10)?0:1 != frame.random_access_indicator) {
        std::cout << "random_access_indicator indication error: " << unsigned(frame.random_access_indicator) << " Frame -> " << unsigned(mFrameCounter) << std::endl;
        mUnitTestStatus = false;
    }

    //verify expected size
    if (frame.data->size() != mFrameCounter ) {
        std::cout << "Frame size missmatch " << unsigned(frame.data->size()) << std::endl;
        mUnitTestStatus = false;
    }

    //Make sure PTS and DTS are set as expected
    if (frame.pts != mFrameCounter ||  frame.dts != mFrameCounter) {
        std::cout << "PTS - DTS" << std::endl;
        mUnitTestStatus = false;
    }

    char referenceVector = 0;
    //verify expected vector
    for (int lI = 0 ; lI < mFrameCounter ; lI++) {
        if (frame.data->data()[lI] != referenceVector++ ) {
            std::cout << "Content missmatch for packet size -> " << unsigned(mFrameCounter) << " at byte: " << unsigned(lI);
            std::cout << " Expected " << unsigned(referenceVector -1 ) << " got " << (uint8_t)frame.data->data()[lI] + 0 << std::endl;
            mUnitTestStatus = false;
        }
    }

    mFrameInTransit = false;

}

void UnitTest1::muxOutput(SimpleBuffer &rTsOutBuffer) {
    //Double to fail at non integer data
    double packets = (double) rTsOutBuffer.size() / 188.0;
    if (packets != (int) packets) {
        std::cout << "Payload not X * 188 " << std::endl;
        mUnitTestStatus = false;
    }

    auto lpData = rTsOutBuffer.data();

    for (int lI = 0 ; lI < packets ; lI++) {
        SimpleBuffer lIn;
        lIn.append(lpData+(lI*188), 188);
        if (auto frame = mDemuxer.decode(lIn))
            dmxOutput(*frame);
    }
}

bool UnitTest1::runTest() {

    uint8_t testVector[TEST_VECTOR_SIZE];
    std::map<uint8_t, int> gStreamPidMap;
    gStreamPidMap[TYPE_VIDEO] = VIDEO_PID;
    MpegTsMuxer lMuxer;//(gStreamPidMap, PMT_PID, VIDEO_PID);
    //lMuxer.tsOutCallback = std::bind(&UnitTest1::muxOutput, this, std::placeholders::_1);

    //Make Vector
    for (int x = 0; x < TEST_VECTOR_SIZE; x++) {
        testVector[x] = x;
    }

    //Run trough all sizes
    int x = 1;
    for (; x < TEST_VECTOR_SIZE+1; x++) {
        SimpleBuffer buffer;
        TsFrame lEsFrame;
        lEsFrame.data = std::make_shared<SimpleBuffer>();
        lEsFrame.data->append((const uint8_t *)&testVector[0], x);
        lEsFrame.pts = x;
        lEsFrame.dts = x;
        lEsFrame.pcr = 0;
        lEsFrame.stream_type = TYPE_VIDEO;
        lEsFrame.stream_id = 224;
        lEsFrame.pid = VIDEO_PID;
        lEsFrame.expected_pes_packet_length = 0;
        lEsFrame.random_access_indicator = (x%10)?0:1;
        lEsFrame.completed = true;

        mFrameInTransit = true;

        lMuxer.encode(lEsFrame, gStreamPidMap, VIDEO_PID, buffer);
        muxOutput(buffer);

        if (mFrameInTransit) {
            std::cout << "Frame " << unsigned(x) << " not muxed/demuxed corectly" << std::endl;
            mUnitTestStatus = false;
        }

        mFrameCounter++;
    }

    if (x != mFrameCounter) {
        std::cout << "mux/demux frame count mismatch " << std::endl;
        mUnitTestStatus = false;
    }

    return mUnitTestStatus; //True = OK
}