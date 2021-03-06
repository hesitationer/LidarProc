/*
 * @Descripttion: 
 * @version: 1.0版本
 * @Author: Frank.Wu
 * @Date: 2019-11-18 21:31:07
 * @LastEditors: Frank.Wu
 * @LastEditTime: 2020-06-09 11:39:41
 */
#ifdef _USE_PCL_

#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h>
#include <pcl/keypoints/narf_keypoint.h>
#include <pcl/features/narf_descriptor.h>
/**
 * @brief  使用PCL库对点云数据进行特征点提取
 * @note   
 * @retval None
 */
class LidarFeaturePoints
{
public:
    /**
    * @brief  通过PCL库计算FPHF特征
    * @note   
    * @param  input_cloud: 输入PCL格式点云数据
    * @param  narfIndex: 特征点位置
    * @param  narfDesc：特征描述
    * @retval 返回构建的特征信息
    * @Desc 
            NARF　
            从深度图像(RangeImage)中提取NARF关键点
            1. 边缘提取：
                对点云而言，场景的边缘代表前景物体和背景物体的分界线。所以，点云的边缘又分为三种：
                前景边缘，背景边缘，阴影边缘。 就是点a 和点b 如果在 rangImage 上是相邻的，然而
                在三维距离上却很远，那么多半这里就有边缘。
            2. 其他因素考虑：
                在提取关键点时， 边缘应该作为一个重要的参考依据。但一定不是唯一的依据。对于某个物
                体来说关键点应该是表达了某些特征的点，而不仅仅是边缘点。所以在设计关键点提取算法时
                ，需要考虑到以下一些因素：边缘和曲面结构都要考虑进去；关键点要能重复； 关键点最好落
                在比较稳定的区域，方便提取法线。
            3. Harris角点提取：
                图像的Harris角点算子将图像的关键点定义为角点。角点也就是物体边缘的交点， harris算子
                利用角点在两个方向的灰度协方差矩阵响应都很大，来定义角点。

            既然关键点在二维图像中已经被成功定义且使用了，看来在三维点云中可以沿用二维图像的定义不过今
            天要讲的是另外一种思路，简单粗暴， 直接把三维的点云投射成二维的图像不就好了。这种投射方法
            叫做range_image.
    */
    long LidarFeature_NARF(pcl::PointCloud<pcl::PointXYZ>::Ptr input_cloud,
                           pcl::PointCloud<int> &narfIndex,
                           pcl::PointCloud<pcl::Narf36> &narfDesc);

    /**
     * @brief  SIFT特征点提取算法应用于激光点云特征点提取
     * @note   
     * @param  input_cloud: 输入点云数据
     * @param  &siftPointIdx: 提取的特征点点云的位置
     * @param  siftFPFH: FPFH特征
     * @retval 
     */
    long LidarFeature_Sift(pcl::PointCloud<pcl::PointXYZ>::Ptr input_cloud,
                           pcl::PointCloud<int> &siftPointIdx,
                           pcl::PointCloud<pcl::FPFHSignature33>::Ptr siftFPFH);

};

/**
 * @brief  对提取的特征进行匹配
 * @note   
 * @retval None
 */

struct MATCHHISTRODIS
{
    int idx1;
    int idx2;
    double relation;
};

class LidarFeatureRegistration
{
public:
    /**
    * @brief  根据SIFT算子提取的FPFH特征进行匹配
    * @note   
    * @param  siftFPFH1: 点集1中的FPFH特征
    * @param  siftFPFH2: 点集2中的FPFH特征
    * @param  &siftMatchPointIdx: 匹配点的ID
    * @retval 
    */
    long LidarRegistration_Sift(pcl::PointCloud<pcl::FPFHSignature33>::Ptr siftFPFH1,
                                pcl::PointCloud<pcl::FPFHSignature33>::Ptr siftFPFH2,
                                pcl::PointCloud<int> &siftMatchPointIdx);


    /**
     * @name: find the most match point pair and correlation value
     * @msg: 
     * @param pcl::PointCloud<int> idxList1：feature index 1
              pcl::PointCloud<int> idxList2：feature index 2
              pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1：point cloud 1
              pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2：point cloud 2
               int matchNum: match number of points
              std::vector<MATCHHISTRODIS> &matches：得到的匹配结果
     * @return: 
     */
    long LidarRegistration_Match(pcl::PointCloud<int> idxList1,
                                 pcl::PointCloud<int> idxList2,
                                 pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                                 pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2,
                                 int matchNum,
                                 std::vector<MATCHHISTRODIS> &matches);
                                 
#ifdef _USE_CERES_
    /**
     * @name: 根据匹配点计算旋转矩阵，
     *        这个需要考虑如果有误匹配应该怎么处理?
     * @msg: 
     * @param {type} 
     * @return: 
     */
    long LidarRegistration_RotTrans(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                                    pcl::PointCloud<int> ptSiftIdx1,
                                    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2,
                                    pcl::PointCloud<int> ptSiftIdx2,
                                    pcl::PointCloud<int> siftMatchPointIdx,
                                    double *r_t);
#endif 

private:
    /**
     * @name: compute the correlation of two points in two datasets
     * @msg: 
     * @param pcl::PointXYZ pnt1
     *        pcl::PointXYZ pnt2
     *        pcl::KdTreeFLANN<pcl::PointXYZ> kdtree1
     *        pcl::KdTreeFLANN<pcl::PointXYZ> kdtree2
     *        int nearPointsNum:
     * @return: correlation
     */
    double LidarRegistration_CorrelationMatch(pcl::PointXYZ pnt1,pcl::PointXYZ pnt2,
                                         pcl::KdTreeFLANN<pcl::PointXYZ> kdtree1,
                                         pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,
                                         pcl::KdTreeFLANN<pcl::PointXYZ> kdtree2,
                                         pcl::PointCloud<pcl::PointXYZ>::Ptr cloud2,
                                         int nearPointsNum=10);                    

    /**
     * @name:calculate correlation of two dataset 
     * @msg: 
     * @param {type} 
     * @return: 
     */
    double LidarRegistration_Correlation(double* data1,double *data2,int num);

    /**
     * @name: get distance histrogram for a point in near point dataset
     * @msg: 
     * @param {type} 
     * @return: 
     */
    long LidarRegistration_DisHistro(int idxPnt,std::vector<int> idxPntAllNear,pcl::PointCloud<pcl::PointXYZ>::Ptr cloud1,double *disHistro);
};

#endif