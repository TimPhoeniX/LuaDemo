#pragma once
#include "Box2D/Common/b2Math.h"
#include <vector>
#include <array>

struct AABB
{
	b2Vec2 low = b2Vec2_zero;
	b2Vec2 high = b2Vec2_zero;
	inline bool isOverlapping(const AABB& other) const
	{
		return !((other.high.y < this->low.y)
				 || (other.low.y > this->high.y)
				 || (other.low.x > this->high.x)
				 || (other.high.x < this->low.x));
	}
	AABB() = default;
	AABB(b2Vec2 low, b2Vec2 high): low(low), high(high){}
};

template<typename T>
struct Cell
{
	std::vector<T*> Entities;
	AABB aabb;
	Cell()
	{
		this->Entities.reserve(10u);
	}
	Cell(const Cell&) = default;
	Cell(Cell&&) = default;
	Cell(b2Vec2 low, b2Vec2 high): aabb{low, high}
	{}
	explicit Cell(AABB aabb): aabb(aabb)
	{}
};

template<typename T, size_t X, size_t Y>
class CellSpacePartition
{
	class AABBQuery
	{
		size_t beginX, beginY, endX, endY;
	public:
		explicit AABBQuery(const CellSpacePartition*const space, const AABB& query)
		{
			b2Vec2 low = b2Clamp(query.low, b2Vec2_zero, {space->width,space->height});
			b2Vec2 high = b2Clamp(query.high, b2Vec2_zero, {space->width,space->height});
			beginX = size_t(X * low.x / space->width);
			beginY = size_t(Y * low.y / space->height);
			endX = size_t(X * high.x / space->width);
			endX = endX < X ? endX : X - 1;
			endY = size_t(Y * high.y / space->height);
			endY = endY < Y ? endY : Y - 1;
		}
		class iterator: std::iterator<std::input_iterator_tag, size_t>
		{
			const AABBQuery* owner;
			size_t x, y;
		public:
			iterator(size_t x, size_t y, const AABBQuery* owner): owner(owner), x(x), y(y)
			{}
			bool operator==(const iterator& other) const
			{
				return this->x == other.x && this->y == other.y;
			}
			bool operator!=(const iterator& other) const
			{
				return !this->operator==(other);
			}
			iterator& operator++()
			{
				if(this->x == this->owner->endX)
				{
					this->x = this->owner->beginX;
					++this->y;
				}
				else
				{
					++this->x;
				}
				return *this;
			}
			size_t operator*() const
			{
				return size_t(x + X*y);
			}
		};
		iterator begin()
		{
			return iterator(beginX, beginY, this);
		}
		iterator end()
		{
			return iterator(beginX, endY+1u, this);
		}
	};
	std::array<Cell<T>, X*Y> cells;
	const float width;
	const float height;
	const float cellWidth;
	const float cellHeight;
public:
	CellSpacePartition(float width, float height): width(width), height(height), cellWidth(width / X), cellHeight(height / Y)
	{
		for(size_t y = 0u; y < Y; ++y)
		{
			for(size_t x = 0u; x < X; ++x)
			{
				b2Vec2 low{x * this->cellWidth, y * this->cellHeight};
				b2Vec2 high{low.x + this->cellWidth, low.y + this->cellHeight};
				AABB aabb = AABB(low, high);
				this->cells[x + X*y].aabb = aabb;
			}
		}
	}

	void ClearCells()
	{
		for(Cell<T>& c : cells)
		{
			c.Entities.clear();
		}
	}

	void CalculateNeighbours(std::vector<T*>& res, b2Vec2 pos, const float radius)
	{
		AABB query{pos - b2Vec2{radius,radius}, pos + b2Vec2{radius, radius}};
		/*for(Cell<T>& cell : this->cells)
		{
			if(!cell.Entities.empty() && cell.aabb.isOverlapping(query))
			{
				for(T* en : cell.Entities)
				{
					if(b2DistanceSquared(en->getPosition(), pos), radius*radius)
					{
						res.push_back(en);
					}
				}
			}
		}*/
		AABBQuery list(this, query);
		for(size_t it: list)
		{
			auto& cell = this->cells[it];
			if(!cell.Entities.empty() && cell.aabb.isOverlapping(query))
			{
				for(T* en : cell.Entities)
				{
					if(b2DistanceSquared(en->getPosition(), pos), radius*radius)
					{
						res.push_back(en);
					}
				}
			}
		}
	}

	size_t PosToIndex(b2Vec2 pos) const
	{
		size_t id = size_t(X * pos.x / this->width) + (size_t(Y*pos.y / this->height) * X);
		return id >= X*Y ? X*Y - 1u : id;
	}

	void AddEntity(T* e)
	{
		if(!e) return;
		this->cells[this->PosToIndex(e->getPosition())].Entities.push_back(e);
	}

	void UpdateEntity(T* e, b2Vec2 oldPos)
	{
		size_t oldI = this->PosToIndex(oldPos);
		size_t newI = this->PosToIndex(e->getPosition());
		if(oldI == newI) return;
		this->cells[oldI].Entities.erase(std::remove(this->cells[oldI].Entities.begin(), this->cells[oldI].Entities.end(), e), this->cells[oldI].Entities.end());
		this->cells[newI].Entities.push_back(e);
	}
};

