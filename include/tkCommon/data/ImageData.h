#pragma once
#include <mutex>
#include <tkCommon/common.h>
#include "SensorData.h"

namespace tk{namespace data{

    template <class T>
    class ImageData : public SensorData{

    public:
        T*  data = nullptr;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::mutex *mtx = nullptr; // mutex contructor is marked delete, so you cant copy the struct containing mutex

        bool gen_tex = false;
        unsigned int texture;
        int index = 0;

        void init(){
            SensorData::init();
            header.name = sensorName::CAMDATA;
        }

        void init(int w, int h, int ch){
        	SensorData::init();

            mtx->lock();
            width = w;
            height = h;
            channels = ch;
            data = new T[width*height*channels];
            mtx->unlock();
            header.name = sensorName::CAMDATA;
        }

        bool empty() {return channels == 0 || width == 0 || height == 0 || data == nullptr; }

		bool checkDimension(SensorData *s){
			auto *t = dynamic_cast<ImageData<T>*>(s);
			if(t->width != width || t->height != height || t->channels != channels){
				return false;
			}
        	return true;
        }

        ImageData<T>& operator=(const ImageData<T>& s){
        	SensorData::operator=(s);
			index = s.index;
            //if(s.width != width || s.height != height || s.channels != channels){
            //    release();
            //    init(s.width, s.height, s.channels);
            //}
            mtx->lock();
            memcpy(data, s.data, width * height * channels * sizeof(T));
            mtx->unlock();

            if(gen_tex == false){
				texture = s.texture;
				gen_tex = s.gen_tex;
            }

            return *this;
        }

        void release(){
            if(empty())
                return;

            mtx->lock();
            T* tmp = data;
            data = nullptr;
            width = 0;
            height = 0;
            channels = 0;
            delete [] tmp;
            glDeleteTextures(1,&texture);
            gen_tex = false;
            mtx->unlock();
        }

        void toGL(){
			if(empty()){
				tk::tformat::printMsg("Viewer","Image empty\n");
			}else{

                //use fast 4-byte alignment (default anyway) if possible
                glPixelStorei(GL_UNPACK_ALIGNMENT, ( (width*sizeof(uint8_t)) & 3) ? 1 : 4);

                //set length of one complete row in data (doesn't need to equal image.cols)
                glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                    
                unsigned int internal_format = 0;
                unsigned int image_format = 0;
                if(channels == 4) {
                    internal_format = GL_RGB;
                    image_format = GL_RGBA;      
                }else if(channels == 3){
                    internal_format = GL_RGB;
                    image_format = GL_RGBA;         
                } else if(channels == 1){
                    GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_RED};
                    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
                    internal_format = GL_RGB;
                    image_format = GL_RED;
                } else {
                    tkFATAL("channels num not supported: " + std::to_string(channels) );
                }
                glTexImage2D(GL_TEXTURE_2D,       
                    0,                   
                    internal_format,              
                    width,         
                    height,        
                    0,                 
                    image_format,             
                    GL_UNSIGNED_BYTE,   
                    data);  
                glBindTexture(GL_TEXTURE_2D, 0);
			}
        };

        ImageData() {
            mtx = new std::mutex();
        }

        ~ImageData(){
            release();
            delete mtx;
        }

		void draw2D(tk::gui::Viewer *viewer) {

        	if(!gen_tex){
				gen_tex = true;
				glGenTextures(1, &texture);
        	}
			this->toGL();

        	if(tk::gui::Viewer::image_width != this->width)
				tk::gui::Viewer::image_width = this->width;
			if(tk::gui::Viewer::image_height != this->height)
				tk::gui::Viewer::image_height = this->height;

			viewer->tkDrawTextureImage(texture, index);
        }

    };
}}