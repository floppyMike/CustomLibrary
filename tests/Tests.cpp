#include <gtest/gtest.h>

#include <CustomLibrary/Collider.h>

using namespace ctl;

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
auto get_line_intersection(mth::Line<float> p1, mth::Line<float> p2) -> bool
{
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1.x2 - p1.x1;
	s1_y = p1.y2 - p1.y1;
	s2_x = p2.x2 - p2.x1;
	s2_y = p2.y2 - p2.y1;

	const float r = -s2_x * s1_y + s1_x * s2_y;

	const float s = (-s1_y * (p1.x1 - p2.x1) + s1_x * (p1.y1 - p2.y1)) / r;
	const float t = (s2_x * (p1.y1 - p2.y1) - s2_y * (p1.x1 - p2.x1)) / r;

	return s >= 0 && s <= 1 && t >= 0 && t <= 1;
}

TEST(GeometryTestLineLineCollision, T1)
{
	mth::Line<float> p1 = { 0, 0, 10, 10 };
	mth::Line<float> p2 = { 0, 10, 10, 0 };

	EXPECT_TRUE(mth::collision(p1, p2));
}

TEST(GeometryTestLineLineCollision, T2)
{
	mth::Line<float> p1 = { 0, 1, 2, 1 };
	mth::Line<float> p2 = { 1, 2, 1, 0 };

	EXPECT_TRUE(mth::collision(p1, p2));
}

TEST(GeometryTestLineLineCollision, T3)
{
	mth::Line<float> p1 = { 0, 0, 0, 1 };
	mth::Line<float> p2 = { 1, 1, 1, 0 };

	EXPECT_FALSE(mth::collision(p1, p2));
}

TEST(GeometryTestLineLineCollision, T4)
{
	mth::Line<float> p1 = { 1, 1, 5, 3 };
	mth::Line<float> p2 = { 3, 4, 7, 2 };

	EXPECT_TRUE(mth::collision(p1, p2));
}

// -----------------------------------------------------------------------------
// Test End
// -----------------------------------------------------------------------------

auto main(int argc, char **argv) -> int
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
