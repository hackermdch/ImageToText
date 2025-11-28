module;
#include "GenshinUgc.h"
#define DEBUG_DUMP 0
module project;

using namespace App;
using namespace Ugc;

using enum NodeType;

static auto empty_data = std::vector<uint8_t>();

Project::Project(const std::filesystem::path& path, uint32_t layout) : guid0(1073741839), layout(layout)
{
	std::ifstream in(path, std::ios::binary);
	root = Decode(in);
	for (auto w : UIWidgets().Children()) guids.insert(*w->Find("guid"));
}

void Project::AddText(std::string name, std::string content, float x, float y, float width, float height, Origin origin, HorizontalAlign ha, VerticalAlign va)
{
	appends.emplace_back(std::make_unique<TextBox>(NextGuid(), layout, name, content, x, y, width, height, origin, ha, va));
}

uint32_t Project::NextGuid()
{
	auto guid = guid0;
	for (; guids.contains(guid); ++guid)(void)0;
	guid0 = guid + 1;
	guids.insert(guid);
	return guid;
}

Combination* Project::CurrentCombination()
{
	if (!current) return current = AddCombination("com0");
	if (current->Full()) return current = AddCombination(std::format("com{}", appends.size()));
	return current;
}

Combination* Project::AddCombination(const std::string& name)
{
	auto& c = appends.emplace_back(std::make_unique<Combination>(NextGuid(), layout, name, *this));
	return (Combination*)c.get();
}

void Project::Save(const std::filesystem::path& path)
{
	auto& widgets = UIWidgets();
	auto l = widgets.Find([this](const INode* node) { return (uint32_t)node->Ref()["guid"] == layout; });
	auto children = l->Find("children")->Get<IntArray>();
	for (auto& w : appends)
	{
		w->Create(widgets);
		children.emplace_back(w->Guid());
	}
	l->Ref()["children"] = children;
	std::ofstream out(path, std::ios::binary);
	Encode(root.get(), out);
#if DEBUG_DUMP
	auto str = root->Dump();
	std::ofstream dump("dump.json");
	dump << root->Dump();
#endif
}

INode& Project::UIWidgets() const
{
	return root->Ref()[4]["ui_layout"]["widgets"];
}

static void BaseData(INode& node, uint32_t guid, uint32_t parent)
{
	node["guid"] = guid;
	node["info"][-1].With([guid](INode& node)
		{
			node[11]["guid"] = guid;
			node[501] = 1u;
			node[502] = 5u;
		}
	);
	node["parent"] = parent;
}

static void BaseData(INode& node, const std::string& name)
{
	node[-1].With([name](INode& node)
		{
			node[12]["name"] = name;
			node[501] = 2u;
			node[502] = 15u;
		}
	);
	node[-1].With([](INode& node)
		{
			node[14][15] = empty_data;
			node[14][501] = 5u;
			node[501] = 4u;
			node[502] = 23u;
			node["content"].With([](INode& node)
				{
					node[14][15] = empty_data;
					node[14][501] = 5u;
					node[501] = 5u;
					node[502] = 23u;
					node[503] = 1u;
				}
			);
		}
	);
}

TextBox::TextBox(uint32_t guid, uint32_t parent, std::string name, std::string content, float x, float y, float width, float height, Origin origin, HorizontalAlign ha, VerticalAlign va) : guid(guid), parent(parent), name(std::move(name)), content(std::move(content)), x(x), y(y), width(width), height(height), origin(origin), hAlign(ha), vAlign(va)
{
}

void TextBox::Create(INode& widgets)
{
	auto& obj = widgets[-1];
	BaseData(obj, guid, parent);
	obj["data"].With([this](INode& node)
		{
			BaseData(node, name);
			node[-1].With([this](INode& node)
				{
					node[11][12] = empty_data;
					node[11][501] = 2u;
					node[501] = 1u;
					node[502] = 12u;
					node["content"].With([this](INode& node)
						{
							node["frame"][501] = 2u;
							node["frame"][12][502] = 3u;
							node["frame"]["multi_platform"]["devices"].With([this](INode& node)
								{
									for (uint32_t i = 0; i < 4; i++)
									{
										auto& n = node[-1];
										if (i != 0) n[501] = i;
										n["ui_rect"].With([this](INode& node)
											{
												node[501][1] = 1.0f;
												node[501][2] = 1.0f;
												node[501][3] = 1.0f;
												node["anchor"]["nx"] = .5f;
												node["anchor"]["ny"] = .5f;
												node["origin"]["nx"] = .5f;
												node["origin"]["ny"] = .5f;
												node["center"]["nx"] = .5f;
												node["center"]["ny"] = .5f;
												node["position"]["x"] = x;
												node["position"]["y"] = y;
												node["size"]["width"] = width;
												node["size"]["height"] = height;
											}
										);
									}
								}
							);
							node[501] = 4u;
							node[502] = 12u;
							node[503] = 1u;
						}
					);
				}
			);
			node[-1].With([this](INode& node)
				{
					node["text_box"] = empty_data;
					node[501] = 9u;
					node[502] = 25u;
					node["content"].With([this](INode& node)
						{
							node["text_box"]["size"] = 10u;
							node["text_box"]["content"]["text"] = content;
							node[501] = 10u;
							node[502] = 25u;
							node[503] = 1u;
						}
					);
				}
			);
		}
	);
}

Combination::Combination(uint32_t guid, uint32_t layout, const std::string& name, Project& project) : guid(guid), layout(layout), name(name), project(project)
{
}

void Combination::AddText(std::string name, std::string content, float x, float y, float width, float height, Origin origin, HorizontalAlign ha, VerticalAlign va)
{
	texts.emplace_back(std::make_unique<TextBox>(project.NextGuid(), guid, name, content, x, y, width, height, origin, ha, va));
}

void Combination::Create(INode& widgets)
{
	std::vector<uint64_t> children;
	for (auto& text : texts)
	{
		text->Create(widgets);
		children.emplace_back(text->Guid());
	}

	auto& obj = widgets[-1];
	BaseData(obj, guid, layout);
	obj["info"][-1].With([this](INode& node)
		{
			node[16] = empty_data;
			node[501] = 6u;
			node[502] = 13u;
			node[503].With([this](INode& node)
				{
					node[15] = empty_data;
					node[501] = 6u;
					node[502] = 13u;
					node[503] = 1u;
				}
			);
		}
	);
	obj["children"] = children;
	BaseData(obj["data"], name);
}
