
#pragma once
#include <iostream>
#include <cstdio>
#include <vector>
#include <time.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <initializer_list>
#include "tkCommon/common.h"
#include "tkCommon/cuda.h"
#include "tkCommon/math/MatSimple.h"
#include "tkCommon/math/MatIO.h"

namespace tk { namespace math {

// GPU INFO
static int  Mat_device = -1;
static bool Mat_integrated_memory = false;

/**
 * @brief   Matrix in cuda. Matrix in COLS order.
 * @tparam  T matrix class type
 */
template<class T>
class Mat : public tk::math::MatDump {

protected:
    bool    _use_gpu;

    void init() {
        _use_gpu = false;

#ifdef TKCUDA_ENABLED
        // get GPU type
        if(Mat_device == -1) {
            cudaDeviceProp  props;
            tkCUDA(cudaGetDevice(&Mat_device));
            tkCUDA(cudaGetDeviceProperties(&props,Mat_device));
            Mat_integrated_memory = props.integrated != 0;
        }
        gpu.integrated_memory = Mat_integrated_memory;
#endif
    }

public:
    MatSimple<T, false> cpu;
    MatSimple<T, true>  gpu;

    Mat(void) {
        init();
    }
    
    // copy constructor for std::vector
    Mat(const Mat &m) {
        init();
        *this = m;
    }


    Mat(T *data_h, T *data_d, int r, int c) {
        init();
        if(data_h != nullptr) {
            cpu.release();
            cpu.owned = true;
            cpu = MatSimple<T, false>(data_h, r, c);
        }
        if(data_d != nullptr) {
            gpu.release();
            gpu.owned = true;
            _use_gpu = true;
            gpu = MatSimple<T, true>(data_d, r, c);
        }
    }

    //template<typename OtherDerived>
    //Mat(const Eigen::MatrixBase<OtherDerived>& other) : Eigen::Matrix<T,-1,-1>(other) { 
    //    init();
    //}

    __host__ 
    void bind(const Mat<T>& s) {
        cpu.bind(s.cpu);
        gpu.bind(s.gpu);
    }

    template<typename OtherDerived>
    Mat<T>& operator=(const Eigen::MatrixBase <OtherDerived>& other) {
        cpu.resize(other.rows(), other.cols());
        Eigen::Map<Eigen::Matrix<T,-1,-1>> m(cpu.data, cpu.rows, cpu.cols);
        m = other;
        return *this;
    }

    __host__
    ~Mat() {
        cpu.release();
        gpu.release();
    }


    __host__ void
    useGPU(){
        _use_gpu = true;
        resize(rows(),cols());
    }

    __host__ bool
    hasGPU() const {
        return _use_gpu;
    }

    __host__ void 
    copyFrom(T*data, int r, int c) {
        resize(r, c);
        memcpy(cpu.data, data, size()*sizeof(T));
        if(_use_gpu == true){
            synchGPU();
        }
    }

    __host__ void 
    copyTo(T*data) {
        memcpy(data, cpu.data, size()*sizeof(T)); 
    }
    
    __host__ void 
    synchGPU(){ 
        useGPU();
        tkCUDA( cudaMemcpy(gpu.data, cpu.data, size() * sizeof(T), cudaMemcpyHostToDevice) ); 
    }

    __host__ void 
    synchCPU(){ 
        tkASSERT(_use_gpu == true, "You set mat only on CPU\n");
        resize(gpu.rows, gpu.cols);
        tkCUDA( cudaMemcpy(cpu.data, gpu.data, size() * sizeof(T), cudaMemcpyDeviceToHost) ); 
    }
    
    __host__ int
    rows() const { return cpu.rows; }

    __host__ int
    cols() const { return cpu.cols; }

    __host__ int
    size() const { return cpu.size; }

    __host__ T*
    data() { return cpu.data; }

    __host__ const T*
    data() const { return cpu.data; }

    __host__ void 
    resize(int r, int c) {
        cpu.resize(r,c);
        if(_use_gpu == true){
            gpu.resize(r, c);
        }
    }

    __host__ T
    operator()(int i, int j) const {
        return cpu.data[i+j*cpu.rows]; 
    }

    __host__ T&
    operator()(int i, int j) {
        return cpu.data[i+j*cpu.rows]; 
    }

    __host__ T&
    operator[](int n) {
        return cpu.data[n]; 
    }

    __host__ Mat<T>& 
    operator=(const Mat<T>& s) {
        cpu = s.cpu;
        if(_use_gpu) {
            this->synchGPU();
        }
        return *this;
    }

    __host__
    const Eigen::Map<const Eigen::Matrix<T,-1,-1>> matrix() const {
        return Eigen::Map<const Eigen::Matrix<T,-1,-1>>(cpu.data, cpu.rows, cpu.cols);
    }
    
    __host__
    Eigen::Map<Eigen::Matrix<T,-1,-1>> writableMatrix() {
        return Eigen::Map<Eigen::Matrix<T,-1,-1>>(cpu.data, cpu.rows, cpu.cols);
    }

    friend std::ostream& 
    operator<<(std::ostream& os, Mat<T>& s) {
        if(s.size() <= 16)
            s.print(os);
        else
            os<<"Mat ("<<s.rows()<<"x"<<s.cols()<<")";
        return os;
    }

    __host__ void 
    print(std::ostream &os = std::cout) {
        int r = rows();
        int c = cols();
        os<<"Mat ("<<r<<"x"<<c<<")\n";
        for(int i = 0; i < r; ++i) {
            os << std::endl;
            for(int j = 0; j < c; ++j) {
                os << std::right << std::setw(20) << cpu.data[i+j*r];
            }
        }
        os<<std::endl;
    }

    __host__
    void fill(float val) {
        for(int i=0; i<cpu.size; i++)
            cpu.data[i] = val;
    }


    bool toVar(std::string name, MatIO::var_t &var) {
        return var.set(name, cpu);
    }
    bool fromVar(MatIO::var_t &var) {
        if(var.empty())
            return false;
        return var.get(cpu);
    }

};

//#pragma pack(push, 1)
template<class T, int R, int C>
class MatStatic : public tk::math::MatDump {
    
protected:
    T mData[R*C];

public:
    MatStatic() {
    }

    template<typename OtherDerived>
    MatStatic<T,R,C>& operator=(const Eigen::MatrixBase <OtherDerived>& other) {
        tkASSERT(R == other.rows() && C == other.cols());
        Eigen::Map<Eigen::Matrix<T,R,C>> m(this->mData);
        m = other;
        return *this;
    }

    __host__ int
    rows() const { return R; }

    __host__ int
    cols() const { return C; }

    __host__ int
    size() const { return R*C; }

    __host__ T*
    data() { return this->mData; }

    __host__ const T*
    data() const { return this->mData; }

    friend std::ostream& 
    operator<<(std::ostream& os, MatStatic<T,R,C>& s) {
        if(s.size() <= 16)
            s.print(os);
        else
            os<<"MatStatic ("<<s.rows()<<"x"<<s.cols()<<")";
        return os;
    }

    __host__ void 
    print(std::ostream &os = std::cout) {
        int r = rows();
        int c = cols();
        os<<"MatStatic ("<<r<<"x"<<c<<")\n";
        for(int i = 0; i < r; ++i) {
            os << std::endl;
            for(int j = 0; j < c; ++j) {
                os << std::right << std::setw(20) << mData[i+j*r];
            }
        }
        os<<std::endl;
    }

    __host__
    const Eigen::Map<const Eigen::Matrix<T,R,C>> matrix() const {
        return Eigen::Map<const Eigen::Matrix<T,R,C>>(mData);
    }
    
    __host__
    Eigen::Map<Eigen::Matrix<T,R,C>> writableMatrix() {
        return Eigen::Map<Eigen::Matrix<T,R,C>>(mData);
    }


    bool   toVar(std::string name, MatIO::var_t &var) {
        Eigen::Matrix<T,R,C> m = matrix();
        return var.set(name, m);
    }
    bool fromVar(MatIO::var_t &var) {
        if(var.empty())
            return false;
        Eigen::Matrix<T,R,C> m;
        bool ok = var.get(m);
        *this = m;
        return ok;
    }

    __host__ T&
    operator()(int i, int j) {
        return mData[i+j*R]; 
    }

    __host__ T&
    operator[](int n) {
        return mData[n]; 
    }
};
//#pragma pack(pop)


typedef MatStatic<double,2,2> Mat2d;
typedef MatStatic<double,3,3> Mat3d;
typedef MatStatic<double,4,4> Mat4d;

typedef MatStatic<int,2,2> Mat2i;
typedef MatStatic<int,3,3> Mat3i;
typedef MatStatic<int,4,4> Mat4i;

typedef MatStatic<float,2,2> Mat2f; 
typedef MatStatic<float,3,3> Mat3f; 
typedef MatStatic<float,4,4> Mat4f; 

}}