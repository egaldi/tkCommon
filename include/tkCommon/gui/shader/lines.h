#pragma once
/**
 * @file    stripline.h
 * @author  Luca Bartoli, Fabio bagni, Gatti Francesco, Massimiliano bosi (you@domain.com)
 * @brief   class that draw lines from points
 * @version 1.0
 * @date    2020-07-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "tkCommon/gui/shader/generic.h"

namespace tk { namespace gui { namespace shader {

/**
 * @brief class that draw a lines formed by [X Y Z R G B A ...]
 * 
 */
class lines : public tk::gui::shader::generic
{
    public:
        bool init(){
            std::string vertex      = std::string(tkCommon_PATH) + "include/tkCommon/gui/shader/glsl/lines.vert";
            std::string geometry    = "";
            std::string fragment    = std::string(tkCommon_PATH) + "include/tkCommon/gui/shader/glsl/lines.frag";
            
            bool status = shader.init(vertex, fragment, geometry);
            if(status == false)
                return false;

            vertexPointer.push_back({3,7,0});
            vertexPointer.push_back({4,7,3});

            vertexPointer.resize(2);
            return true;
        }

        void draw(tk::gui::Buffer<float>* buffer, int n, int size = 1.0f,GLenum linemode = GL_LINE_STRIP){

		    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));

            buffer->setVertexAttribs(vertexPointer);

            shader.use();
            shader.setMat4("modelview",modelview);

            buffer->use();
            glLineWidth(size);
            glDrawArrays(linemode, 0, n);
            glLineWidth(1.0f);
            buffer->unuse();

            shader.unuse();

            glCheckError();
        }

        bool close(){
            return shader.close();
        }
};

}}}