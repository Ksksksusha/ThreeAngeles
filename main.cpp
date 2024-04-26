#include "triangle.hpp"
#include "point.hpp"
#include "vector.hpp"
#include "octree.hpp"
#include "app.hpp"
#include "model.hpp"
#include <iostream>
#include <vector>
#include <array>
#include "chrono"
#include <set>

using namespace geometry;

int main()
{
    int triag_num = 0;
    if (triag_num < 0)
    {
        std::cout << "nuber of triags can't be negative" << std::endl;
        return -1;
    }

    std::cin >> triag_num;
    std::vector<octrees::triag_id_t> triags;

    if (triag_num > 0)
    {
        for (size_t i = 0; i < static_cast<size_t>(triag_num); i++)
        {
            double a = 0, b = 0, c = 0;
            std::cin >> a >> b >> c;
            point_t p1{a, b, c};
            std::cin >> a >> b >> c;
            point_t p2{a, b, c};
            std::cin >> a >> b >> c;
            point_t p3{a, b, c};

            triags.push_back(octrees::triag_id_t{triangle_t{p1, p2, p3}, i});
        }
    }
    else
        std::cerr << "Triangle number should be greater than 0" << std::endl;

    // const std::clock_t start = clock();

    octrees::octree_t octree(triags);

    std::vector<bool> answer;
    answer.reserve(triag_num);

    for (int i = 0; i < triag_num; i++)
        answer[i] = false;

    octree.get_collisions(answer);

    for (int i = 0; i < triag_num; i++)
        if(answer[i]) std::cout << i << std::endl;

    // std::cout << "Total time is " << (clock() - start) / (double) CLOCKS_PER_SEC << std::endl;

    glm::vec3 red_color     = {1.f, 0.f, 0.f};

    glm::vec3 blue_color    = {0.f, 0.f, 1.f};

    glm::vec3 color         = {0.f, 0.f, 0.f};

    std::vector<Vulkan::Model::Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(triags.size() * 3);
    indices.reserve(triags.size() * 3 * 2);

    for (size_t i = 0, triangle = 0; i < triag_num; i++, triangle++)
    {
        if (answer[i])
        {
            color = red_color;

            std::cout << i << std::endl;
        }

        else
            color = blue_color;

        vertices.push_back({Vulkan::GetGlmVector(triags[triangle].triag.getA()), color, Vulkan::GetNormal(triags[triangle].triag)});

        vertices.push_back({Vulkan::GetGlmVector(triags[triangle].triag.getB()), color, Vulkan::GetNormal(triags[triangle].triag)});

        vertices.push_back({Vulkan::GetGlmVector(triags[triangle].triag.getC()), color, Vulkan::GetNormal(triags[triangle].triag)});
    }

    for (int i = 0; i != triags.size() * 3; ++i)
        indices.push_back(i);

    Vulkan::App app(vertices, indices);

    app.run();

    return 0;
}