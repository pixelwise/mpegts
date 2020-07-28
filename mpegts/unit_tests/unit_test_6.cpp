//
// Created by Anders Cedronius on 2020-07-01.
//


//This unit test is testing the PTS TDS and PCR values after mux and demux

//PTS only
//PTS + DTS
//PTS + DTS + OOB (meaning separate packets) PCR
//PTS + DTS + PCR in the video pid (PES packet)

#include "unit_test_6.h"

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
//PCR PID
#define PCR_PID 300

//Test Vector size
#define TEST_VECTOR_SIZE 100

#define NUM_FRAMES_LOOP 2000

void UnitTest6::dmxOutputPcr(uint64_t lPcr) {
    if ((dts + pts + pcr) != lPcr) {
        std::cout << "PCR mismatch." << std::endl;
        mUnitTestStatus = false;
    }
}

void UnitTest6::dmxOutputPtsDts(const TsFrame *pEs) {
    if (pts != pEs->pts) {
        std::cout << "PTS mismatch." << std::endl;
        mUnitTestStatus = false;
    }

    if (dts != pEs->dts) {
        std::cout << "DTS mismatch." << std::endl;
        mUnitTestStatus = false;
    }

    uint8_t referenceVector = 0;
    //verify expected vector
    for (int lI = 0 ; lI < TEST_VECTOR_SIZE ; lI++) {
        if (pEs->data->data()[lI] != referenceVector++ ) {
            std::cout << "Content missmatch." << std::endl;
            mUnitTestStatus = false;
        }
    }

    mFrameInTransit = false;
}

void UnitTest6::dmxOutputPts(const TsFrame *pEs){

    if (pts != pEs->pts) {
        std::cout << "PTS missmatch." << std::endl;
    }

    uint8_t referenceVector = 0;
    //verify expected vector
    for (int lI = 0 ; lI < TEST_VECTOR_SIZE ; lI++) {
        if (pEs->data->data()[lI] != referenceVector++ ) {
            std::cout << "Content missmatch." << std::endl;
            mUnitTestStatus = false;
        }
    }

    mFrameInTransit = false;

}

void UnitTest6::muxOutputPts(SimpleBuffer &rTsOutBuffer) {
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
            dmxOutputPts(frame.get());
    }
}

void UnitTest6::muxOutputPtsDts(SimpleBuffer &rTsOutBuffer) {
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
            dmxOutputPtsDts(frame.get());
    }
}

void UnitTest6::muxOutputPtsDtsPcr(SimpleBuffer &rTsOutBuffer) {
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
        if (auto frame = mDemuxer.decode(lIn, [this](auto pcr){dmxOutputPcr(pcr);}))
            dmxOutputPtsDts(frame.get());
    }
}


bool UnitTest6::runTest() {


    uint8_t testVector[TEST_VECTOR_SIZE];
    std::map<uint8_t, int> gStreamPidMap;
    gStreamPidMap[TYPE_VIDEO] = VIDEO_PID;
    mpMuxer = new MpegTsMuxer;

    //Make Vector
    for (int x = 0; x < TEST_VECTOR_SIZE; x++) {
        testVector[x] = x;
    }

    pts = 0;
    dts = 0;
    pcr = 0;

    //Send frames only containing PTS
    for (int x = 1; x < NUM_FRAMES_LOOP; x++) {
        SimpleBuffer buffer;
        TsFrame lEsFrame;
        lEsFrame.data = std::make_shared<SimpleBuffer>();
        lEsFrame.data->append((const uint8_t *)&testVector[0], TEST_VECTOR_SIZE);
        lEsFrame.pts = pts;
        lEsFrame.dts = pts;
        lEsFrame.pcr = 0;
        lEsFrame.stream_type = TYPE_VIDEO;
        lEsFrame.stream_id = 224;
        lEsFrame.pid = VIDEO_PID;
        lEsFrame.expected_pes_packet_length = 0;
        lEsFrame.random_access_indicator = 1;
        lEsFrame.completed = true;
        mFrameInTransit = true;
        mpMuxer->encode(lEsFrame, gStreamPidMap, VIDEO_PID, buffer);
        muxOutputPts(buffer);
        if (mFrameInTransit) {
            std::cout << "Frame " << unsigned(x) << " not muxed/demuxed corectly" << std::endl;
            mUnitTestStatus = false;
        }
        pts += 1920;
    }

    //Send frames containing PTS / DTS
    for (int x = 1; x < NUM_FRAMES_LOOP; x++) {
        SimpleBuffer buffer;
        TsFrame lEsFrame;
        lEsFrame.data = std::make_shared<SimpleBuffer>();
        lEsFrame.data->append((const uint8_t *)&testVector[0], TEST_VECTOR_SIZE);
        lEsFrame.pts = pts;
        lEsFrame.dts = dts;
        lEsFrame.pcr = 0;
        lEsFrame.stream_type = TYPE_VIDEO;
        lEsFrame.stream_id = 224;
        lEsFrame.pid = VIDEO_PID;
        lEsFrame.expected_pes_packet_length = 0;
        lEsFrame.random_access_indicator = 1;
        lEsFrame.completed = true;
        mFrameInTransit = true;
        mpMuxer->encode(lEsFrame, gStreamPidMap, VIDEO_PID, buffer);
        muxOutputPtsDts(buffer);
        if (mFrameInTransit) {
            std::cout << "Frame " << unsigned(x) << " not muxed/demuxed corectly" << std::endl;
            mUnitTestStatus = false;
        }
        pts += 1111;
        dts += 2920;
    }


    //Send frames containing PTS / DTS then also send out of band PCR
    for (int x = 1; x < NUM_FRAMES_LOOP; x++) {
        SimpleBuffer buffer;
        TsFrame lEsFrame;
        lEsFrame.data = std::make_shared<SimpleBuffer>();
        lEsFrame.data->append((const uint8_t *)&testVector[0], TEST_VECTOR_SIZE);
        lEsFrame.pts = pts;
        lEsFrame.dts = dts;
        lEsFrame.pcr = 0;
        lEsFrame.stream_type = TYPE_VIDEO;
        lEsFrame.stream_id = 224;
        lEsFrame.pid = VIDEO_PID;
        lEsFrame.expected_pes_packet_length = 0;
        lEsFrame.random_access_indicator = 1;
        lEsFrame.completed = true;
        mFrameInTransit = true;
        mpMuxer->encode(lEsFrame, gStreamPidMap, VIDEO_PID, buffer);
        muxOutputPtsDtsPcr(buffer);
        buffer.clear();
        mpMuxer->create_pcr(buffer, pts+dts+pcr);
        muxOutputPtsDtsPcr(buffer);
        if (mFrameInTransit) {
            std::cout << "Frame " << unsigned(x) << " not muxed/demuxed corectly" << std::endl;
            mUnitTestStatus = false;
        }
        pts += 3920;
        dts += 1923;
        pcr += 1;
    }

    //Here we switch the PCR to be embedded into the data..

    //So we delete the old muxer and create a new one using the video pid as PCR pid
    //That means that the PCR will be taken from the Elementary stream data.
    delete mpMuxer;
    mpMuxer = new MpegTsMuxer;

    //Send frames containing PTS / DTS then also send in band PCR
    for (int x = 1; x < NUM_FRAMES_LOOP; x++) {
        SimpleBuffer buffer;
        TsFrame lEsFrame;
        lEsFrame.data = std::make_shared<SimpleBuffer>();
        lEsFrame.data->append((const uint8_t *)&testVector[0], TEST_VECTOR_SIZE);
        lEsFrame.pts = pts;
        lEsFrame.dts = dts;
        lEsFrame.pcr = pts+dts+pcr;
        lEsFrame.stream_type = TYPE_VIDEO;
        lEsFrame.stream_id = 224;
        lEsFrame.pid = VIDEO_PID;
        lEsFrame.expected_pes_packet_length = 0;
        lEsFrame.random_access_indicator = 1;
        lEsFrame.completed = true;
        mFrameInTransit = true;
        mpMuxer->encode(lEsFrame, gStreamPidMap, VIDEO_PID, buffer);
        muxOutputPtsDtsPcr(buffer);
        if (mFrameInTransit) {
            std::cout << "Frame " << unsigned(x) << " not muxed/demuxed corectly" << std::endl;
            mUnitTestStatus = false;
        }
        pts += 1234;
        dts += 4321;
        pcr += 1;
    }

    delete mpMuxer;
    return mUnitTestStatus; //True = OK
}