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
// File: enn_xml_file.cpp
//

#include <pugixml.hpp>
#include "enn_xml_file.h"

namespace enn
{

/// XML writer for pugixml.
class XMLWriter : public pugi::xml_writer
{
public:
	/// Construct.
	XMLWriter(Serializer& dest) :
		dest_(dest),
		success_(true)
	{
	}

	/// Write bytes to output.
	void write(const void* data, size_t size)
	{
		if (dest_.Write(data, size) != size)
			success_ = false;
	}

	/// Destination serializer.
	Serializer& dest_;
	/// Success flag.
	bool success_;
};

XMLFile::XMLFile() : document_(new pugi::xml_document())
{
}

XMLFile::~XMLFile()
{
	delete document_;
	document_ = 0;
}

bool XMLFile::Load(Deserializer& source)
{
	unsigned dataSize = source.GetSize();
	if (!dataSize)
	{
		ENN_ASSERT(0);
		return false;
	}

	SharedPtr<char, SPFM_FREE> buffer((char*)ENN_MALLOC(dataSize));

	if (source.Read(buffer.get(), dataSize) != dataSize)
		return false;

	if (!document_->load_buffer(buffer.get(), dataSize))
	{
		ENN_ASSERT(0);
		document_->reset();
		return false;
	}

	return true;
}

bool XMLFile::Save(Serializer& dest) const
{
	XMLWriter writer(dest);
	document_->save(writer);
	return writer.success_;
}

XMLElement XMLFile::CreateRoot(const String& name)
{
	document_->reset();
	pugi::xml_node root = document_->append_child(name.c_str());
	return XMLElement(this, root.internal_object());
}

XMLElement XMLFile::GetRoot(const String& name)
{
	pugi::xml_node root = document_->first_child();
	if (root.empty())
		return XMLElement();

	if (!name.empty() && name != root.name())
		return XMLElement();
	else
		return XMLElement(this, root.internal_object());
}

void XMLFile::Patch(XMLFile* patchFile)
{
	Patch(patchFile->GetRoot());
}

void XMLFile::Patch(XMLElement patchElement)
{
	pugi::xml_node root = pugi::xml_node(patchElement.GetNode());

	for (pugi::xml_node::iterator patch = root.begin(); patch != root.end(); patch++)
	{
		pugi::xml_attribute sel = patch->attribute("sel");
		if (sel.empty())
		{
			ENN_ASSERT(0);
			continue;
		}

		// Only select a single node at a time, they can use xpath to select specific ones in multiple otherwise the node set becomes invalid due to changes
		pugi::xpath_node original = document_->select_single_node(sel.value());
		if (!original)
		{
			ENN_ASSERT(0);
			continue;
		}

		if (strcmp(patch->name(), "add") == 0)
			PatchAdd(*patch, original);
		else if (strcmp(patch->name(), "replace") == 0)
			PatchReplace(*patch, original);
		else if (strcmp(patch->name(), "remove") == 0)
			PatchRemove(original);
		else
			ENN_ASSERT(0);
	}
}

void XMLFile::PatchAdd(const pugi::xml_node& patch, pugi::xpath_node& original)
{
	// If not a node, log an error
	if (original.attribute())
	{
		ENN_ASSERT(0);
		return;
	}

	// If no type add node, if contains '@' treat as attribute
	pugi::xml_attribute type = patch.attribute("type");
	if (!type || strlen(type.value()) <= 0)
		AddNode(patch, original);
	else if (type.value()[0] == '@')
		AddAttribute(patch, original);
}

void XMLFile::PatchReplace(const pugi::xml_node& patch, pugi::xpath_node& original)
{
	// If no attribute but node then its a node, otherwise its an attribute or null
	if (!original.attribute() && original.node())
	{
		pugi::xml_node parent = original.node().parent();

		parent.insert_copy_before(patch.first_child(), original.node());
		parent.remove_child(original.node());
	}
	else if (original.attribute())
	{
		original.attribute().set_value(patch.child_value());
	}
}

void XMLFile::PatchRemove(const pugi::xpath_node& original)
{
	// If no attribute but node then its a node, otherwise its an attribute or null
	if (!original.attribute() && original.node())
	{
		pugi::xml_node parent = original.parent();
		parent.remove_child(original.node());
	}
	else if (original.attribute())
	{
		pugi::xml_node parent = original.parent();
		parent.remove_attribute(original.attribute());
	}
}

void XMLFile::AddNode(const pugi::xml_node& patch, pugi::xpath_node& original)
{
	// If pos is null, append or prepend add as a child, otherwise add before or after, the default is to append as a child
	pugi::xml_attribute pos = patch.attribute("pos");
	if (!pos || strlen(pos.value()) <= 0 || pos.value() == "append")
	{
		pugi::xml_node::iterator start = patch.begin();
		pugi::xml_node::iterator end = patch.end();

		// There can not be two consecutive text nodes, so check to see if they need to be combined
		// If they have been we can skip the first node of the nodes to add
		if (CombineText(patch.first_child(), original.node().last_child(), false))
			start++;

		for (; start != end; start++)
			original.node().append_copy(*start);
	}
	else if (strcmp(pos.value(), "prepend") == 0)
	{
		pugi::xml_node::iterator start = patch.begin();
		pugi::xml_node::iterator end = patch.end();

		// There can not be two consecutive text nodes, so check to see if they need to be combined
		// If they have been we can skip the last node of the nodes to add
		if (CombineText(patch.last_child(), original.node().first_child(), true))
			end--;

		pugi::xml_node pos = original.node().first_child();
		for (; start != end; start++)
			original.node().insert_copy_before(*start, pos);
	}
	else if (strcmp(pos.value(), "before") == 0)
	{
		pugi::xml_node::iterator start = patch.begin();
		pugi::xml_node::iterator end = patch.end();

		// There can not be two consecutive text nodes, so check to see if they need to be combined
		// If they have been we can skip the first node of the nodes to add
		if (CombineText(patch.first_child(), original.node().previous_sibling(), false))
			start++;

		// There can not be two consecutive text nodes, so check to see if they need to be combined
		// If they have been we can skip the last node of the nodes to add
		if (CombineText(patch.last_child(), original.node(), true))
			end--;

		for (; start != end; start++)
			original.parent().insert_copy_before(*start, original.node());
	}
	else if (strcmp(pos.value(), "after") == 0)
	{
		pugi::xml_node::iterator start = patch.begin();
		pugi::xml_node::iterator end = patch.end();

		// There can not be two consecutive text nodes, so check to see if they need to be combined
		// If they have been we can skip the first node of the nodes to add
		if (CombineText(patch.first_child(), original.node(), false))
			start++;

		// There can not be two consecutive text nodes, so check to see if they need to be combined
		// If they have been we can skip the last node of the nodes to add
		if (CombineText(patch.last_child(), original.node().next_sibling(), true))
			end--;

		pugi::xml_node pos = original.node();
		for (; start != end; start++)
			pos = original.parent().insert_copy_after(*start, pos);
	}
}

void XMLFile::AddAttribute(const pugi::xml_node& patch, pugi::xpath_node& original)
{
	pugi::xml_attribute attribute = patch.attribute("type");

	if (!patch.first_child() && patch.first_child().type() != pugi::node_pcdata)
	{
		ENN_ASSERT(0);
		return;
	}

	String name(attribute.value());
	name = name.substr(1, name.size() - 1);

	pugi::xml_attribute newAttribute = original.node().append_attribute(name.c_str());
	newAttribute.set_value(patch.child_value());
}

bool XMLFile::CombineText(const pugi::xml_node& patch, pugi::xml_node original, bool prepend)
{
	if (!patch || !original)
		return false;

	if ((patch.type() == pugi::node_pcdata && original.type() == pugi::node_pcdata) ||
		(patch.type() == pugi::node_cdata && original.type() == pugi::node_cdata))
	{
		if (prepend)
		{
			original.set_value((String(patch.value()) + original.value()).c_str());
		}
			
		else
		{
			original.set_value((String(original.value()) + patch.value()).c_str());
		}

		return true;
	}

	return false;
}

}