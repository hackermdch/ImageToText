module;
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <cstdint>
#include "stb_image.h"
#include "stb_image_resize2.h"
module image;

using namespace Image;

static void WriteHex(uint8_t byte, std::ostringstream& ss)
{
	static constexpr char chars[] = "0123456789ABCDEF";
	ss << chars[byte >> 4 & 0x0F];
	ss << chars[byte & 0x0F];
}

Converter::Converter(const std::filesystem::path& path, int width, int height, int grainSize) : width(width / grainSize), height(height / grainSize), grain_size(grainSize)
{
	std::ifstream in(path, std::ios::binary);
	std::vector<uint8_t> data(std::filesystem::file_size(path));
	in.read((char*)data.data(), data.size());
	auto img = stbi_load_from_memory(data.data(), data.size(), &width, &height, &grainSize, 4);
	image = new uint8_t[this->width * this->height * 4];
	stbir_resize_uint8_linear(img, width, height, width * 4, image, this->width, this->height, this->width * 4, STBIR_RGBA);
	stbi_image_free(img);
}

Converter::~Converter()
{
	delete[] image;
}

Block Converter::NextBlock()
{
	std::ostringstream ss;
	ss << "<size=" << grain_size << ">";
	int lastColor = 0;
	Block block;
	block.col = col;
	block.row = row;
	for (auto i = 0; i < 40; i++)
	{
		auto p = (row * width + col) * 4;
		if (auto color = *(int*)(image + p); color != lastColor || i == 0)
		{
			lastColor = color;
			if (i != 0) ss << "</color>";
			ss << "<color=#";
			WriteHex(image[p], ss);
			WriteHex(image[p + 1], ss);
			WriteHex(image[p + 2], ss);
			if (image[p + 3] != 0xff) WriteHex(image[p + 3], ss);
			ss << '>';
		}
		ss << (char*)u8"¨€";
		if (++col >= width)
		{
			col = 0;
			++row;
			break;
		}
	}
	ss << "</color></size>";
	block.text = ss.str();
	return block;
}

bool Converter::HasNext() const
{
	return row < height;
}
