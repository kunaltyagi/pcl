/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2016-, Open Perception, Inc.
 *  Copyright (c) 2016, Voxar Labs, CIn-UFPE / DEINFO-UFRPE
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <pcl/test/gtest.h>
#include <pcl/point_cloud.h>
#include <pcl/features/gasd.h>
#include <pcl/io/pcd_io.h>

pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
createColorCloud (pcl::PointCloud<pcl::PointXYZRGBA> &colorCloud)
{
  for (std::size_t i = 0; i < cloud->size (); ++i)
  {
    pcl::PointXYZRGBA p;
    p.getVector3fMap () = cloud->points[i].getVector3fMap ();

    p.rgba = ( (i % 255) << 16) + ( ( (255 - i) % 255) << 8) + ( (i * 37) % 255);
    colorCloud.push_back (p);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, GASDTransformEstimation)
{
  pcl::GASDEstimation<pcl::PointXYZ, pcl::GASDSignature512> gasd;
  gasd.setInputCloud (cloud);

  pcl::PointCloud<pcl::GASDSignature512> descriptor;
  gasd.compute (descriptor);

  Eigen::Matrix4f trans = gasd.getTransform ();

  Eigen::Matrix4f ref_trans;
  ref_trans << 0.661875, -0.704840, 0.255192, 0.0846344,
  -0.748769, -0.605475, 0.269713, 0.0330151,
  -0.035592, -0.369596, -0.928511, 0.0622551,
  0, 0, 0, 1;

  for (Eigen::Index i = 0; i < trans.rows(); ++i)
  {
    for (Eigen::Index j = 0; j < trans.cols (); ++j)
    {
      EXPECT_NEAR (trans (i, j), ref_trans (i, j), 1e-5);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST (PCL, GASDShapeEstimationNoInterp)
{
  pcl::GASDEstimation<pcl::PointXYZ, pcl::GASDSignature512> gasd;
  gasd.setInputCloud (cloud);
  gasd.setShapeHistsInterpMethod (pcl::INTERP_NONE);

  pcl::PointCloud<pcl::GASDSignature512> descriptor;
  gasd.compute (descriptor);

  const float ref_values[512] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.0202, 0, 0, 0, 0, 0, 0, 1.76768, 1.26263, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 2.77778, 0, 0, 0, 0, 0, 0, 1.0101, 2.0202, 0, 0, 0, 0, 0, 0, 2.0202, 0.757576, 0, 0,
    0, 0, 0, 1.0101, 3.28283, 0, 0, 0, 0, 0, 0, 0, 1.51515, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.51515, 0, 0, 0, 0, 0, 0, 0, 1.51515, 0, 0, 0, 0, 0, 0,
    2.27273, 0.757576, 0, 0, 0, 0, 0, 2.0202, 2.0202, 0, 0, 0, 0, 0, 0, 1.0101, 2.27273, 2.27273, 0, 0, 0, 0, 0,
    0, 0, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1.0101, 0, 0, 0, 0, 0, 0, 0, 2.77778, 0.252525, 0, 0, 0, 0, 0, 0, 2.77778, 0, 0, 0, 0, 0, 0, 0, 2.0202,
    1.0101, 0.252525, 0, 0, 0, 0, 0, 0, 2.0202, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0.757576, 0, 0, 0, 0, 0, 0, 0, 3.0303, 0, 0, 0, 0, 0, 0, 0, 2.77778, 0, 0, 0, 0, 0, 0, 0, 2.52525, 0, 0, 0, 0,
    0, 0, 0, 1.0101, 4.29293, 1.26263, 0, 0, 0, 0, 0, 0, 1.0101, 1.76768, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0.757576, 1.26263, 0, 0, 0, 0, 0, 0, 2.77778, 0.252525, 0, 0, 0, 0, 0, 0, 2.52525, 0,
    0, 0, 0, 0, 0, 0, 2.77778, 0.252525, 0, 0, 0, 0, 0, 0, 0.50505, 3.53535, 0, 0, 0, 0, 0, 0, 0, 1.0101, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.51515, 3.53535, 0, 0, 0, 0,
    0, 0, 2.0202, 1.51515, 0, 0, 0, 0, 0, 0, 1.76768, 1.26263, 0, 0, 0, 0, 0, 0, 0.757576, 0.757576, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2.0202, 0.252525, 0, 0, 0, 0, 0, 0, 2.52525, 0, 0, 0, 0, 0, 0, 0, 0.50505, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  EXPECT_EQ (descriptor.points.size (), 1);
  for (std::size_t i = 0; i < std::size_t (descriptor.points[0].descriptorSize ()); ++i)
  {
    EXPECT_NEAR (descriptor.points[0].histogram[i], ref_values[i], 1e-5);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, GASDShapeEstimationTrilinearInterp)
{
  pcl::GASDEstimation<pcl::PointXYZ, pcl::GASDSignature512> gasd;
  gasd.setInputCloud (cloud);

  pcl::PointCloud<pcl::GASDSignature512> descriptor;
  gasd.compute (descriptor);

  const float ref_values[512] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00836128, 0.00588401, 0, 0, 0, 0, 0, 0.107094, 0.662119, 0.175875, 0,
    0, 0, 0, 0.00971758, 0.839464, 0.58815, 0.0212094, 0, 0, 0, 0, 0.231226, 0.0734101, 0.0136519, 0, 0, 0, 0, 0,
    0.0148243, 0.00481199, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0175941,
    0.00421349, 0, 0, 0, 0, 0, 0.283015, 1.10961, 0.0363255, 0, 0, 0, 0, 0, 0.330514, 0.765326, 0.0400337, 0, 0,
    0, 0, 0.113547, 0.602405, 0.760736, 0.162908, 0, 0, 0, 0, 0.19996, 0.568878, 0.0862251, 0.00529312, 0, 0, 0,
    0, 0.029714, 0.0622333, 0.0943459, 0, 0, 0, 0, 0, 0, 0.000478281, 0.0027764, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0.0274657, 0.00816464, 0, 0, 0, 0, 0, 0.0368769, 0.609568, 0.0643177, 0, 0, 0, 0, 0, 0.231449,
    0.577855, 0.00143416, 0, 0, 0, 0, 0.150154, 1.03347, 0.499773, 0, 0, 0, 0, 0, 0.856571, 1.09567, 0.0753022, 0,
    0, 0, 0, 0, 0.589158, 1.39603, 1.21675, 0.000193459, 0, 0, 0, 0, 0, 0.0955702, 0.30635, 0.000289646, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.030262, 0.0172037, 0, 0, 0, 0, 0, 0.00457827,
    0.375081, 0.115732, 0, 0, 0, 0, 0, 0.0289331, 1.70761, 0.498999, 0, 0, 0, 0, 0, 0.0748183, 1.59429, 0.1418, 0,
    0, 0, 0, 0, 0.0890234, 1.12343, 1.32057, 0.192363, 2.03302e-005, 0, 0, 0, 0, 0.310746, 1.158, 0.215904,
    0.00137463, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000985736, 0.525969, 0.173892, 0, 0, 0,
    0, 0, 0.03184, 1.10013, 0.177279, 0, 0, 0, 0, 0, 0.157355, 1.27302, 0.0196858, 0, 0, 0, 0, 0, 0.0968957,
    1.44605, 0.111663, 0, 0, 0, 0, 0, 0.00114074, 0.414239, 1.5723, 0.589681, 0.000551233, 0, 0, 0, 0, 0.0759757,
    0.837368, 0.721145, 0.0346183, 0, 0, 0, 0, 0, 0.0230347, 0.00829717, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0.000667462, 0.265466, 0.211503, 0.0281879, 0, 0, 0, 0, 0.130565, 1.38486, 0.168072, 0.020685, 0, 0, 0, 0,
    0.242921, 0.93304, 0, 0, 0, 0, 0, 0, 0.0796441, 1.00082, 0.289627, 0, 0, 0, 0, 0, 0.000704473, 0.596791,
    1.22236, 0.015159, 1.08117e-005, 0, 0, 0, 0, 0.0205081, 0.214601, 0.0495967, 0.000464472, 0, 0, 0, 0, 0,
    0.0129949, 0.0046808, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00173052, 0.319008, 0.0513479, 0, 0, 0, 0,
    0.0156562, 0.232644, 1.43675, 0.106155, 0, 0, 0, 0, 0.0809344, 0.605434, 0.803232, 0.0557288, 0, 0, 0, 0,
    0.0272915, 0.607169, 0.366215, 0.0111251, 0, 0, 0, 0, 4.34266e-005, 0.141203, 0.0975747, 0, 0, 0, 0, 0, 0,
    0.000786102, 0.00152033, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000134866,
    0.0595355, 0.00785339, 0, 0, 0, 0, 0, 0.0386555, 0.582852, 0.0295824, 0, 0, 0, 0, 0, 0.118361, 1.33305,
    0.209958, 0, 0, 0, 0, 0, 0.0603114, 0.412731, 0.0294292, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

  EXPECT_EQ (descriptor.points.size (), 1);
  for (std::size_t i = 0; i < std::size_t (descriptor.points[0].descriptorSize ()); ++i)
  {
    EXPECT_NEAR (descriptor.points[0].histogram[i], ref_values[i], 1e-5);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST (PCL, GASDShapeAndColorEstimationNoInterp)
{
  // Create fake point cloud with colors
  pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudWithColors (new pcl::PointCloud<pcl::PointXYZRGBA>);
  createColorCloud (*cloudWithColors);

  pcl::GASDColorEstimation<pcl::PointXYZRGBA, pcl::GASDSignature984> gasd;
  gasd.setInputCloud (cloudWithColors);

  pcl::PointCloud<pcl::GASDSignature984> descriptor;
  gasd.compute (descriptor);

  const float ref_values[984] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.0101, 0, 0, 0, 0, 3.53535, 1.26263, 0, 0, 0,
    1.51515, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.757576, 0, 0, 0, 0, 0.757576, 6.06061, 0,
    0, 0, 0, 4.0404, 1.51515, 0, 0, 0, 0, 6.81818, 0, 0, 0, 0, 0, 1.76768, 1.76768, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0.50505, 0, 0, 0, 0, 0, 3.78788, 0.50505, 0, 0, 0, 0, 5.80808, 0, 0, 0, 0, 0, 2.0202,
    4.29293, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3.78788, 0.252525, 0, 0, 0, 0, 4.79798, 0, 0, 0, 0,
    0, 4.0404, 1.26263, 0, 0, 0, 0, 0.50505, 6.56566, 2.77778, 0, 0, 0, 0, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 2.27273, 3.0303, 0, 0, 0, 0, 4.79798, 0, 0, 0, 0, 0, 4.0404, 2.0202, 0, 0, 0, 0, 0.757576, 1.51515, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.27273, 0, 0, 0, 0, 0.50505, 5.80808, 0, 0, 0, 0, 0, 1.26263,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1.26263, 0.252525, 0.50505, 0.50505, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.757576, 1.0101, 0.757576, 0.50505, 0, 0, 0, 0, 0,
    0.252525, 1.51515, 1.51515, 0.252525, 0, 0.252525, 0.757576, 1.76768, 1.26263, 0, 0, 0.252525, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.50505, 1.26263, 0.757576, 1.51515, 0.757576,
    1.0101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.252525,
    0.252525, 0, 0.252525, 0.50505, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1.51515, 1.76768, 1.26263, 1.26263, 0.252525, 0, 0, 0, 0, 0, 0, 0.757576, 0.50505, 0,
    0, 0.757576, 0.252525, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0.252525, 0, 0.252525, 1.51515, 3.28283, 2.0202, 1.76768, 0.252525, 0, 0.50505, 1.26263, 1.0101, 0, 0, 0,
    0.757576, 0.757576, 1.51515, 0.50505, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.50505, 0.50505, 0.757576, 0.50505, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.252525, 0.252525, 0.757576,
    0, 0, 0, 0.252525, 0, 0, 0, 0, 0, 0, 0.252525, 0, 0, 0, 0, 0.252525, 0.50505, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.0101, 1.26263, 1.0101, 0.252525, 2.27273, 1.51515, 0,
    0.50505, 1.26263, 0.757576, 1.0101, 0.252525, 0.252525, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.50505, 0.252525, 0, 0, 0.50505, 0.252525, 0, 0, 1.0101,
    0.757576, 1.76768, 1.76768, 0, 0, 0, 1.76768, 2.77778, 2.52525, 2.27273, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.0101, 1.26263,
    1.0101, 0.50505, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0.50505, 0.50505, 0.252525, 0.252525, 0, 0, 0.252525, 0.50505, 0, 0.757576, 0,
    0.50505, 2.52525, 0, 0, 0, 0, 0, 0, 0, 0, 1.26263, 2.52525, 3.53535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.252525, 0.757576, 0, 0, 0.252525, 0.252525, 0, 0, 0.252525, 0.252525, 0.252525,
    0.252525, 0.50505, 0, 0, 0, 0, 0, 0, 0, 0, 1.26263, 0.757576, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

  EXPECT_EQ (descriptor.points.size (), 1);
  for (std::size_t i = 0; i < std::size_t (descriptor.points[0].descriptorSize ()); ++i)
  {
    EXPECT_NEAR (descriptor.points[0].histogram[i], ref_values[i], 1e-5);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, GASDShapeAndColorEstimationQuadrilinearInterp)
{
  // Create fake point cloud with colors
  pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudWithColors (new pcl::PointCloud<pcl::PointXYZRGBA>);
  createColorCloud (*cloudWithColors);

  pcl::GASDColorEstimation<pcl::PointXYZRGBA, pcl::GASDSignature984> gasd;
  gasd.setInputCloud (cloudWithColors);
  gasd.setColorHistsInterpMethod (pcl::INTERP_QUADRILINEAR);

  pcl::PointCloud<pcl::GASDSignature984> descriptor;
  gasd.compute (descriptor);

  const float ref_values[984] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.0101, 0, 0, 0, 0, 3.53535, 1.26263, 0, 0, 0,
    1.51515, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.757576, 0, 0, 0, 0, 0.757576, 6.06061, 0,
    0, 0, 0, 4.0404, 1.51515, 0, 0, 0, 0, 6.81818, 0, 0, 0, 0, 0, 1.76768, 1.76768, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0.50505, 0, 0, 0, 0, 0, 3.78788, 0.50505, 0, 0, 0, 0, 5.80808, 0, 0, 0, 0, 0, 2.0202,
    4.29293, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3.78788, 0.252525, 0, 0, 0, 0, 4.79798, 0, 0, 0,
    0, 0, 4.0404, 1.26263, 0, 0, 0, 0, 0.50505, 6.56566, 2.77778, 0, 0, 0, 0, 0.252525, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2.27273, 3.0303, 0, 0, 0, 0, 4.79798, 0, 0, 0, 0, 0, 4.0404, 2.0202, 0, 0, 0, 0, 0.757576,
    1.51515, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.27273, 0, 0, 0, 0, 0.50505, 5.80808, 0, 0,
    0, 0, 0, 1.26263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0.0895643, 0.143288, 0.0720839, 0.125029, 0.171768, 0.140765, 0.00434639, 0.00200408, 0, 0, 0.00597724,
    0.0525044, 0.193269, 0.429334, 0.207777, 0.24761, 0.268739, 0.237959, 0.0413768, 0.00387127, 0,
    0.000931569, 0.00385942, 0.0404541, 0.00418768, 0, 0, 0, 0, 0.00142713, 0.0162888, 0, 0, 0.000403183,
    0, 0, 0, 0.00116143, 0.00354532, 0.00280258, 0.00148538, 0, 0, 0, 0, 0, 0, 0.0123895, 0.248346, 0.591086,
    0.636957, 0.597111, 0.656083, 0.378095, 0.0377745, 0.0129163, 0, 0, 0.130372, 0.659749, 0.927181, 0.498144,
    0.27824, 0.426372, 0.616247, 1.04577, 0.643063, 0.0249504, 0, 0.0606673, 0.0426988, 0.156632, 0.0643944, 0,
    0, 0, 0, 0.0177954, 0.0985798, 0, 0, 0.0262568, 0, 0, 0, 0.00271963, 0.0233101, 0.0331309, 0.0160384, 0,
    0, 0, 0, 0, 0, 0.00471289, 0.3579, 0.851654, 0.893212, 1.1593, 0.81276, 0.451746, 0.00573487,
    0.00665285, 0.0159345, 0.0197507, 0.0100332, 0.116356, 0.258904, 0.285916, 0.137536, 0.329754, 0.201282,
    0.276937, 0.0504019, 0.00140554, 0.00331221, 0.00406454, 0.00205266, 0.00332975, 0.00195687, 0, 0, 0, 0,
    3.03073e-005, 3.03073e-005, 0, 0, 0, 0, 0, 0, 0, 0.000609614, 0.000500804, 1.35678e-005, 0, 0, 0, 0, 0, 0,
    0, 0.0354586, 0.0530443, 0.0349205, 0.0647915, 0.0347517, 0.0334093, 0.000258137, 0.00104701, 0.00399398,
    0.00607182, 0, 0, 0.0145624, 0.0392641, 0.0252404, 0.0542372, 0.0210596, 0.0219414, 0.000380611,
    0.000221201, 0.000827126, 0.00124953, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0.000831429, 0, 0.0802922, 0.12617, 0.158971, 0.151476, 0.134574, 0.0953864, 0.00562285, 0.00363446,
    0.000513492, 0.000831429, 0.000155639, 0, 0.175686, 0.321095, 0.233626, 0.252716, 0.239946, 0.190063, 0.00987096,
    0.00659357, 0.000123905, 0.000155639, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0012772, 0.00426605, 0.00325023,
    0.00231641, 0, 0, 0, 0, 0, 0.0507473, 0.0557701, 0.296648, 0.941834, 1.30216, 1.22476, 1.00798, 0.328648,
    0.0418844, 0.0566177, 0.0853042, 0.107112, 0.0125218, 0.012783, 0.350977, 0.584441, 0.549991, 0.690193,
    0.674605, 0.354125, 0.0598277, 0.0514012, 0.0274164, 0.0305356, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0.0044642, 0.045621, 0.0482378, 0.0215847, 0, 0, 0, 0, 0, 0.149713, 0.0196222, 0.265814, 1.27681,
    2.13404, 2.0593, 1.40056, 0.405919, 0.00728233, 0.233705, 0.784621, 0.679261, 0.0656672, 0.00416686,
    0.167507, 0.862586, 1.01845, 1.07935, 0.827779, 0.200586, 0.00256227, 0.0732096, 0.320982, 0.275378, 0, 0,
    0.0120096, 0.0255571, 0.0166044, 0.0161996, 0.0177707, 0.00366082, 0, 0, 0, 0, 0, 0, 0, 0, 0.0010984,
    0.00114305, 1.79879e-005, 0, 0, 0, 0, 0, 0.0228719, 0, 0.0425383, 0.218586, 0.341904, 0.287174, 0.163829,
    0.0258371, 0.000214485, 0.0198714, 0.118435, 0.11307, 0.0161313, 0, 0.0829918, 0.553307, 0.643753,
    0.581184, 0.474243, 0.0471955, 0.000316249, 0.00722545, 0.0624794, 0.0518481, 0, 0, 0.0144017, 0.0326961,
    0.0204908, 0.0169458, 0.0223265, 0.00392622, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.132962,
    0.0193449, 0.0551697, 0.105996, 0.407031, 0.35539, 0.0566072, 0.0303696, 0.0243085, 0.144756, 0.15231,
    0.130866, 0.147091, 0.0111985, 0.0176582, 0.0786188, 0.24031, 0.179748, 0.0279387, 0.00883565, 0.0148111,
    0.134383, 0.172131, 0.180972, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0.563728, 0.841891, 0.651939, 0.339866, 1.10015, 1.0062, 0.242513, 0.509944, 0.866722, 0.624862, 0.607641,
    0.594272, 0.328508, 0.189046, 0.152908, 0.161903, 0.442358, 0.383888, 0.109291, 0.132222, 0.157232,
    0.276749, 0.317518, 0.356862, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0.436075, 0.432522, 0.0892119, 0.386335, 0.986563, 0.919872, 0.313085, 0.123551, 0.543859, 0.596885,
    0.920122, 1.01302, 0.249186, 0.112221, 0.100269, 0.778301, 1.36733, 1.47131, 0.97278, 0.0962166,
    0.12436, 0.241092, 0.422573, 0.432224, 0, 0, 0.0292873, 0.069272, 0.0241701, 0.0450751, 0.0621059,
    0.0105498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0145794, 0.000546371, 0.00160651, 0.232425,
    0.411777, 0.279927, 0.0993127, 0.00288046, 5.21399e-005, 0.00594067, 0.0722626, 0.0420616, 0.0115545,
    0.000641368, 0.101776, 0.773579, 1.03488, 0.850195, 0.654514, 0.0566626, 5.28226e-005, 0.00465405,
    0.0493028, 0.0260373, 0, 0, 0.035701, 0.0890744, 0.0369472, 0.0510598, 0.0810702, 0.0114321, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.155581, 0.0081056, 0.0392221, 0.0102823, 0.0121948, 0.0159764,
    0.00453647, 0.016245, 0.0122912, 0.113791, 0.0920094, 0.14147, 0.363948, 0.0102655, 0.0146145,
    0.00654133, 0.0112985, 0.0106292, 0.00172027, 0.00635388, 0.00840588, 0.165742, 0.242665, 0.403613, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4.03759e-006, 3.95477e-006, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.718571,
    0.431673, 0.22117, 0.0441141, 0.0132298, 0.0152672, 0.0713271, 0.307082, 0.443776, 0.69396, 0.604915,
    0.694449, 1.17028, 0.209737, 0.0721899, 0.022148, 0.0104337, 0.0100016, 0.0245223, 0.12181, 0.152934,
    0.979424, 1.55304, 1.81121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000199128, 0.000195043, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0.486315, 0.325842, 0.0116061, 0.0347715, 0.180407, 0.21691, 0.0546041, 0.0337822,
    0.322526, 0.517204, 0.312276, 0.276607, 0.460413, 0.185895, 0.00199094, 0.0450161, 0.180782, 0.233176,
    0.0872505, 0.0137579, 0.177266, 0.554998, 0.503438, 0.351193, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00515609, 0.00142381, 0, 0.00913963, 0.0473098, 0.0779652, 0.0204231,
    0.000522999, 7.43773e-005, 0.000796905, 0.00721695, 0.00654336, 0.00568307, 0.00167137, 0, 0.0142623,
    0.0640188, 0.0967736, 0.0307571, 0.00109069, 7.53513e-005, 0.000807341, 0.007594, 0.00643415, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

  EXPECT_EQ (descriptor.points.size (), 1);
  for (std::size_t i = 0; i < std::size_t( descriptor.points[0].descriptorSize ()); ++i)
  {
    EXPECT_NEAR (descriptor.points[0].histogram[i], ref_values[i], 1e-5);
  }
}

/* ---[ */
int
main (int argc,
      char** argv)
{
  if (argc < 2)
  {
    std::cerr << "No test file given. Please download `bun0.pcd` and pass its path to the test." << std::endl;
    return (-1);
  }

  cloud.reset (new pcl::PointCloud<pcl::PointXYZ>);

  if (pcl::io::loadPCDFile < pcl::PointXYZ > (argv[1], *cloud) < 0)
  {
    std::cerr << "Failed to read test file. Please download `bun0.pcd` and pass its path to the test." << std::endl;
    return (-1);
  }

  testing::InitGoogleTest (&argc, argv);
  return (RUN_ALL_TESTS ());
}
/* ]--- */
