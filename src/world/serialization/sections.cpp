#include "sections.h"
#include "../../utils/utils.h"

#include <zlib.h>
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace TankGame
{
	void DeflateAndWrite(const char* data, size_t dataSize, std::ostream& output)
	{
		z_stream deflateStream = { };
		
		deflateStream.avail_in = dataSize;
		deflateStream.next_in = reinterpret_cast<const Bytef*>(data);
		
		if (deflateInit(&deflateStream, Z_DEFAULT_COMPRESSION) != Z_OK)
			Panic("Error initializing ZLIB.");
		
		char outBuffer[256];
		int status;
		
		//Deflates and writes the data 256 bytes at a time
		do
		{
			deflateStream.avail_out = sizeof(outBuffer);
			deflateStream.next_out = reinterpret_cast<Bytef*>(outBuffer);
			
			status = deflate(&deflateStream, Z_FINISH);
			assert(status != Z_STREAM_ERROR);
			
			int bytesDecompressed = sizeof(outBuffer) - deflateStream.avail_out;
			output.write(outBuffer, bytesDecompressed);
		} while (deflateStream.avail_out == 0);
		
		deflateEnd(&deflateStream);
	}
	
	void WriteSection(const char* data, size_t dataSize, std::ostream& stream)
	{
		uint64_t payloads[2] = { dataSize, 0 };
		stream.write(reinterpret_cast<const char*>(payloads), sizeof(payloads));
		
		auto dataBeginPos = stream.tellp();
		
		DeflateAndWrite(data, dataSize, stream);
		
		auto dataEndPos = stream.tellp();
		uint64_t compressedSize = dataEndPos - dataBeginPos;
		
		stream.seekp(dataBeginPos - static_cast<std::ostream::pos_type>(sizeof(uint64_t)));
		stream.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
		stream.seekp(dataEndPos);
	}
	
	std::vector<char> ReadSection(std::istream& input)
	{
		uint64_t sectionSize;
		uint64_t compressedSectionSize;
		
		input.read(reinterpret_cast<char*>(&sectionSize), sizeof(sectionSize));
		input.read(reinterpret_cast<char*>(&compressedSectionSize), sizeof(compressedSectionSize));
		
		std::vector<char> output;
		output.reserve(sectionSize);
		
		z_stream inflateStream = { };
		inflateInit(&inflateStream);
		
		int status;
		char outBuffer[256];
		char inBuffer[256];
		
		long bytesLeft = compressedSectionSize;
		
		//Inflates the data 256 bytes at a time
		do
		{
			long bytesToRead = std::min<long>(sizeof(inBuffer), bytesLeft);
			input.read(inBuffer, bytesToRead);
			
			assert(input.gcount() == bytesToRead);
			
			bytesLeft -= bytesToRead;
			
			inflateStream.avail_in = bytesToRead;
			inflateStream.next_in = reinterpret_cast<Bytef*>(inBuffer);
			
			if (inflateStream.avail_in == 0)
				break;
			
			do
			{
				inflateStream.avail_out = sizeof(outBuffer);
				inflateStream.next_out = reinterpret_cast<Bytef*>(outBuffer);
				
				status = inflate(&inflateStream, Z_NO_FLUSH);
				assert(status != Z_STREAM_ERROR);
				
				if (status == Z_MEM_ERROR)
					Panic("ZLib out of memory");
				if (status == Z_DATA_ERROR || status == Z_NEED_DICT)
					Panic("Invalid deflate stream.");
				
				int bytesCompressed = static_cast<int>(sizeof(outBuffer)) - inflateStream.avail_out;
				for (int i = 0; i < bytesCompressed; i++)
					output.push_back(outBuffer[i]);
			}
			while (inflateStream.avail_out == 0);
		}
		while (status != Z_STREAM_END && bytesLeft > 0);
		
		inflateEnd(&inflateStream);
		
		return output;
	}
}
