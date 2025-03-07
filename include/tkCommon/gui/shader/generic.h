#pragma once
#include <glm/glm.hpp>
#include "tkCommon/gui/utils/Shader.h"
#include "tkCommon/gui/utils/CommonViewer.h"


namespace tk { namespace gui { namespace shader {

/**
 * @brief shader generic class
 * 
 */
class generic
{
    protected:
        tk::gui::Shader shader;
        std::vector<tk::gui::vertexAttribs_t> vertexPointer;
};

}}}