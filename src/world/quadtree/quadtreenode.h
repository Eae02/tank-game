#pragma once

#include "../../rectangle.h"
#include "../../utils/memory/objectpool.h"
#include "iquadtreeentry.h"

#include <memory>
#include <vector>

namespace TankGame
{
	class QuadTreeNode
	{
		friend class QuadTree;
		friend class QuadTreeVisualizer;
		
	public:
		QuadTreeNode(const class QuadTree& tree, const Rectangle& area, int depth);
		
		void Update();
		
		//Recursively calculates the number of entries in this node and all it's children.
		size_t GetEntriesCount() const;
		
		template <typename CallbackTp>
		void IterateIntersecting(CallbackTp callback, const Rectangle& rectangle) const
		{
			if (!m_isLeaf)
			{
				for (const ObjectPool<QuadTreeNode>::UniquePtr& child : m_children)
				{
					if (child->m_area.Intersects(rectangle))
						child->IterateIntersecting(callback, rectangle);
				}
			}
			
			for (long i = m_entries.size() - 1; i >= 0; i--)
			{
				if (m_entries[i]->GetBoundingRectangle().Intersects(rectangle))
					callback(*m_entries[i]);
			}
		}
		
		static constexpr int DIVIDE_THRESHOLD = 2;
		static constexpr int MAX_DEPTH = 6;
		
	private:
		void AddEntry(const IQuadTreeEntry& entry, const Rectangle& rectangle);
		void RemoveEntry(const IQuadTreeEntry& entry, const Rectangle& rectangle);
		
		void Divide();
		void Collapse();
		
		void SetTree(const class QuadTree& tree);
		
		bool m_isLeaf = true;
		
		std::vector<const IQuadTreeEntry*> m_entries;
		
		//Pointers to the node's children, for leaf nodes these are all null.
		ObjectPool<QuadTreeNode>::UniquePtr m_children[4];
		
		//The node's depth in the tree. Depth 0 is the root node.
		int m_depth;
		
		const class QuadTree* m_tree;
		
		Rectangle m_area;
	};
}
