#pragma once

#include "quadtreenode.h"
#include "iquadtreeentry.h"
#include "../../rectangle.h"
#include "../../utils/memory/objectpool.h"

namespace TankGame
{
	class QuadTree
	{
		friend class QuadTreeNode;
		friend class QuadTreeVisualizer;
		
	public:
		QuadTree(const Rectangle& area);
		
		virtual ~QuadTree() = default;
		
		QuadTree(QuadTree&& other);
		QuadTree& operator=(QuadTree&& other);
		
		QuadTree(const QuadTree& other) = delete;
		QuadTree& operator=(const QuadTree& other) = delete;
		
		void Add(const IQuadTreeEntry& entry);
		void Remove(const IQuadTreeEntry& entry);
		
		void Update();
		
		template <typename CallbackTp>
		void IterateIntersecting(CallbackTp callback, const Rectangle& rectangle) const
		{
			m_rootNode.IterateIntersecting(callback, rectangle);
		}
		
	private:
		struct Entry
		{
			const IQuadTreeEntry* m_entry;
			Rectangle m_oldRectangle;
			
			inline explicit Entry(const IQuadTreeEntry& entry)
			    : m_entry(&entry), m_oldRectangle(entry.GetBoundingRectangle()) { }
		};
		
		std::vector<Entry> m_entries;
		
		/*
			Nodes are allocated from a pool as they will be allocated / freed often when the quadtree is restructured.
			The pool must be allocated on the heap so it doesn't move in memory. This is important because nodes keep
			a reference to the pool object they were allocated from.
		*/
		std::unique_ptr<ObjectPool<QuadTreeNode>> m_nodePool;
		
		QuadTreeNode m_rootNode;
	};
}
