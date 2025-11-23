#pragma once
#include <any>
#include <string>
#include <memory>
#include <vector>

namespace Ugc
{
	enum class NodeType : uint8_t
	{
		Unknown,
		Int,
		Float,
		String,
		Object,
		Array,
		Raw
	};

	struct INode
	{
		virtual uint32_t Id() = 0;
		virtual std::string Name() = 0;
		virtual NodeType Type() = 0;
		virtual std::any Value() = 0;
		virtual INode* Find(uint32_t id) = 0;
		virtual std::vector<INode*> Children() = 0;
		virtual std::string Dump() = 0;
		virtual INode* CreateNode(uint32_t id, std::string name, NodeType type, std::any value) = 0;
		virtual INode* AddClone(INode* node) = 0;
		virtual void RemoveNode(INode* child) = 0;
		virtual void Id(uint32_t id) = 0;
		virtual void Name(const std::string& name) = 0;
		virtual void Type(NodeType type) = 0;
		virtual void Value(std::any value) = 0;
		virtual ~INode() = 0;

		template<typename T>
		T Get() { return std::any_cast<T>(Value()); }

		INode* CreateNode(uint32_t id, uint32_t value, const std::string& name = "")
		{
			return CreateNode(id, name, NodeType::Int, value);
		}

		INode* CreateNode(uint32_t id, float value, const std::string& name = "")
		{
			return CreateNode(id, name, NodeType::Float, value);
		}

		INode* CreateNode(uint32_t id, const std::string& value, const std::string& name = "")
		{
			return CreateNode(id, name, NodeType::String, value);
		}

		INode* CreateNode(uint32_t id, std::vector<uint8_t> value, const std::string& name = "")
		{
			return CreateNode(id, name, NodeType::Raw, std::move(value));
		}

		INode* CreateNode(uint32_t id, bool array = false, const std::string& name = "")
		{
			if (array) return CreateNode(id, name, NodeType::Array, {});
			return CreateNode(id, name, NodeType::Object, {});
		}

		INode() = delete;
		INode(const INode&) = delete;
		INode(INode&&) = delete;
		INode& operator=(const INode&) = delete;
		INode& operator=(INode&&) = delete;
	};

	std::unique_ptr<INode> Decode(std::istream& in);
	void Encode(INode* root, std::ostream& out);
}
