module;
#include "GenshinUgc.h"
export module project;

import std;

export namespace App
{
	class Project;
	class Combination;

	enum class Origin : uint8_t
	{
		Center,
		Left,
		Right,
		Top,
		Bottom,
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom
	};

	enum class HorizontalAlign
	{
		Left,
		Center,
		Right
	};

	enum class VerticalAlign
	{
		Top,
		Center,
		Bottom
	};

	class IWidget
	{
	public:
		virtual void Create(Ugc::INode& widgets) = 0;
		virtual uint32_t Guid() const = 0;
		virtual ~IWidget() = default;
	};

	class TextBox : public IWidget
	{
		uint32_t guid;
	public:
		uint32_t parent;
		std::string name;
		std::string content;
		float x, y;
		float width, height;
		Origin origin; // not implemented
		HorizontalAlign hAlign; // not implemented
		VerticalAlign vAlign; // not implemented

		TextBox(uint32_t guid, uint32_t parent, std::string name, std::string content, float x = 0, float y = 0, float width = 100, float height = 40, Origin origin = Origin::Center, HorizontalAlign ha = HorizontalAlign::Left, VerticalAlign va = VerticalAlign::Bottom);
		void Create(Ugc::INode& widgets) override;
		uint32_t Guid() const override { return guid; }
	};

	class Combination : public IWidget
	{
		uint32_t guid;
		uint32_t layout;
		Project& project;
		std::string name;
		std::vector<std::unique_ptr<TextBox>> texts;
	public:
		Combination(uint32_t guid, uint32_t layout, const std::string& name, Project& project);
		void AddText(std::string name, std::string content, float x = 0, float y = 0, float width = 100, float height = 40, Origin origin = Origin::Center, HorizontalAlign ha = HorizontalAlign::Left, VerticalAlign va = VerticalAlign::Bottom);
		void Create(Ugc::INode& widgets) override;
		bool Full() const { return texts.size() >= 19; }
		uint32_t Guid() const override { return guid; }
	};

	class Project
	{
		std::unique_ptr<Ugc::INode> root;
		std::vector<std::unique_ptr<IWidget>> appends;
		Combination* current;
		std::unordered_set<uint32_t> guids;
		uint32_t guid0, layout;

		Ugc::INode& UIWidgets() const;
		Combination* AddCombination(const std::string& name);
	public:
		explicit Project(const std::filesystem::path& path, uint32_t layout);
		void AddText(std::string name, std::string content, float x = 0, float y = 0, float width = 100, float height = 40, Origin origin = Origin::Center, HorizontalAlign ha = HorizontalAlign::Left, VerticalAlign va = VerticalAlign::Bottom);
		uint32_t NextGuid();
		Combination* CurrentCombination();
		void Save(const std::filesystem::path& path);
	};
}
