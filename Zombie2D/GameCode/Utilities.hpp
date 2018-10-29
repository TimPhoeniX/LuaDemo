#pragma once

inline SGE::Shape* getCircle()
{
	static SGE::Shape* circle = SGE::Shape::Circle(0.5f);
	return circle;
}

inline bool LineIntersection(b2Vec2 a, b2Vec2 b,
					  b2Vec2 c, b2Vec2 d,
					  float& distToIp, b2Vec2& point)
{
	float det = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);
	if(det == 0.f) return false;
	float invDet = 1.f / det;
	float s = ((a.y - c.y) * (d.x - c.x) - (a.x - c.x)*(d.y - c.y)) * invDet;
	float t = ((a.y - c.y) * (b.x - a.x) - (a.x - c.x)*(b.y - a.y)) * invDet;
	if(s > 0.f && s < 1.f && s > 0 && s < 1)
	{
		distToIp = b2Distance(a, b) * s;
		point = a + s * (b - a);
		return true;
	}
	else
	{
		distToIp = 0;
		return false;
	}
}

inline b2Vec2 PointToWorldSpace(const b2Vec2& point, const b2Vec2& heading, const b2Vec2& position)
{
	auto orientation = heading.Orientation();
	auto result = b2Mul(b2Rot(orientation), point);
	return result + position;
}

inline b2Vec2 PointToLocalSpace(const b2Vec2& point, const b2Vec2& heading, const b2Vec2& position)
{
	auto orientation = heading.Orientation();
	auto result = point - position;
	return b2Mul(b2Rot(-orientation), result);
}

inline b2Vec2 VectorToWorldSpace(const b2Vec2& vec, const b2Vec2& heading)
{
	return b2Mul(b2Rot(heading.Orientation()), vec);
}