#include "quadtree.h"
#include "../../utils/utils.h"

#include <algorithm>
#include <cmath>

namespace TankGame
{
	//Returns the maximum number of nodes in a quadtree with the specified depth
	static size_t GetMaxNodeCount(int depth)
	{
		size_t nodeCount = 0;
		for (int i = 1; i <= depth; i++)
			nodeCount += std::pow(4, i);
		return nodeCount;
	}
	
	QuadTree::QuadTree(const Rectangle& area)
	    : m_rootNode(*this, area, 0),
	      m_nodePool(std::make_unique<ObjectPool<QuadTreeNode>>(GetMaxNodeCount(QuadTreeNode::MAX_DEPTH)))
	{
		
	}
	
	QuadTree& QuadTree::operator=(QuadTree&& other)
	{
		m_rootNode = std::move(other.m_rootNode);
		m_rootNode.SetTree(*this);
		
		m_nodePool = std::move(other.m_nodePool);
		
		return *this;
	}
	
	QuadTree::QuadTree(QuadTree&& other)
	    : m_rootNode(std::move(other.m_rootNode)), m_nodePool(std::move(other.m_nodePool))
	{
		m_rootNode.SetTree(*this);
	}
	
	//Adds an entry to the quadtree.
	void QuadTree::Add(const IQuadTreeEntry& entry)
	{
		m_rootNode.AddEntry(entry, entry.GetBoundingRectangle());
		
		m_entries.emplace_back(entry);
	}
	
	//Removes an entry from the quadtree.
	void QuadTree::Remove(const IQuadTreeEntry& entry)
	{
		auto pos = std::find_if(m_entries.begin(), m_entries.end(), [&entry] (const Entry& e)
		{
			return e.m_entry == &entry;
		});
		
		if (pos == m_entries.end())
			return;
		
		m_rootNode.RemoveEntry(entry, pos->m_oldRectangle);
		
		*pos = m_entries.back();
		m_entries.pop_back();
	}
	
	//Updates the layout of the quadtree to account for moved entries. Called at the end of each frame.
	void QuadTree::Update()
	{
		for (Entry& entry : m_entries)
		{
			Rectangle newRectangle = entry.m_entry->GetBoundingRectangle();
			
			if (newRectangle != entry.m_oldRectangle)
			{
				m_rootNode.RemoveEntry(*entry.m_entry, entry.m_oldRectangle);
				m_rootNode.AddEntry(*entry.m_entry, newRectangle);
				
				entry.m_oldRectangle = newRectangle;
			}
		}
		
		m_rootNode.Update();
	}
}
