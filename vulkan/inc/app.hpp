#ifndef APP_HPP_
#define APP_HPP_

#include "device.hpp"
#include "object.hpp"
#include "window.hpp"
#include "render.hpp"
#include "render_system.hpp"
#include "keyboard.hpp"
#include "camera.hpp"
#include "uniform_buffer.hpp"
#include "descriptor.hpp"
#include "point.hpp"
#include "triangle.hpp"

#include <memory>
#include <vector>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

glm::vec3 GetGlmVector(const Point& point);

//-------------------------------------------------------------------------------//

glm::vec3 GetNormal(const Triangle& triangle);

//-------------------------------------------------------------------------------//

class App 
{
    public:
    
        static const int WIDTH  = 800;

        static const int HEIGHT = 600;

        App(const Model::Builder& builder, const std::pair<Point, float> lightParametres);
        
        ~App();

        App(const App &) = delete;
        
        App &operator=(const App &) = delete;

        void RunApplication();

    private:
    
        void LoadObjects(const Model::Builder& builder);

        std::pair<Point, float> lightParametres_;
        
        std::unique_ptr<Model> CreateTriangleModel(Device &device, glm::vec3 offset, const Model::Builder& builder);
        
        Window                          window_{WIDTH, HEIGHT, "Triangles"};
        
        Device                          device_{window_};

        Render                          render_{window_, device_};

        std::unique_ptr<DescriptorPool> globalPool_{};
        
        std::vector<Object>             objects_;
};

//-------------------------------------------------------------------------------//

}  // end of Vulkan namespace 

#endif