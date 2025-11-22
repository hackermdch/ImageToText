import std;
import project;
import image;

struct Arguments
{
	std::filesystem::path inputFile;
	std::filesystem::path outputFile;
	std::filesystem::path imageFile;
	int resolutionX;
	int resolutionY;
	float posX;
	float posY;
	float height = 25;
	int grainSize = 5;
	unsigned layout = 1073741825;
};

void Help()
{
	std::println("Usage: ImageToHex <input file> <output file> <image file> <resolution> [extra options]");
	std::println("  <input file>   The .gil project file to read.");
	std::println("  <output file>  The .gil project file to write.");
	std::println("  <image file>   The image file to convert to text.");
	std::println("  <resolution>   The resolution of input image.");
	std::println("Extra options: ");
	std::println("  -p=x,y   Set screen location.");
	std::println("  -h=i   Set text box height.");
	std::println("  -g=i   Set pixel granularity.");
	std::println("  -l=guid   Set belonging layout.");
}

void Process(const Arguments& args)
{
	App::Project project(args.inputFile, args.layout);
	Image::Converter converter(args.imageFile, args.resolutionX, args.resolutionY, args.grainSize);
	while (converter.HasNext())
	{
		auto [row, col, text] = converter.NextBlock();
		project.CurrentCombination()->AddText(std::format("image_{}:{}", row, col), text, args.posX + col * args.grainSize, args.posY - row * args.grainSize, args.resolutionX, args.height);
	}
	project.Save(args.outputFile);
}

int main(int argc, char** argv)
{
	try
	{
#if _DEBUG
		Process({ "1.gil","out.gil","1.png",560,315,0,315 / 2.f });
#else
		if (argc < 5)
		{
			Help();
			return -1;
		}
		Arguments args{};
		args.inputFile = argv[1];
		args.outputFile = argv[2];
		args.imageFile = argv[3];
		{
			std::string res = argv[4];
			auto pos = res.find(',');
			if (pos == std::string::npos)
			{
				Help();
				return -1;
			}
			args.resolutionX = std::stoi(res.substr(0, pos));
			args.resolutionY = std::stoi(res.substr(pos + 1));
		}
		for (int i = 5; i < argc; i++)
		{
			if (std::string opt = argv[i]; opt.starts_with("-p="))
			{
				auto val = opt.substr(5);
				auto pos = val.find(',');
				if (pos == std::string::npos)
				{
					Help();
					return -1;
				}
				args.posX = std::stof(val.substr(0, pos));
				args.posY = std::stof(val.substr(pos + 1));
			}
			else if (opt.starts_with("-h=")) args.height = std::stof(opt.substr(3));
			else if (opt.starts_with("-g=")) args.grainSize = std::stoi(opt.substr(3));
			else if (opt.starts_with("-l=")) args.layout = std::stoul(opt.substr(3));
			else
			{
				Help();
				return -1;
			}
		}
		Process(std::move(args));
		std::println("Processed successfully");
#endif
	}
	catch (...)
	{
		std::println("Unknown error");
		return -1;
	}
}
