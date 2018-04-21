#include <iostream>
#include <fstream>
#include <cstdio>
#include <cinttypes>

struct FileHeader {
	char chunkType[4];		// MThd
	char headerLengh[4];	// Always be 6
	char midiFormat[2];		// type will be 0, 1, or 2
	char midiTrackCnt[2];
	char midiDeltaTime[2];
};

struct TrackHeader {
	char chunkType[4];		// MTrk
	char trackLengh[4];
};

uint16_t byte2_to_uint16(const char* buffer) {
    return ((uint16_t)(buffer[0]) << 8) | (uint16_t)(buffer[1]);
}

uint32_t byte4_to_uint32(const char* buffer) {
    return ((unsigned char)buffer[0] << 24) | ((unsigned char)buffer[1] << 16) |
        ((unsigned char)buffer[2] << 8) | (unsigned char)buffer[3];
}

int main(int argc, char *argv[]) {

	std::cout << "Midi Rip:" << std::endl;

	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <filepath>" << std::endl;
		std::cout << "Simply drag the blob file onto the program also works." << std::endl;
		return 0;
	}

	std::ifstream blobFile(argv[1], std::ios::binary);
	if (!blobFile.is_open()) {
		std::cout<< "[Error] The input blob file '" << argv[1] << "' seems not readable." << std::endl;
	}
	
	char curByte;
	char fileName[25];
	std::string rawMThd(14, 'M');
	std::string rawMTrk(8, 'M');
	struct FileHeader MThd;
	struct TrackHeader MTrk;
	int fileCount = 0;
	
	while (blobFile.get(curByte)) {
		if (curByte == 'M') {
			// Check if we got a SMF file header
			blobFile.read(&rawMThd[1], 13);
			if (!blobFile) break;
			if (rawMThd.substr(0, 4) != "MThd") {
				blobFile.seekg(-13, std::ios_base::cur);
				continue;
			} else {
				memcpy(&MThd, rawMThd.c_str(), 14);
				if (byte4_to_uint32(MThd.headerLengh) != 6) {
					blobFile.seekg(-13, std::ios_base::cur);
					continue;
				}
				std::cout<< "[Info ] New midi file founded at pos " << blobFile.tellg() << " ." << std::endl;
				fileCount++;
				snprintf(fileName, 25, "midirip_extract_%04d.mid", fileCount);
				std::ofstream midiFile(fileName);
				if (midiFile.is_open()) {
					std::cout<< "[Info ] \t Creating file: '" << fileName << "' ." <<  std::endl;
				} else {
					std::cout<< "[Error] Unable to create file: '" << argv[1] << "' . Aborted." << std::endl;
					return 0;
				}
				
				midiFile.write(rawMThd.c_str(), 14);
				int trackCount = byte2_to_uint16(MThd.midiTrackCnt);
				for (int i = 1; i <= trackCount; i++) {
					std::cout<< "[Info ] \t Processing track " << i << " of " << trackCount << " ..." << std::endl;
					blobFile.read(&rawMTrk[0], 8);
					if (rawMTrk.substr(0, 4) != "MTrk") {
						std::cout<< "[Error] \t Track header not correct, closing file..." << std::endl;
						break;
					}
					memcpy(&MTrk, rawMTrk.c_str(), 8);
					midiFile.write(rawMTrk.c_str(), 8);
					uint32_t payloadSize = byte4_to_uint32(MTrk.trackLengh);
					std::string payload;
					payload.reserve(payloadSize);
					blobFile.read(&payload[0], payloadSize);
					midiFile.write(payload.c_str(), payloadSize);
				}
				midiFile.close();
				std::cout<< "[Info ] \t File write done!" <<  std::endl;
			}
		}
	}
	blobFile.close();
	
	std::cout<< "[Info ] Your file '" << argv[1] << "' Process done." << std::endl;
	
	return 0;
}






