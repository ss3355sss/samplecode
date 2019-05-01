#pragma once
#include "base/pch.h"

#include "base/array.h"
#include "base/bbox.h"
#include "base/intersectionQueryEngine.h"
#include "base/nearestNeighborQueryEngine.h"

NAMESPACE_BASE_BEGIN
template <typename ItemT, typename T>
class Bvh3 final : public NearestNeighborQueryEngine3<ItemT, T>, public IntersectionQueryEngine3<ItemT, T>
{
private:
	struct Node
	{
	public:
		char flags;
		union
		{
			size_t child;	/*index*/
			size_t item;	/*index*/
		};
		BBox3<T> bound;

	public:
		Node() : flags(0)
		{
			child = MaxSize();
		}

	public:
		void initLeaf(size_t it, const BBox3<T>& b)
		{
			flags 	= 3;
			item 	= it;
			bound 	= b;
		}
		void initInternal(uint8_t axis, size_t c, const BBox3<T>& b)
		{
			flags = axis;
			child = c;
			bound = b;
		}
		bool isLeaf() const
		{
			return flags == 3;
		}
	};

public:
	typedef Array1<ItemT> 		ItemContainerT;
	typedef Array1<BBox3<T>> 	BoundContainerT;
	typedef Array1<Node> 		NodeContainerT;

public:
	typedef T										value_type;
	typedef typename ItemContainerT::value_type 	item_type;
	typedef typename ItemContainerT::iterator 		iterator;
	typedef typename ItemContainerT::const_iterator const_iterator;


private:
	BBox3<T> 		bound_;
	ItemContainerT 	items_;
	BoundContainerT itemBounds_;
	NodeContainerT	nodes_;

public:
	Bvh3()
	{
	}
	virtual ~Bvh3() = default;

public:
	void build(const ItemContainerT& items, const BoundContainerT& itemsBounds)
	{
		items_ 			= items;
		itemBounds_ 	= itemsBounds;

		if (items_.empty())
		{
			return;
		}

		nodes_.clear();
		for (size_t i = 0; i < items_.size(); ++i)
		{
			bound_.merge(itemBounds_[i]);
		}

		Array1<size_t> itemIndices(items_.size());
		std::iota(std::begin(itemIndices), std::end(itemIndices), 0);

		build(0, itemIndices.data(), items_.size(), 0);
	}
	void clear()
	{
		bound_ = BBox3<T>();
		items_.clear();
		itemBounds_.clear();
		nodes_.clear();
	}
	const BBox3<T>& bound() const
	{
		return bound_;
	}
	iterator begin()
	{
		return items_.begin();
	}
	iterator end()
	{
		return items_.end();
	}
	const_iterator cbegin() const
	{
		return items_.cbegin();
	}
	const_iterator cend() const
	{
		return items_.cend();
	}
	size_t size() const
	{
		return items_.size();
	}
	const ItemT& item(size_t i) const
	{
		return items_[i];
	}

public:
	/* NearestNeighborQueryEngine3 Override */
	virtual NearestNeighborQueryResult3<ItemT, T> nearest(const Point3<T>& pt, const NearestNeighborDistanceFunc3<ItemT, T>& distanceFunc) const
	{
		NearestNeighborQueryResult3<ItemT, T> best;
		best.dist = Infinity<T>();
		best.item = nullptr;

		// Prepare to traverse BVH
		static const int kMaxTreeDepth = 8 * sizeof(size_t);
		const Node* todo[kMaxTreeDepth];
		size_t todoPos = 0;

		// Traverse BVH nodes
		const Node* node = nodes_.data();
		while (node != nullptr)
		{
			if (node->isLeaf())
			{
				double dist = distanceFunc(items_[node->item], pt);
				if (dist < best.dist)
				{
					best.dist = dist;
					best.item = &items_[node->item];
				}

				// Grab next node to process from todo stack
				if (todoPos > 0)
				{
					// Dequeue
					--todoPos;
					node = todo[todoPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				const double bestDistSqr = best.dist * best.dist;

				const Node* left = node + 1;
				const Node* right = &nodes_[node->child];

				Vec3<T> closestLeft 	= left->bound.clamp(pt);
				Vec3<T> closestRight 	= right->bound.clamp(pt);

				T distMinLeftSqr = closestLeft.distanceSquaredTo(pt);
				T distMinRightSqr = closestRight.distanceSquaredTo(pt);

				bool shouldVisitLeft = distMinLeftSqr < bestDistSqr;
				bool shouldVisitRight = distMinRightSqr < bestDistSqr;

				const Node* first_child;
				const Node* second_child;
				if (shouldVisitLeft && shouldVisitRight)
				{
					if (distMinLeftSqr < distMinRightSqr)
					{
						first_child = left;
						second_child = right;
					} else
					{
						first_child = right;
						second_child = left;
					}

					// Enqueue secondChild in todo stack
					todo[todoPos] = second_child;
					++todoPos;
					node = first_child;
				}
				else if (shouldVisitLeft)
				{
					node = left;
				} else if (shouldVisitRight)
				{
					node = right;
				} else
				{
					if (todoPos > 0)
					{
						// Dequeue
						--todoPos;
						node = todo[todoPos];
					}
					else
					{
						break;
					}
				}
			}
		}
		return best;

	}

public:
	/* IntersectionQueryEngine3 Override */
	virtual bool intersects(const BBox3<T>& box, const BoxIntersectionTestFunc3<ItemT, T>& testFunc) const
	{
		if (!bound_.overlaps(box))
		{
			return false;
		}

		// prepare to traverse BVH for box
		static const int kMaxTreeDepth = 8 * sizeof(size_t);
		const Node* todo[kMaxTreeDepth];
		size_t todoPos = 0;

		// traverse BVH nodes for box
		const Node* node = nodes_.data();

		while (node != nullptr)
		{
			if (node->isLeaf())
			{
				if (testFunc(items_[node->item], box))
				{
					return true;
				}

				// grab next node to process from todo stack
				if (todoPos > 0)
				{
					// Dequeue
					--todoPos;
					node = todo[todoPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				// get node children pointers for box
				const Node* firstChild = node + 1;
				const Node* secondChild = (Node*)&nodes_[node->child];

				// advance to next child node, possibly enqueue other child
				if (!firstChild->bound.overlaps(box))
				{
					node = secondChild;
				}
				else if (!secondChild->bound.overlaps(box))
				{
					node = firstChild;
				}
				else
				{
					// enqueue secondChild in todo stack
					todo[todoPos] = secondChild;
					++todoPos;
					node = firstChild;
				}
			}
		}

		return false;

	}
	virtual bool intersects(const Ray3<T>& ray, const RayIntersectionTestFunc3<ItemT, T>& testFunc) const
	{
		if (!bound_.intersects(ray))
		{
			return false;
		}

		// prepare to traverse BVH for ray
		static const int kMaxTreeDepth = 8 * sizeof(size_t);
		const Node* todo[kMaxTreeDepth];
		size_t todoPos = 0;

		// traverse BVH nodes for ray
		const Node* node = nodes_.data();

		while (node != nullptr)
		{
			if(node->isLeaf())
			{
				if(testFunc(items_[node->item], ray))
				{
					return true;
				}

				// grab next node to process from todo stack
				if(todoPos > 0)
				{
					// Dequeue
					--todoPos;
					node = todo[todoPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				// get node children pointers for ray
				const Node* firstChild;
				const Node* secondChild;
				if(ray.direction()[node->flags] > 0.0)
				{
					firstChild  = node + 1;
					secondChild = (Node*) &nodes_[node->child];
				}
				else
				{
					firstChild  = (Node*) &nodes_[node->child];
					secondChild = node + 1;
				}

				// advance to next child node, possibly enqueue other child
				if(!firstChild->bound.intersects(ray))
				{
					node = secondChild;
				}
				else if(!secondChild->bound.intersects(ray))
				{
					node = firstChild;
				}
				else
				{
					// enqueue secondChild in todo stack
					todo[todoPos] = secondChild;
					++todoPos;
					node = firstChild;
				}
			}
		}

		return false;

	}

	virtual void forEachIntersectingItem(const BBox3<T>& box, const BoxIntersectionTestFunc3<ItemT, T>& testFunc, const IntersectionVisitorFunc3<ItemT, T>& visitorFunc) const
	{
		if(!bound_.overlaps(box))
		{
			return;
		}

		// prepare to traverse BVH for box
		static const int kMaxTreeDepth = 8 * sizeof(size_t);
		const Node* todo[kMaxTreeDepth];
		size_t todoPos = 0;

		// traverse BVH nodes for box
		const Node* node = nodes_.data();

		while (node != nullptr)
		{
			if(node->isLeaf())
			{
				if(testFunc(items_[node->item], box))
				{
					visitorFunc(items_[node->item]);
				}

				// grab next node to process from todo stack
				if(todoPos > 0)
				{
					// Dequeue
					--todoPos;
					node = todo[todoPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				// get node children pointers for box
				const Node* firstChild  = node + 1;
				const Node* secondChild = (Node*) &nodes_[node->child];

				// advance to next child node, possibly enqueue other child
				if(!firstChild->bound.overlaps(box))
				{
					node = secondChild;
				}
				else if(!secondChild->bound.overlaps(box))
				{
					node = firstChild;
				}
				else
				{
					// enqueue secondChild in todo stack
					todo[todoPos] = secondChild;
					++todoPos;
					node = firstChild;
				}
			}
		}
	}
	virtual void forEachIntersectingItem(const Ray3<T>& ray, const RayIntersectionTestFunc3<ItemT, T>& testFunc, const IntersectionVisitorFunc3<ItemT, T>& visitorFunc) const
	{
		if(!bound_.intersects(ray))
		{
			return;
		}

		// prepare to traverse BVH for ray
		static const int kMaxTreeDepth = 8 * sizeof(size_t);
		const Node* todo[kMaxTreeDepth];
		size_t todoPos = 0;

		// traverse BVH nodes for ray
		const Node* node = nodes_.data();

		while (node != nullptr)
		{
			if(node->isLeaf())
			{
				if(testFunc(items_[node->item], ray))
				{
					visitorFunc(items_[node->item]);
				}

				// grab next node to process from todo stack
				if(todoPos > 0)
				{
					// Dequeue
					--todoPos;
					node = todo[todoPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				// get node children pointers for ray
				const Node* firstChild;
				const Node* secondChild;
				if(ray.direction()[node->flags] > static_cast<T>(0))
				{
					firstChild  = node + 1;
					secondChild = (Node*) &nodes_[node->child];
				}
				else
				{
					firstChild  = (Node*) &nodes_[node->child];
					secondChild = node + 1;
				}

				// advance to next child node, possibly enqueue other child
				if(!firstChild->bound.intersects(ray))
				{
					node = secondChild;
				}
				else if(!secondChild->bound.intersects(ray))
				{
					node = firstChild;
				}
				else
				{
					// enqueue secondChild in todo stack
					todo[todoPos] = secondChild;
					++todoPos;
					node = firstChild;
				}
			}
		}
	}
	virtual ClosestIntersectionQueryResult3<ItemT, T> closestIntersection(const Ray3<T>& ray, const GetRayIntersectionFunc3<ItemT, T>& testFunc) const
	{
		ClosestIntersectionQueryResult3<ItemT, T> best;
		best.dist = Infinity<T>();
		best.item = nullptr;

		if(!bound_.intersects(ray))
		{
			return best;
		}

		// prepare to traverse BVH for ray
		static const int kMaxTreeDepth = 8 * sizeof(size_t);
		const Node* todo[kMaxTreeDepth];
		size_t todoPos = 0;

		// traverse BVH nodes for ray
		const Node* node = nodes_.data();

		while (node != nullptr)
		{
			if(node->isLeaf())
			{
				double dist = testFunc(items_[node->item], ray);
				if(dist < best.dist)
				{
					best.dist = dist;
					best.item = items_.data() + node->item;
				}

				/* grab next node to process from todo stack */
				if(todoPos > static_cast<T>(0))
				{
					// Dequeue
					--todoPos;
					node = todo[todoPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				/* get node children pointers for ray */
				const Node* firstChild;
				const Node* secondChild;
				if(ray.direction()[node->flags] > static_cast<T>(0))
				{
					firstChild  = node + 1;
					secondChild = (Node*) &nodes_[node->child];
				}
				else
				{
					firstChild  = (Node*) &nodes_[node->child];
					secondChild = node + 1;
				}

				/* advance to next child node, possibly enqueue other child */
				if(!firstChild->bound.intersects(ray))
				{
					node = secondChild;
				}
				else if(!secondChild->bound.intersects(ray))
				{
					node = firstChild;
				}
				else
				{
					/* enqueue secondChild in todo stack */
					todo[todoPos] = secondChild;
					++todoPos;
					node = firstChild;
				}
			}
		}

		return best;
	}

private:
	size_t build(size_t node_index, size_t* item_indices, size_t num_items, size_t current_depth)
	{
		// add a node
		nodes_.push_back(Node());

		// initialize leaf node if termination criteria met
		if (num_items == 1)
		{
			nodes_[node_index].initLeaf(item_indices[0], itemBounds_[item_indices[0]]);
			return current_depth + 1;
		}

		// find the mid-point of the bounding box to use as a qsplit pivot
		BBox3<T> nodeBound;
		for (size_t i = 0; i < num_items; ++i)
		{
			nodeBound.merge(itemBounds_[item_indices[i]]);
		}

		Vec3<T> d = nodeBound.upper() - nodeBound.lower();

		// choose which axis to split along
		uint8_t axis;
		//if (d.x > d.y && d.x > d.z)
		if (d[0] > d[1] && d[0] > d[2])
		{
			axis = 0;
		} else
		{
			//axis = (d.y > d.z) ? 1 : 2;
			axis = (d[1] > d[2]) ? 1 : 2;

		}

		T pivot = 0.5 * (nodeBound.upper()[axis] + nodeBound.lower()[axis]);

		// classify primitives with respect to split
		size_t midPoint = qsplit(item_indices, num_items, pivot, axis);

		// recursively initialize children _nodes
		size_t d0 = build(node_index + 1, item_indices, midPoint, current_depth + 1);
		nodes_[node_index].initInternal(axis, nodes_.size(), nodeBound);
		size_t d1 = build(nodes_[node_index].child, item_indices + midPoint, num_items - midPoint, current_depth + 1);

		return std::max(d0, d1);

	}
	size_t qsplit(size_t* item_indices, size_t num_items, double pivot, uint8_t axis)
	{
		T centroid;
		size_t ret = 0;
		for (size_t i = 0; i < num_items; ++i)
		{
			BBox3<T> b = itemBounds_[item_indices[i]];
			centroid = 0.5f * (b.lower()[axis] + b.upper()[axis]);
			if (centroid < pivot) {
				std::swap(item_indices[i], item_indices[ret]);
				ret++;
			}
		}

		if (ret == 0 || ret == num_items)
		{
			ret = num_items >> 1;
		}
		return ret;
	}

private:
	friend class boost::serialization::access;
	template<typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver)
	{
		//TODO::Serialize Bvh3
	}

};

template<typename ItemT> using Bvh3f    = Bvh3<ItemT, float>;
template<typename ItemT> using Bvh3d    = Bvh3<ItemT, double>;


NAMESPACE_BASE_END
