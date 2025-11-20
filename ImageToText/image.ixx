module;
#include <cstdint>
export module image;

import std;

export namespace Image
{
	struct Block
	{
		int row = 0, col = 0;
		std::string text;
	};

	class Converter
	{
		uint8_t* image;
		const int width, height;
		const int grain_size;
		int row = 0, col = 0;
	public:

		Converter(const std::filesystem::path& path, int width, int height, int grainSize);
		~Converter();
		Block NextBlock();
		bool HasNext() const;
	};
}
