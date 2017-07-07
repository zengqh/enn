//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2014/04/18
// File: enn_xml_element.cpp
//

#include "enn_xml_element.h"
#include <pugixml.hpp>
#include "enn_xml_file.h"
#include "enn_string_util.h"

namespace enn
{

const XMLElement XMLElement::EMPTY;

XMLElement::XMLElement() :
	node_(0),
	xpathResultSet_(0),
	xpathNode_(0),
	xpathResultIndex_(0)
{
}

XMLElement::XMLElement(XMLFile* file, pugi::xml_node_struct* node) :
	file_(file),
	node_(node),
	xpathResultSet_(0),
	xpathNode_(0),
	xpathResultIndex_(0)
{
}

XMLElement::XMLElement(XMLFile* file, const XPathResultSet* resultSet, const pugi::xpath_node* xpathNode, unsigned xpathResultIndex) :
	file_(file),
	node_(0),
	xpathResultSet_(resultSet),
	xpathNode_(resultSet ? xpathNode : (xpathNode ? new pugi::xpath_node(*xpathNode) : 0)),
	xpathResultIndex_(xpathResultIndex)
{
}

XMLElement::XMLElement(const XMLElement& rhs) :
	file_(rhs.file_),
	node_(rhs.node_),
	xpathResultSet_(rhs.xpathResultSet_),
	xpathNode_(rhs.xpathResultSet_ ? rhs.xpathNode_ : (rhs.xpathNode_ ? new pugi::xpath_node(*rhs.xpathNode_) : 0)),
	xpathResultIndex_(rhs.xpathResultIndex_)
{
}

XMLElement::~XMLElement()
{
	// XMLElement class takes the ownership of a single xpath_node object, so destruct it now
	if (!xpathResultSet_ && xpathNode_)
	{
		delete xpathNode_;
		xpathNode_ = 0;
	}
}

XMLElement& XMLElement::operator = (const XMLElement& rhs)
{
	file_ = rhs.file_;
	node_ = rhs.node_;
	xpathResultSet_ = rhs.xpathResultSet_;
	xpathNode_ = rhs.xpathResultSet_ ? rhs.xpathNode_ : (rhs.xpathNode_ ? new pugi::xpath_node(*rhs.xpathNode_) : 0);
	xpathResultIndex_ = rhs.xpathResultIndex_;
	return *this;
}

XMLElement XMLElement::CreateChild(const String& name)
{
	return CreateChild(name.c_str());
}

XMLElement XMLElement::CreateChild(const char* name)
{
	if (!file_ || (!node_ && !xpathNode_))
		return XMLElement();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	pugi::xml_node child = const_cast<pugi::xml_node&>(node).append_child(name);
	return XMLElement(file_, child.internal_object());
}

bool XMLElement::RemoveChild(const XMLElement& element)
{
	if (!element.file_ || (!element.node_ && !element.xpathNode_) || !file_ || (!node_ && !xpathNode_))
		return false;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	const pugi::xml_node& child = element.xpathNode_ ? element.xpathNode_->node() : pugi::xml_node(element.node_);
	return const_cast<pugi::xml_node&>(node).remove_child(child);
}

bool XMLElement::RemoveChild(const String& name)
{
	return RemoveChild(name.c_str());
}

bool XMLElement::RemoveChild(const char* name)
{
	if (!file_ || (!node_ && !xpathNode_))
		return false;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return const_cast<pugi::xml_node&>(node).remove_child(name);
}

bool XMLElement::RemoveChildren(const String& name)
{
	return RemoveChildren(name.c_str());
}

bool XMLElement::RemoveChildren(const char* name)
{
	if ((!file_ || !node_) && !xpathNode_)
		return false;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	if (!strlen(name))
	{
		for (;;)
		{
			pugi::xml_node child = node.last_child();
			if (child.empty())
				break;
			const_cast<pugi::xml_node&>(node).remove_child(child);
		}
	}
	else
	{
		for (;;)
		{
			pugi::xml_node child = node.child(name);
			if (child.empty())
				break;
			const_cast<pugi::xml_node&>(node).remove_child(child);
		}
	}

	return true;
}

bool XMLElement::RemoveAttribute(const String& name)
{
	return RemoveAttribute(name.c_str());
}

bool XMLElement::RemoveAttribute(const char* name)
{
	if (!file_ || (!node_ && !xpathNode_))
		return false;

	// If xpath_node contains just attribute, remove it regardless of the specified name
	if (xpathNode_ && xpathNode_->attribute())
		return xpathNode_->parent().remove_attribute(xpathNode_->attribute());  // In attribute context, xpath_node's parent is the parent node of the attribute itself

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return const_cast<pugi::xml_node&>(node).remove_attribute(node.attribute(name));
}

XMLElement XMLElement::SelectSingle(const String& query, pugi::xpath_variable_set* variables) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return XMLElement();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	pugi::xpath_node result = node.select_single_node(query.c_str(), variables);
	return XMLElement(file_, 0, &result, 0);
}

XMLElement XMLElement::SelectSinglePrepared(const XPathQuery& query) const
{
	if (!file_ || (!node_ && !xpathNode_ && !query.GetXPathQuery()))
		return XMLElement();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	pugi::xpath_node result = node.select_single_node(*query.GetXPathQuery());
	return XMLElement(file_, 0, &result, 0);
}

XPathResultSet XMLElement::Select(const String& query, pugi::xpath_variable_set* variables) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return XPathResultSet();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	pugi::xpath_node_set result = node.select_nodes(query.c_str(), variables);
	return XPathResultSet(file_, &result);
}

XPathResultSet XMLElement::SelectPrepared(const XPathQuery& query) const
{
	if (!file_ || (!node_ && !xpathNode_ && query.GetXPathQuery()))
		return XPathResultSet();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	pugi::xpath_node_set result = node.select_nodes(*query.GetXPathQuery());
	return XPathResultSet(file_, &result);
}

bool XMLElement::SetValue(const String& value)
{
	return SetValue(value.c_str());
}

bool XMLElement::SetValue(const char* value)
{
	if (!file_ || (!node_ && !xpathNode_))
		return false;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return const_cast<pugi::xml_node&>(node).append_child(pugi::node_pcdata).set_value(value);
}

bool XMLElement::SetAttribute(const String& name, const String& value)
{
	return SetAttribute(name.c_str(), value.c_str());
}

bool XMLElement::SetAttribute(const char* name, const char* value)
{
	if (!file_ || (!node_ && !xpathNode_))
		return false;

	// If xpath_node contains just attribute, set its value regardless of the specified name
	if (xpathNode_ && xpathNode_->attribute())
		return xpathNode_->attribute().set_value(value);

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	pugi::xml_attribute attr = node.attribute(name);
	if (attr.empty())
		attr = const_cast<pugi::xml_node&>(node).append_attribute(name);
	return attr.set_value(value);
}

bool XMLElement::SetAttribute(const String& value)
{
	return SetAttribute(value.c_str());
}

bool XMLElement::SetAttribute(const char* value)
{
	// If xpath_node contains just attribute, set its value
	return xpathNode_ && xpathNode_->attribute() && xpathNode_->attribute().set_value(value);
}

bool XMLElement::SetBool(const String& name, bool value)
{
	return SetAttribute(name, to_string(value));
}

bool XMLElement::SetAxisAlignedBox(const AxisAlignedBox& value)
{
	if (!SetVector3("min", value.getMinimum()))
		return false;
	return SetVector3("max", value.getMaximum());
}

bool XMLElement::SetBuffer(const String& name, const void* data, unsigned size)
{
	String dataStr;
	BufferToString(dataStr, data, size);
	return SetAttribute(name, dataStr);
}

bool XMLElement::SetBuffer(const String& name, const vector<unsigned char>::type& value)
{
	if (!value.size())
		return SetAttribute(name, "");
	else
		return SetBuffer(name, &value[0], value.size());
}

bool XMLElement::SetColor(const String& name, const Color& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetFloat(const String& name, float value)
{
	return SetAttribute(name, to_string(value));
}

bool XMLElement::SetUInt(const String& name, unsigned value)
{
	return SetAttribute(name, to_string(value));
}

bool XMLElement::SetInt(const String& name, int value)
{
	return SetAttribute(name, to_string(value));
}

bool XMLElement::SetIntRect(const String& name, const IntRect& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetIntVector2(const String& name, const IntVector2& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetQuaternion(const String& name, const Quaternion& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetString(const String& name, const String& value)
{
	return SetAttribute(name, value);
}

bool XMLElement::SetVector2(const String& name, const Vector2& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetVector3(const String& name, const Vector3& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetVector4(const String& name, const Vector4& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetMatrix3(const String& name, const Matrix3& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::SetMatrix4(const String& name, const Matrix4& value)
{
	return SetAttribute(name, value.to_string());
}

bool XMLElement::IsNull() const
{
	return !NotNull();
}

bool XMLElement::NotNull() const
{
	return node_ || (xpathNode_ && !xpathNode_->operator !());
}

XMLElement::operator bool() const
{
	return NotNull();
}

String XMLElement::GetName() const
{
	if ((!file_ || !node_) && !xpathNode_)
		return String();

	// If xpath_node contains just attribute, return its name instead
	if (xpathNode_ && xpathNode_->attribute())
		return String(xpathNode_->attribute().name());

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return String(node.name());
}

bool XMLElement::HasChild(const String& name) const
{
	return HasChild(name.c_str());
}

bool XMLElement::HasChild(const char* name) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return false;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return !node.child(name).empty();
}

XMLElement XMLElement::GetChild(const String& name) const
{
	return GetChild(name.c_str());
}

XMLElement XMLElement::GetChild(const char* name) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return XMLElement();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	if (!strlen(name))
		return XMLElement(file_, node.first_child().internal_object());
	else
		return XMLElement(file_, node.child(name).internal_object());
}

XMLElement XMLElement::GetNext(const String& name) const
{
	return GetNext(name.c_str());
}

XMLElement XMLElement::GetNext(const char* name) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return XMLElement();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	if (!strlen(name))
		return XMLElement(file_, node.next_sibling().internal_object());
	else
		return XMLElement(file_, node.next_sibling(name).internal_object());
}

XMLElement XMLElement::GetParent() const
{
	if (!file_ || (!node_ && !xpathNode_))
		return XMLElement();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return XMLElement(file_, node.parent().internal_object());
}

unsigned XMLElement::GetNumAttributes() const
{
	if (!file_ || (!node_ && !xpathNode_))
		return 0;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	unsigned ret = 0;

	pugi::xml_attribute attr = node.first_attribute();
	while (!attr.empty())
	{
		++ret;
		attr = attr.next_attribute();
	}

	return ret;
}

bool XMLElement::HasAttribute(const String& name) const
{
	return HasAttribute(name.c_str());
}

bool XMLElement::HasAttribute(const char* name) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return false;

	// If xpath_node contains just attribute, check against it
	if (xpathNode_ && xpathNode_->attribute())
		return String(xpathNode_->attribute().name()) == name;

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return !node.attribute(name).empty();
}

String XMLElement::GetValue() const
{
	if (!file_ || (!node_ && !xpathNode_))
		return "";

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return String(node.child_value());
}

String XMLElement::GetAttribute(const String& name) const
{
	return String(GetAttributeCString(name.c_str()));
}

String XMLElement::GetAttribute(const char* name) const
{
	return String(GetAttributeCString(name));
}

const char* XMLElement::GetAttributeCString(const char* name) const
{
	if (!file_ || (!node_ && !xpathNode_))
		return 0;

	// If xpath_node contains just attribute, return it regardless of the specified name
	if (xpathNode_ && xpathNode_->attribute())
		return xpathNode_->attribute().value();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	return node.attribute(name).value();
}


vector<String>::type XMLElement::GetAttributeNames() const
{
	if (!file_ || (!node_ && !xpathNode_))
		return vector<String>::type();

	const pugi::xml_node& node = xpathNode_ ? xpathNode_->node() : pugi::xml_node(node_);
	vector<String>::type ret;

	pugi::xml_attribute attr = node.first_attribute();
	while (!attr.empty())
	{
		ret.push_back(String(attr.name()));
		attr = attr.next_attribute();
	}

	return ret;
}

bool XMLElement::GetBool(const String& name) const
{
	return to_bool(GetAttribute(name));
}

AxisAlignedBox XMLElement::GetAxisAlignedBox() const
{
	AxisAlignedBox ret;

	Vector3 min_ = GetVector3("min");
	Vector3 max_ = GetVector3("max");
	
	ret.setExtents(min_, max_);

	return ret;
}

vector<unsigned char>::type XMLElement::GetBuffer(const String& name) const
{
	vector<unsigned char>::type ret;
	StringToBuffer(ret, GetAttribute(name));
	return ret;
}

bool XMLElement::GetBuffer(const String& name, void* dest, unsigned size) const
{
	vector<unsigned char>::type ret;
	vector<String>::type bytes = split_str(GetAttribute(name), ' ');
	unsigned char* destBytes = (unsigned char*)dest;
	if (size < bytes.size())
		return false;

	for (unsigned i = 0; i < bytes.size(); ++i)
		destBytes[i] = to_int(bytes[i]);
	return true;
}

Color XMLElement::GetColor(const String& name) const
{
	return to_color(GetAttribute(name));
}

float XMLElement::GetFloat(const String& name) const
{
	return to_float(GetAttribute(name));
}

unsigned XMLElement::GetUInt(const String& name) const
{
	return to_uint(GetAttribute(name));
}

int XMLElement::GetInt(const String& name) const
{
	return to_int(GetAttribute(name));
}

IntRect XMLElement::GetIntRect(const String& name) const
{
	return to_int_rect(GetAttribute(name));
}

IntVector2 XMLElement::GetIntVector2(const String& name) const
{
	return to_int_vec2(GetAttribute(name));
}

Quaternion XMLElement::GetQuaternion(const String& name) const
{
	return to_quat(GetAttribute(name));
}

Vector2 XMLElement::GetVector2(const String& name) const
{
	return to_vec2(GetAttribute(name));
}

Vector3 XMLElement::GetVector3(const String& name) const
{
	return to_vec3(GetAttribute(name));
}

Vector4 XMLElement::GetVector4(const String& name) const
{
	return to_vec4(GetAttribute(name));
}

Vector4 XMLElement::GetVector(const String& name) const
{
	return to_vec4(GetAttribute(name));
}

Matrix3 XMLElement::GetMatrix3(const String& name) const
{
	return to_matrix3(GetAttribute(name));
}

Matrix4 XMLElement::GetMatrix4(const String& name) const
{
	return to_matrix4(GetAttribute(name));
}

XMLFile* XMLElement::GetFile() const
{
	return file_;
}

XMLElement XMLElement::NextResult() const
{
	if (!xpathResultSet_ || !xpathNode_)
		return XMLElement();

	return xpathResultSet_->operator [](++xpathResultIndex_);
}

XPathResultSet::XPathResultSet() :
	resultSet_(0)
{
}

XPathResultSet::XPathResultSet(XMLFile* file, pugi::xpath_node_set* resultSet) :
	file_(file),
	resultSet_(resultSet ? new pugi::xpath_node_set(resultSet->begin(), resultSet->end()) : 0)
{
	// Sort the node set in forward document order
	if (resultSet_)
		resultSet_->sort();
}

XPathResultSet::XPathResultSet(const XPathResultSet& rhs) :
	file_(rhs.file_),
	resultSet_(rhs.resultSet_ ? new pugi::xpath_node_set(rhs.resultSet_->begin(), rhs.resultSet_->end()) : 0)
{
}

XPathResultSet::~XPathResultSet()
{
	delete resultSet_;
	resultSet_ = 0;
}

XPathResultSet& XPathResultSet::operator = (const XPathResultSet& rhs)
{
	file_ = rhs.file_;
	resultSet_ = rhs.resultSet_ ? new pugi::xpath_node_set(rhs.resultSet_->begin(), rhs.resultSet_->end()) : 0;
	return *this;
}

XMLElement XPathResultSet::operator[](unsigned index) const
{
	if (!resultSet_)
	{
		ENN_ASSERT(0);
	}

	return resultSet_ && index < Size() ? XMLElement(file_, this, &resultSet_->operator [](index), index) : XMLElement();
}

XMLElement XPathResultSet::FirstResult()
{
	return operator [](0);
}

unsigned XPathResultSet::Size() const
{
	return resultSet_ ? resultSet_->size() : 0;
}

bool XPathResultSet::Empty() const
{
	return resultSet_ ? resultSet_->empty() : true;
}

XPathQuery::XPathQuery() :
	query_(0),
	variables_(0)
{
}

XPathQuery::XPathQuery(const String& queryString, const String& variableString) :
	query_(0),
	variables_(0)
{
	SetQuery(queryString, variableString);
}

XPathQuery::~XPathQuery()
{
	delete variables_;
	variables_ = 0;
	delete query_;
	query_ = 0;
}

void XPathQuery::Bind()
{
	// Delete previous query object and create a new one binding it with variable set
	delete query_;
	query_ = new pugi::xpath_query(queryString_.c_str(), variables_);
}

bool XPathQuery::SetVariable(const String& name, bool value)
{
	if (!variables_)
		variables_ = new pugi::xpath_variable_set();
	return variables_->set(name.c_str(), value);
}

bool XPathQuery::SetVariable(const String& name, float value)
{
	if (!variables_)
		variables_ = new pugi::xpath_variable_set();
	return variables_->set(name.c_str(), value);
}

bool XPathQuery::SetVariable(const String& name, const String& value)
{
	return SetVariable(name.c_str(), value.c_str());
}

bool XPathQuery::SetVariable(const char* name, const char* value)
{
	if (!variables_)
		variables_ = new pugi::xpath_variable_set();
	return variables_->set(name, value);
}

bool XPathQuery::SetVariable(const String& name, const XPathResultSet& value)
{
	if (!variables_)
		variables_ = new pugi::xpath_variable_set();

	pugi::xpath_node_set* nodeSet = value.GetXPathNodeSet();
	if (!nodeSet)
		return false;

	return variables_->set(name.c_str(), *nodeSet);
}

bool XPathQuery::SetQuery(const String& queryString, const String& variableString, bool bind)
{
	if (!variableString.empty())
	{
		Clear();
		variables_ = new pugi::xpath_variable_set();

		// Parse the variable string having format "name1:type1,name2:type2,..." where type is one of "Bool", "Float", "String", "ResultSet"
		vector<String>::type vars = split_str(variableString, ',');

		for (vector<String>::type::const_iterator i = vars.begin(); i != vars.end(); ++i)
		{
			const String& str = *i;

			vector<String>::type tokens = split_str(trim_str(str), (':'));
			if (tokens.size() != 2)
				continue;

			pugi::xpath_value_type type;
			if (tokens[1] == "Bool")
				type = pugi::xpath_type_boolean;
			else if (tokens[1] == "Float")
				type = pugi::xpath_type_number;
			else if (tokens[1] == "String")
				type = pugi::xpath_type_string;
			else if (tokens[1] == "ResultSet")
				type = pugi::xpath_type_node_set;
			else
				return false;

			if (!variables_->add(tokens[0].c_str(), type))
				return false;
		}
	}

	queryString_ = queryString;

	if (bind)
		Bind();

	return true;
}

void XPathQuery::Clear()
{
	queryString_.clear();

	delete variables_;
	variables_ = 0;
	delete query_;
	query_ = 0;
}

bool XPathQuery::EvaluateToBool(XMLElement element) const
{
	if (!query_ || ((!element.GetFile() || !element.GetNode()) && !element.GetXPathNode()))
		return false;

	const pugi::xml_node& node = element.GetXPathNode() ? element.GetXPathNode()->node() : pugi::xml_node(element.GetNode());
	return query_->evaluate_boolean(node);
}

float XPathQuery::EvaluateToFloat(XMLElement element) const
{
	if (!query_ || ((!element.GetFile() || !element.GetNode()) && !element.GetXPathNode()))
		return 0.0f;

	const pugi::xml_node& node = element.GetXPathNode() ? element.GetXPathNode()->node() : pugi::xml_node(element.GetNode());
	return (float)query_->evaluate_number(node);
}

String XPathQuery::EvaluateToString(XMLElement element) const
{
	if (!query_ || ((!element.GetFile() || !element.GetNode()) && !element.GetXPathNode()))
		return "";

	const pugi::xml_node& node = element.GetXPathNode() ? element.GetXPathNode()->node() : pugi::xml_node(element.GetNode());
	String result;
	result.reserve(query_->evaluate_string(0, 0, node));    // First call get the size
	query_->evaluate_string(const_cast<pugi::char_t*>(result.c_str()), result.capacity(), node);  // Second call get the actual string
	return result;
}

XPathResultSet XPathQuery::Evaluate(XMLElement element) const
{
	if (!query_ || ((!element.GetFile() || !element.GetNode()) && !element.GetXPathNode()))
		return XPathResultSet();

	const pugi::xml_node& node = element.GetXPathNode() ? element.GetXPathNode()->node() : pugi::xml_node(element.GetNode());
	pugi::xpath_node_set result = query_->evaluate_node_set(node);
	return XPathResultSet(element.GetFile(), &result);
}

}