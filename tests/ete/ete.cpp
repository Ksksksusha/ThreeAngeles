#include <ctime>
#include <vector>
#include <fstream>
#include <set>

#include "triangle.hpp"
#include "octree.hpp"
#include "coordinates.hpp"
#include "intersection.hpp"

//-------------------------------------------------------------------------------//

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Incorrect value of args" << std::endl;
        return -1;
    }

    std::ifstream test_file;

    test_file.open(argv[1]);

    size_t tr_numbers = 0;

    test_file >> tr_numbers;

    const std::clock_t start = clock();

    std::list<Triangle> triangles;

    double BoundingBoxX = 0, BoundingBoxY = 0, BoundingBoxZ = 0;

    for (int i = 0; i < tr_numbers; i++)
    {
        double p1x = 0, p1y = 0, p1z = 0, p2x = 0, p2y = 0, p2z = 0, p3x = 0, p3y = 0, p3z = 0;

        test_file >> p1x >> p1y >> p1z;
        Point p1(p1x, p1y, p1z);

        test_file >> p2x >> p2y >> p2z;
        Point p2(p2x, p2y, p2z);

        test_file >> p3x >> p3y >> p3z;
        Point p3(p3x, p3y, p3z);

        Triangle t(p1, p2, p3);

        t.number = i;

        triangles.push_back(t);

        if (BoundingBoxX < std::max(std::abs(p1.X()), std::max(std::abs(p2.X()), std::abs(p3.X()))))
            BoundingBoxX = std::max(std::abs(p1.X()), std::max(std::abs(p2.X()), std::abs(p3.X())));

        if (BoundingBoxY < std::max(std::abs(p1.Y()), std::max(std::abs(p2.Y()), std::abs(p3.Y()))))
            BoundingBoxY = std::max(std::abs(p1.Y()), std::max(std::abs(p2.Y()), std::abs(p3.Y())));

        if (BoundingBoxZ < std::max(std::abs(p1.Z()), std::max(std::abs(p2.Z()), std::abs(p3.Z()))))
            BoundingBoxZ = std::max(std::abs(p1.Z()), std::max(std::abs(p2.Z()), std::abs(p3.Z())));
    }

    Point bounding_box(BoundingBoxX, BoundingBoxY, BoundingBoxZ);

    OctreeNew oct(triangles, bounding_box);

    std::set<size_t> intersecting_triangles;

    bool *FlagArray = new bool [tr_numbers]{};

    FindIntersectionsInNode(oct.root_, FlagArray);

    for (size_t i = 0; i < tr_numbers; i++)
    {
        if (FlagArray[i])
            std::cout << i << std::endl;
    }

    std::cout << "Total time is " << (clock() - start) / (double) CLOCKS_PER_SEC << std::endl;

    test_file.close();

    return 0;
}

//-------------------------------------------------------------------------------//
