/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkGrayscaleImageSegmentationVolumeEstimator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleImageSegmentationVolumeEstimator_h
#define __itkGrayscaleImageSegmentationVolumeEstimator_h

#include "itkSegmentationVolumeEstimator.h"

namespace itk
{

/** \class GrayscaleImageSegmentationVolumeEstimator
 * \brief Class for estimating the volume of a segmentation stored in a SpatialObject
 *  that carries a gray-scale image of pixel type float.
 *
 * \ingroup SpatialObjectFilters
 */
template <unsigned int NDimension>
class ITK_EXPORT GrayscaleImageSegmentationVolumeEstimator :
 public SegmentationVolumeEstimator<NDimension>
{
public:
  /** Standard class typedefs. */
  typedef GrayscaleImageSegmentationVolumeEstimator   Self;
  typedef SegmentationVolumeEstimator<NDimension>     Superclass;
  typedef SmartPointer<Self>                          Pointer;
  typedef SmartPointer<const Self>                    ConstPointer;

  /** Method for constructing new instances of this class. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro(GrayscaleImageSegmentationVolumeEstimator, SegmentationVolumeEstimator);

  /** Dimension of the space */
  itkStaticConstMacro(Dimension, unsigned int, NDimension);

  /** Type of spatialObject that will be passed as input and output of this
   * segmentation method. */
  typedef typename Superclass::SpatialObjectType          SpatialObjectType;
  typedef typename Superclass::SpatialObjectPointer       SpatialObjectPointer;
  typedef typename Superclass::SpatialObjectConstPointer  SpatialObjectConstPointer;

protected:
  GrayscaleImageSegmentationVolumeEstimator();
  virtual ~GrayscaleImageSegmentationVolumeEstimator();

  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of
   * the segmentation. */
  void  GenerateData();

  /** Required type of the input */
  typedef float                                               InputPixelType;
  typedef ImageSpatialObject< NDimension, InputPixelType >    InputImageSpatialObjectType;
  typedef Image< InputPixelType, NDimension >                 InputImageType;

private:
  GrayscaleImageSegmentationVolumeEstimator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
# include "itkGrayscaleImageSegmentationVolumeEstimator.txx"
#endif

#endif
