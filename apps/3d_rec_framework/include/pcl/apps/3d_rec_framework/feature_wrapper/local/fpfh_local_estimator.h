/*
 * shot_local_estimator.h
 *
 *  Created on: Mar 24, 2012
 *      Author: aitor
 */

#pragma once

#include <pcl/apps/3d_rec_framework/feature_wrapper/local/local_estimator.h>
#include <pcl/apps/3d_rec_framework/feature_wrapper/normal_estimator.h>
#include <pcl/features/fpfh.h>

namespace pcl {
namespace rec_3d_framework {

template <typename PointInT, typename FeatureT>
class FPFHLocalEstimation : public LocalEstimator<PointInT, FeatureT> {

  using PointInTPtr = typename pcl::PointCloud<PointInT>::Ptr;
  using FeatureTPtr = typename pcl::PointCloud<FeatureT>::Ptr;
  using KeypointCloud = pcl::PointCloud<pcl::PointXYZ>;

  using LocalEstimator<PointInT, FeatureT>::support_radius_;
  using LocalEstimator<PointInT, FeatureT>::normal_estimator_;
  using LocalEstimator<PointInT, FeatureT>::keypoint_extractor_;

public:
  bool
  estimate(PointInTPtr& in,
           PointInTPtr& processed,
           PointInTPtr& keypoints,
           FeatureTPtr& signatures) override
  {

    if (!normal_estimator_) {
      PCL_ERROR("FPFHLocalEstimation :: This feature needs normals... "
                "please provide a normal estimator\n");
      return false;
    }

    if (keypoint_extractor_.empty()) {
      PCL_ERROR("FPFHLocalEstimation :: This feature needs a keypoint extractor... "
                "please provide one\n");
      return false;
    }

    // compute normals
    pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
    normal_estimator_->estimate(in, processed, normals);

    this->computeKeypoints(processed, keypoints, normals);
    std::cout << " " << normals->size() << " " << processed->size() << std::endl;

    if (keypoints->empty()) {
      PCL_WARN("FPFHLocalEstimation :: No keypoints were found\n");
      return false;
    }

    assert(processed->size() == normals->size());

    // compute signatures
    using FPFHEstimator =
        pcl::FPFHEstimation<PointInT, pcl::Normal, pcl::FPFHSignature33>;
    typename pcl::search::KdTree<PointInT>::Ptr tree(new pcl::search::KdTree<PointInT>);

    pcl::PointCloud<pcl::FPFHSignature33>::Ptr fpfhs(
        new pcl::PointCloud<pcl::FPFHSignature33>);
    FPFHEstimator fpfh_estimate;
    fpfh_estimate.setSearchMethod(tree);
    fpfh_estimate.setInputCloud(keypoints);
    fpfh_estimate.setSearchSurface(processed);
    fpfh_estimate.setInputNormals(normals);
    fpfh_estimate.setRadiusSearch(support_radius_);
    fpfh_estimate.compute(*fpfhs);

    signatures->resize(fpfhs->size());
    signatures->width = static_cast<int>(fpfhs->size());
    signatures->height = 1;

    int size_feat = 33;
    for (std::size_t k = 0; k < fpfhs->size(); k++)
      for (int i = 0; i < size_feat; i++)
        signatures->points[k].histogram[i] = fpfhs->points[k].histogram[i];

    return true;
  }
};

} // namespace rec_3d_framework
} // namespace pcl
