#ifndef INC_DETECT_CIRCLES__
#define INC_DETECT_CIRCLES__

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

#include <vector>

class BaseHoughCircles
{
   public:

      BaseHoughCircles(const char *filename);
      virtual ~BaseHoughCircles() {}
      virtual int detectCircles(std::vector <cv::Vec3f> &circlesData)
      {
	 circlesData.clear();
	 return -1;
      }
   protected:
      void createHSVTrackbarsWindow(void);

      int H_MIN;
      int H_MAX;
      int S_MIN;
      int S_MAX;
      int V_MIN;
      int V_MAX;
      cv::VideoCapture video;
};

class GpuHoughCircles : public BaseHoughCircles
{
   public:
      GpuHoughCircles(const char *filename);
      ~GpuHoughCircles() {}
      void createHSVTrackbarsWindow(void);

      void cvOpen(const cv::gpu::GpuMat &gpu_image, cv::gpu::GpuMat &gpuMorphOpsOut, cv::gpu::Stream &gpuStream);
      void generateThreshold(const cv::gpu::GpuMat &imageIn, cv::gpu::GpuMat &imageOut, cv::gpu::Stream &gpuStream);
      int detectCircles(std::vector <cv::Vec3f> &circlesData);
   private:
      cv::Mat frame;
      cv::gpu::GpuMat gpuFrame;
      cv::gpu::GpuMat gpuPyrDownFrame;

      cv::gpu::GpuMat gpuMorphOpsTemp;
      cv::gpu::GpuMat gpuMorphOpsOut;
      //cv::gpu::GpuMat gpuErodeBuf;
      //cv::gpu::GpuMat gpuDilateBuf;
      cv::gpu::GpuMat gpuHoughCirclesOut;
      cv::gpu::HoughCirclesBuf gpuHoughCirclesBuffer;

      cv::gpu::GpuMat gpuThresholdImageOut;
      cv::gpu::GpuMat gpuThresholdLocalImage;

      std::vector <cv::gpu::GpuMat> gpuSplitImage;
      cv::gpu::GpuMat gpuSplitImageGE;
      cv::gpu::GpuMat gpuSplitImageLE;
      //cv::gpu::GpuMat gpuAndMask;

      //cv::gpu::GpuMat gpuThresholdImageIn;

      cv::gpu::CudaMem cudaMemIn[1];
      //cv::gpu::CudaMem cudaMemOut[2];
      unsigned int cudaMemIndex;

      bool outputDataAvailable;
};

class CpuHoughCircles : public BaseHoughCircles
{
   public:
      CpuHoughCircles(const char *filename);
      ~CpuHoughCircles() {}
      void makeCircles(const cv::Mat &cpu_image, std::vector<cv::Vec3f> &circle_list);
      void generateThreshold(const cv::Mat &imageIn, cv::Mat &imageOut);

      int detectCircles(std::vector <cv::Vec3f> &circlesData);
};

#endif
