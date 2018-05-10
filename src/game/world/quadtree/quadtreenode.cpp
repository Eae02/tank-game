#include "quadtreenode.h"
#include "quadtree.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>

namespace TankGame
{
	constexpr int QuadTreeNode::DIVIDE_THRESHOLD;
	constexpr int QuadTreeNode::MAX_DEPTH;
	
	QuadTreeNode::QuadTreeNode(const QuadTree& tree, const Rectangle& area, int depth)
	    : m_depth(depth), m_tree(&tree), m_area(area)
	{
		
	}
	
	void QuadTreeNode::Divide()
	{
		if (!m_isLeaf)
			return;
		m_isLeaf = false;
		
		std::array<Rectangle, 4> childAreas = m_area.Divide();
		
		for (int i = 0; i < 4; i++)
		{
			m_children[i] = m_tree->m_nodePool->MakeUnique(*m_tree, childAreas[i], m_depth + 1);
			
			for (long j = m_entries.size() - 1; j >= 0; j--)
			{
				if (childAreas[i].Contains(m_entries[j]->GetBoundingRectangle()))
				{
					m_children[i]->m_entries.push_back(m_entries[j]);
					
					m_entries[j] = m_entries.back();
					m_entries.pop_back();
				}
			}
			
			m_children[i]->Update();
		}
	}
	
	void QuadTreeNode::Collapse()
	{
		if (m_isLeaf)
			return;
		m_isLeaf = true;
		
		for (int i = 0; i < 4; i++)
		{
			m_children[i]->Collapse();
			
			for (const IQuadTreeEntry* entry : m_children[i]->m_entries)
			{
				m_entries.push_back(entry);
			}
			
			m_children[i] = nullptr;
		}
	}
	
	//Updates the layout of the node to account for moved entries.
	void QuadTreeNode::Update()
	{
		if (m_isLeaf)
		{
			if (m_depth >= MAX_DEPTH)
				return;
			
			std::array<Rectangle, 4> childAreas = m_area.Divide();
			
			size_t entriesLostByDividing = 0;
			for (const IQuadTreeEntry* entry : m_entries)
			{
				Rectangle boundingRect = entry->GetBoundingRectangle();
				if (std::any_of(childAreas.begin(), childAreas.end(),
				                [&] (const Rectangle& r) { return r.Contains(boundingRect); }))
				{
					entriesLostByDividing++;
				}
			}
			
			if (entriesLostByDividing > DIVIDE_THRESHOLD)
				Divide();
		}
		else
		{
			if (GetEntriesCount() <= DIVIDE_THRESHOLD)
			{
				Collapse();
			}
			else
			{
				for (int i = 0; i < 4; i++)
					m_children[i]->Update();
			}
		}
	}
	
	size_t QuadTreeNode::GetEntriesCount() const
	{
		size_t count = m_entries.size();
		
		if (!m_isLeaf)
		{
			for (const ObjectPool<QuadTreeNode>::UniquePtr& child : m_children)
				count += child->GetEntriesCount();
		}
		
		return count;
	}
	
	void QuadTreeNode::SetTree(const class QuadTree& tree)
	{
		m_tree = &tree;
		
		if (!m_isLeaf)
		{
			for (const ObjectPool<QuadTreeNode>::UniquePtr& child : m_children)
				child->SetTree(tree);
		}
	}
	
	void QuadTreeNode::AddEntry(const IQuadTreeEntry& entry, const Rectangle& rectangle)
	{
		if (!m_isLeaf)
		{
			for (const ObjectPool<QuadTreeNode>::UniquePtr& child : m_children)
			{
				if (child->m_area.Contains(rectangle))
				{
					child->AddEntry(entry, rectangle);
					return;
				}
			}
		}
		
		m_entries.push_back(&entry);
	}
	
	void QuadTreeNode::RemoveEntry(const class IQuadTreeEntry& entry, const Rectangle& rectangle)
	{
		if (!m_isLeaf)
		{
			for (const ObjectPool<QuadTreeNode>::UniquePtr& child : m_children)
			{
				if (child->m_area.Contains(rectangle))
				{
					child->RemoveEntry(entry, rectangle);
					return;
				}
			}
		}
		
		auto pos = std::find(m_entries.begin(), m_entries.end(), &entry);
		assert(pos != m_entries.end());
		
		*pos = m_entries.back();
		m_entries.pop_back();
	}
}
