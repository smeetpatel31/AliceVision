// This file is part of the AliceVision project and is made available under
// the terms of the MPL2 license (see the COPYING.md file).

#pragma once

#include "LocalizationResult.hpp"

#include <aliceVision/image/image_container.hpp>
#include <aliceVision/feature/ImageDescriber.hpp>
#include <aliceVision/camera/PinholeRadial.hpp>
#include <aliceVision/robust_estimation/robust_estimators.hpp>
#include <aliceVision/numeric/numeric.h>

namespace aliceVision {
namespace localization {

struct LocalizerParameters
{
  LocalizerParameters() :
  _visualDebug(""),
  _refineIntrinsics(false),
  _fDistRatio(0.8),
  _featurePreset(feature::EDESCRIBER_PRESET::ULTRA_PRESET),
  _errorMax(std::numeric_limits<double>::infinity()),
  _resectionEstimator(robust::ROBUST_ESTIMATOR_ACRANSAC),
  _matchingEstimator(robust::ROBUST_ESTIMATOR_ACRANSAC),
  _useLocalizeRigNaive(false),
  _angularThreshold(D2R(0.1)) { }

  virtual ~LocalizerParameters() = 0;

  /// enable visual debugging options
  std::string _visualDebug;  
  /// whether or not the Intrinsics of the query camera has to be refined
  bool _refineIntrinsics;
  /// the distance ratio to use when matching feature with the ratio test
  float _fDistRatio;
  /// the preset to use for feature extraction of the query image
  feature::EDESCRIBER_PRESET _featurePreset;
  /// maximum reprojection error allowed for resectioning
  double _errorMax;
  /// the type of *sac framework to use for resection
  robust::EROBUST_ESTIMATOR _resectionEstimator;
  /// the type of *sac framework to use for matching
  robust::EROBUST_ESTIMATOR _matchingEstimator; 	
  /// force the use of the rig localization without openGV
  bool _useLocalizeRigNaive;
  /// in rad, it is the maximum angular error for the opengv rig resection
  double _angularThreshold;                       
};

inline LocalizerParameters::~LocalizerParameters() {}

using OccurenceKey = IndMatch3D2D;
using OccurenceMap = std::map<OccurenceKey, std::size_t>;

class ILocalizer
{
public:
    ILocalizer() : _isInit(false) { };

    // Only relevant for CCTagLocalizer
    virtual void setCudaPipe(int) { }
    
    bool isInit() const {return _isInit;}
    
    const sfm::SfM_Data& getSfMData() const {return _sfm_data; }
    
    /**
   * @brief Localize one image
   * 
   * @param[in] imageGrey The input greyscale image.
   * @param[in] param The parameters for the localization.
   * @param[in] useInputIntrinsics Uses the \p queryIntrinsics as known calibration.
   * @param[in,out] queryIntrinsics Intrinsic parameters of the camera, they are used if the
   * flag useInputIntrinsics is set to true, otherwise they are estimated from the correspondences.
   * @param[out] localizationResult The localization result containing the pose and the associations.
   * @param[in] imagePath Optional complete path to the image, used only for debugging purposes.
   * @return  true if the image has been successfully localized.
   */
  virtual bool localize(const image::Image<unsigned char> & imageGrey,
                        const LocalizerParameters *param,
                        bool useInputIntrinsics,
                        camera::PinholeRadialK3 &queryIntrinsics,
                        LocalizationResult & localizationResult,
                        const std::string& imagePath = std::string()) = 0;

  virtual bool localize(const feature::MapRegionsPerDesc &queryRegions,
                        const std::pair<std::size_t, std::size_t> &imageSize,
                        const LocalizerParameters *param,
                        bool useInputIntrinsics,
                        camera::PinholeRadialK3 &queryIntrinsics,
                        LocalizationResult & localizationResult,
                        const std::string& imagePath = std::string()) = 0;
    
  virtual bool localizeRig(const std::vector<image::Image<unsigned char> > & vec_imageGrey,
                           const LocalizerParameters *param,
                           std::vector<camera::PinholeRadialK3 > &vec_queryIntrinsics,
                           const std::vector<geometry::Pose3 > &vec_subPoses,
                           geometry::Pose3 &rigPose, 
                           std::vector<LocalizationResult>& vec_locResults)=0;
    
  virtual bool localizeRig(const std::vector<feature::MapRegionsPerDesc> & vec_queryRegions,
                           const std::vector<std::pair<std::size_t, std::size_t> > &imageSize,
                           const LocalizerParameters *param,
                           std::vector<camera::PinholeRadialK3 > &vec_queryIntrinsics,
                           const std::vector<geometry::Pose3 > &vec_subPoses,
                           geometry::Pose3 &rigPose,
                           std::vector<LocalizationResult>& vec_locResults)=0;
   
  virtual ~ILocalizer( ) {}

protected:
  bool _isInit;
  sfm::SfM_Data _sfm_data;

};

} //namespace aliceVision 
} //namespace localization 

