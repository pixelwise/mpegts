#include <iostream>
#include <memory>
#include <fstream>
#include "mpegts_muxer.h"
#include "mpegts_demuxer.h"
#include "simple_buffer.h"
#include "ts_packet.h"

std::map<uint16_t, std::ofstream*> file_map;

void write_file(const TsFrame *frame)
{
    if (!frame)
        return;

    if (file_map.find(frame->pid) == file_map.end()) {
        std::string ext = ".unknown";
        if (frame->stream_type == 0x0f) {
            ext = ".aac";
        } else if (frame->stream_type == 0x1b) {
            ext = ".264";
        }
        file_map[frame->pid] = new std::ofstream(std::to_string(frame->pid) + ext, std::ios::binary);
    }

    file_map[frame->pid]->write(frame->data->data(), frame->data->size());
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <in>.ts <out>.ts" << std::endl;
        return 0;
    }

    std::string in_file_name = argv[1];
    std::string out_file_name = argv[2];

    std::cout << "input ts: " << in_file_name << std::endl;
    std::cout << "output ts: " << out_file_name << std::endl;

    std::ifstream ifile(in_file_name, std::ios::binary | std::ios::in);
    std::ofstream outts(out_file_name, std::ios::binary);

    std::shared_ptr<MpegTsMuxer> muxer(new MpegTsMuxer);

    MpegTsDemuxer demuxer;
    char packet[188] = { 0 };

    SimpleBuffer in;
    SimpleBuffer out;

    while (!ifile.eof()) {
        ifile.read(packet, 188);
        in.append(packet, 188);

        auto frame = demuxer.decode(in);

        write_file(frame.get());
        if (frame) {
            muxer->encode(*frame, demuxer.stream_pid_map, demuxer.pmt_id, out);
            outts.write(out.data(), out.size());
            out.clear();
        }
    }

    ifile.close();
    for (auto it = file_map.begin(); it != file_map.end(); it++) {
        it->second->close();
    }

    return 0;
}
