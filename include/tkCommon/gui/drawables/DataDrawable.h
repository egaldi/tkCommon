#pragma once
#include "tkCommon/gui/drawables/Drawable.h"
#include "tkCommon/data/SensorData.h"
#include "tkCommon/rt/Pool.h"
#include "tkCommon/gui/shader/text.h"


namespace tk{ namespace gui{

class DataDrawable : public Drawable {
    public:
        DataDrawable();

        void draw(tk::gui::Viewer *viewer) final;

        //virtual void updateRef(tk::data::SensorData* data) final;

        virtual void setPool(tk::rt::DataPool *aPool) final;

        tk::data::SensorData* data;

        bool mDataEnableTf = false;
    protected:

        void forceUpdate();

        virtual void drawData(tk::gui::Viewer *viewer) = 0; 
        virtual void updateData(tk::gui::Viewer *viewer) = 0; 

        
        uint32_t counter;

        std::stringstream print;

        bool mDrwHasReference;
    private:

        //std::mutex  mPointerMutex;
        //bool        mNewPointer;

        bool mFirstData;

        bool mDrwHasPool;
        tk::rt::DataPool *mPool;
        int mPoolLastData;

        bool mFirstDraw;
        tk::gui::shader::text *mShaderText;
	};
}}