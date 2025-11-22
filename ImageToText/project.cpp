module;
#include "GenshinUgc.h"
#define DEBUG_DUMP 0
module project;

using namespace App;
using namespace Ugc;

static auto empty_data = std::vector<uint8_t>();

static uint32_t GetGuid(INode* widget)
{
	return widget->Find(501)->Children()[0]->Get<uint32_t>();
}

static INode* With(INode* node, const std::function<void(INode*)>& action)
{
	action(node);
	return node;
}

static INode* FindNode(const std::vector<INode*>& array, const std::function<bool(INode*)>& predicate)
{
	for (auto node : array) if (predicate(node)) return node;
	return nullptr;
}

struct VarInt
{
	uint8_t data[8]{};
	size_t size = 0;
};

static VarInt ToVarInt(uint32_t value) noexcept
{
	VarInt r{};
	for (; r.size == 0 || (value && r.size < 8); ++r.size)
	{
		uint8_t v = value & 0x7F;
		value >>= 7;
		if (value) v |= 0x80;
		r.data[r.size] = v;
	}
	return r;
}

Project::Project(const std::filesystem::path& path, uint32_t layout) : guid0(1073741839), layout(layout)
{
	std::ifstream in(path, std::ios::binary);
	root = Decode(in);
	for (auto w : UIWidgets()->Children()) guids.insert(GetGuid(w));
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
	if (appends.empty()) return AddCombination("com0");
	auto& c = appends.back();
	if (c->Full()) return AddCombination(std::format("com{}", appends.size()));
	return c.get();
}

Combination* Project::AddCombination(const std::string& name)
{
	return appends.emplace_back(std::make_unique<Combination>(NextGuid(), layout, name, *this)).get();
}

void Project::Save(const std::filesystem::path& path)
{
	auto widgets = UIWidgets();
	auto l = FindNode(widgets->Children(), [this](INode* node) { return GetGuid(node) == layout; });
	auto arr = l->Find(503)->Children()[0];
	auto data = arr->Get<std::vector<uint8_t>>();
	std::vector<VarInt> datas;

	for (auto& com : appends)
	{
		com->Create(widgets);
		datas.emplace_back(ToVarInt(com->guid));
	}
	size_t total_size = 0;
	for (auto& [value, size] : datas) total_size += size;

	auto ptr = data.size();
	data.resize(data.size() + total_size);
	for (auto& [value, size] : datas)
	{
		memcpy(data.data() + ptr, value, size);
		ptr += size;
	}
	arr->Value(data);

	std::ofstream out(path, std::ios::binary);
	Encode(root.get(), out);
#if DEBUG_DUMP
	auto str = root->Dump();
	std::ofstream dump("dump.json");
	dump << root->Dump();
#endif
}

INode* Project::UIWidgets()
{
	return root->Find(4)->Find(9)->Find(502);
}

static void BaseData(INode* node, uint32_t guid, uint32_t parent)
{
	node->CreateNode(501, true)->CreateNode(501, guid);
	With(node->CreateNode(502, true)->CreateNode(502), [guid](INode* node)
		{
			node->CreateNode(11)->CreateNode(501, guid);
			node->CreateNode(501, 1u);
			node->CreateNode(502, 5u);
		}
	);
	node->CreateNode(504, parent);
}

static void BaseData(INode* node, const std::string& name)
{
	With(node->CreateNode(505), [name](INode* node)
		{
			node->CreateNode(12)->CreateNode(501, name);
			node->CreateNode(501, 2u);
			node->CreateNode(502, 15u);
		}
	);
	With(node->CreateNode(505), [](INode* node)
		{
			With(node->CreateNode(14), [](INode* node)
				{
					node->CreateNode(15, empty_data);
					node->CreateNode(501, 5u);
				}
			);
			node->CreateNode(501, 4u);
			node->CreateNode(502, 23u);
			With(node->CreateNode(503), [](INode* node)
				{
					With(node->CreateNode(14), [](INode* node)
						{
							node->CreateNode(15, empty_data);
							node->CreateNode(501, 5u);
						}
					);
					node->CreateNode(501, 5u);
					node->CreateNode(502, 23u);
					node->CreateNode(503, 1u);
				}
			);
		}
	);
}

TextBox::TextBox(uint32_t parent, std::string name, std::string content, float x, float y, float width, float height, Origin origin, HorizontalAlign ha, VerticalAlign va) : guid(0), parent(parent), name(std::move(name)), content(std::move(content)), x(x), y(y), width(width), height(height), origin(origin), hAlign(ha), vAlign(va)
{
}

void TextBox::Create(INode* widgets)
{
	auto obj = widgets->CreateNode(502);
	BaseData(obj, guid, parent);
	With(obj->CreateNode(505, true), [this](INode* node)
		{
			BaseData(node, name);
			With(node->CreateNode(505), [this](INode* node)
				{
					With(node->CreateNode(11), [this](INode* node)
						{
							node->CreateNode(12, empty_data);
							node->CreateNode(501, 2u);
						}
					);
					node->CreateNode(501, 1u);
					node->CreateNode(502, 12u);
					With(node->CreateNode(503), [this](INode* node)
						{
							With(node->CreateNode(13), [this](INode* node)
								{
									With(node->CreateNode(12), [this](INode* node)
										{
											With(node->CreateNode(501, true), [this](INode* node)
												{
													for (uint32_t i = 0; i < 4; i++)
													{
														With(node->CreateNode(501), [this, i](INode* node)
															{
																if (i != 0) node->CreateNode(501, i);
																With(node->CreateNode(502), [this](INode* node)
																	{
																		With(node->CreateNode(501), [this](INode* node)
																			{
																				node->CreateNode(1, 1.0f);
																				node->CreateNode(2, 1.0f);
																				node->CreateNode(3, 1.0f);
																			}
																		);
																		With(node->CreateNode(502), [this](INode* node)
																			{
																				node->CreateNode(501, .5f);
																				node->CreateNode(502, .5f);
																			}
																		);
																		With(node->CreateNode(503), [this](INode* node)
																			{
																				node->CreateNode(501, .5f);
																				node->CreateNode(502, .5f);
																			}
																		);
																		With(node->CreateNode(504), [this](INode* node)
																			{
																				node->CreateNode(501, x);
																				node->CreateNode(502, y);
																			}
																		);
																		With(node->CreateNode(505), [this](INode* node)
																			{
																				node->CreateNode(501, width);
																				node->CreateNode(502, height);
																			}
																		);
																		With(node->CreateNode(506), [this](INode* node)
																			{
																				node->CreateNode(501, .5f);
																				node->CreateNode(502, .5f);
																			}
																		);
																	}
																);
															}
														);
													}
												}
											);
											node->CreateNode(502, 3u);
										}
									);
									node->CreateNode(501, 2u);
								}
							);
							node->CreateNode(501, 4u);
							node->CreateNode(502, 12u);
							node->CreateNode(503, 1u);
						}
					);
				}
			);
			With(node->CreateNode(505), [this](INode* node)
				{
					node->CreateNode(19, empty_data);
					node->CreateNode(501, 9u);
					node->CreateNode(502, 25u);
					With(node->CreateNode(503), [this](INode* node)
						{
							With(node->CreateNode(19), [this](INode* node)
								{
									node->CreateNode(502, 10u);
									node->CreateNode(505)->CreateNode(501, content);
								}
							);
							node->CreateNode(501, 10u);
							node->CreateNode(502, 25u);
							node->CreateNode(503, 1u);
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
	auto text = texts.emplace_back(std::make_unique<TextBox>(guid, name, content, x, y, width, height, origin, ha, va)).get();
	text->guid = project.NextGuid();
}

void Combination::Create(INode* widgets)
{
	std::vector<VarInt> datas;
	for (auto& text : texts)
	{
		text->Create(widgets);
		datas.emplace_back(ToVarInt(text->guid));
	}
	size_t total_size = 0;
	for (auto& [value, size] : datas) total_size += size;
	std::vector<uint8_t> data(total_size);
	uint32_t ptr = 0;
	for (auto& [value, size] : datas)
	{
		memcpy(data.data() + ptr, value, size);
		ptr += size;
	}

	auto obj = widgets->CreateNode(502);
	BaseData(obj, guid, layout);
	With(obj->Find(502)->CreateNode(502), [this](INode* node)
		{
			node->CreateNode(16, empty_data);
			node->CreateNode(501, 6u);
			node->CreateNode(502, 13u);
			With(node->CreateNode(503), [this](INode* node)
				{
					node->CreateNode(15, empty_data);
					node->CreateNode(501, 6u);
					node->CreateNode(502, 13u);
					node->CreateNode(503, 1u);
				}
			);
		}
	);
	obj->CreateNode(503, true)->CreateNode(503, data);
	BaseData(obj->CreateNode(505, true), name);
}
