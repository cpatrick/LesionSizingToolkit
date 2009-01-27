/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkIsotropicResampler.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkIsotropicResampler_txx
#define __itkIsotropicResampler_txx

#include "itkIsotropicResampler.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkProgressAccumulator.h"

namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
IsotropicResampler<NDimension>
::IsotropicResampler()
{
  this->SetNumberOfRequiredInputs( 1 );
  this->SetNumberOfRequiredOutputs( 1 );

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput( 0, outputObject.GetPointer() );
}


/*
 * Destructor
 */
template <unsigned int NDimension>
IsotropicResampler<NDimension>
::~IsotropicResampler()
{
}

template <unsigned int NDimension>
void
IsotropicResampler<NDimension>
::SetInput( const SpatialObjectType * spatialObject )
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<SpatialObjectType *>( spatialObject ));
}

template <unsigned int NDimension>
const typename IsotropicResampler<NDimension>::SpatialObjectType *
IsotropicResampler<NDimension>
::GetOutput() const
{
  return static_cast<const SpatialObjectType*>(this->ProcessObject::GetOutput(0));
}


/*
 * PrintSelf
 */
template <unsigned int NDimension>
void
IsotropicResampler<NDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}


/*
 * Generate Data
 */
template <unsigned int NDimension>
void
IsotropicResampler<NDimension>
::GenerateData()
{
  // Report progress.
  typename ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  typename InputImageSpatialObjectType::ConstPointer inputObject =
    dynamic_cast<const InputImageSpatialObjectType * >( this->ProcessObject::GetInput(0) );

  if( !inputObject )
    {
    itkExceptionMacro("Missing input spatial object or incorrect type");
    }

  const InputImageType * inputImage = inputObject->GetImage();

  if( !inputImage )
    {
    itkExceptionMacro("Missing input image");
    }


  typedef itk::ResampleImageFilter< InputImageType, InputImageType >  ResampleFilterType;

  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  typedef itk::IdentityTransform< double, Dimension >  TransformType;

  typename TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();

  typedef itk::BSplineInterpolateImageFunction< InputImageType, double >  BSplineInterpolatorType;

  typename BSplineInterpolatorType::Pointer bsplineInterpolator = BSplineInterpolatorType::New();

  bsplineInterpolator->UseImageDirectionOn();
  bsplineInterpolator->SetSplineOrder( 3 );

  resampler->SetTransform( transform );
  resampler->SetInterpolator( bsplineInterpolator );
  resampler->SetDefaultPixelValue( -1024 ); // Hounsfield Units for Air

  const typename InputImageType::SpacingType & inputSpacing = inputImage->GetSpacing();

  typename InputImageType::SpacingType outputSpacing;

  const double finalSpacing = 0.2;  // 0.2 mm

  outputSpacing[0] = finalSpacing;
  outputSpacing[1] = finalSpacing;
  outputSpacing[2] = finalSpacing;

  resampler->SetOutputSpacing( outputSpacing );

  resampler->SetOutputOrigin( inputImage->GetOrigin() );
  resampler->SetOutputDirection( inputImage->GetDirection() );

  typedef typename InputImageType::SizeType   SizeType;

  SizeType inputSize = inputImage->GetLargestPossibleRegion().GetSize();
  
  typedef typename SizeType::SizeValueType SizeValueType;

  const double dx = inputSize[0] * inputSpacing[0] / outputSpacing[0];
  const double dy = inputSize[1] * inputSpacing[1] / outputSpacing[1];
  const double dz = inputSize[2] * inputSpacing[2] / outputSpacing[2];

  typename InputImageType::SizeType   finalSize;

  finalSize[0] = static_cast<SizeValueType>( dx );
  finalSize[1] = static_cast<SizeValueType>( dy );
  finalSize[2] = static_cast<SizeValueType>( dz );

  resampler->SetSize( finalSize );

  resampler->SetInput( inputImage );

  progress->RegisterInternalFilter( resampler, 1.0 );  

  resampler->Update();

  typename OutputImageType::Pointer outputImage = resampler->GetOutput();

  outputImage->DisconnectPipeline();

  typedef ImageSpatialObject< Dimension, OutputPixelType > OutputImageSpatialObjectType;

  OutputImageSpatialObjectType * outputObject =
    dynamic_cast< OutputImageSpatialObjectType * >(this->ProcessObject::GetOutput(0));

  outputObject->SetImage( outputImage );
}

} // end namespace itk

#endif
