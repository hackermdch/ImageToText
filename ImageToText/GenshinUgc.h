#pragma once
#include <any>
#include <functional>
#include <string>
#include <memory>
#include <vector>

namespace Ugc
{
	enum class NodeType : uint8_t
	{
		Unknown,
		Int,
		Long,
		Float,
		Double,
		String,
		Object,
		Array,
		IntArray,
		Raw
	};

	struct INode
	{
		virtual uint32_t Id() const = 0;
		virtual std::string Name() const = 0;
		virtual NodeType Type() const = 0;
		virtual std::any Value() const = 0;
		virtual INode* Find(uint32_t id) const = 0;
		virtual INode* Find(const std::string& name) const = 0;
		virtual INode* Find(const std::function<bool(const INode*)>& predicate) const = 0;
		virtual std::vector<INode*> Children() const = 0;
		virtual std::string Dump() const = 0;
		virtual INode* CreateNode(uint32_t id, std::string name, NodeType type, std::any value) = 0;
		virtual INode* AddClone(INode* node) = 0;
		virtual void RemoveNode(INode* child) = 0;
		virtual void Id(uint32_t id) = 0;
		virtual void Name(const std::string& name) = 0;
		virtual void Type(NodeType type) = 0;
		virtual void Value(std::any value) = 0;
		virtual ~INode() = 0;

		virtual INode& Ref() const = 0;
		virtual INode& operator[](uint32_t id) = 0;
		virtual INode& operator[](const std::string& name) = 0;
		virtual uint32_t operator=(uint32_t value) = 0;
		virtual uint64_t operator=(uint64_t value) = 0;
		virtual float operator=(float value) = 0;
		virtual double operator=(double value) = 0;
		virtual const std::string& operator=(const std::string& value) = 0;
		virtual const std::vector<uint8_t>& operator=(const std::vector<uint8_t>& value) = 0;
		virtual const std::vector<uint64_t>& operator=(const std::vector<uint64_t>& value) = 0;
		virtual operator uint32_t() const = 0;
		virtual operator uint64_t() const = 0;
		virtual operator float() const = 0;
		virtual operator double() const = 0;
		virtual operator std::string() const = 0;
		virtual operator std::vector<uint8_t>() const = 0;
		virtual operator std::vector<uint64_t>() const = 0;

		template<NodeType T>
		auto Get() const
		{
			using enum NodeType;
			if constexpr (T == Unknown || T == Raw) return std::any_cast<std::vector<uint8_t>>(Value());
			else if constexpr (T == Int) return std::any_cast<uint32_t>(Value());
			else if constexpr (T == Long) return std::any_cast<uint64_t>(Value());
			else if constexpr (T == Float) return std::any_cast<float>(Value());
			else if constexpr (T == Double) return std::any_cast<double>(Value());
			else if constexpr (T == String) return std::any_cast<std::string>(Value());
			else if constexpr (T == IntArray) return std::any_cast<std::vector<uint64_t>>(Value());
			else static_assert(false, "Unsupported type");
		}

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

		INode& operator[](const char* name)
		{
			return operator[](std::string(name));
		}

		INode& With(const std::function<void(INode&)>& action)
		{
			action(*this);
			return *this;
		}

		INode() = delete;
		INode(const INode&) = delete;
		INode(INode&&) = delete;
		INode& operator=(const INode&) = delete;
		INode& operator=(INode&&) = delete;
	};

	std::unique_ptr<INode> Decode(std::istream& in);
	void Encode(const INode* root, std::ostream& out);
}
