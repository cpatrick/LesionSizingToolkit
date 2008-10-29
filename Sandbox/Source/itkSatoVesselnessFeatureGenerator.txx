/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSatoVesselnessFeatureGenerator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSatoVesselnessFeatureGenerator_txx
#define __itkSatoVesselnessFeatureGenerator_txx

#include "itkSatoVesselnessFeatureGenerator.h"


namespace itk
{

/*
 * Constructor
 */
template <unsigned int NDimension>
SatoVesselnessFeatureGenerator<NDimension>
::SatoVesselnessFeatureGenerator()
{
  this->SetNumberOfRequiredInputs( 1 );

  this->m_HessianFilter = HessianFilterType::New();
  this->m_VesselnessFilter = VesselnessMeasureFilterType::New();

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput( 0, outputObject.GetPointer() );

  this->m_Sigma =  1.0;
  this->m_Alpha1 = 0.5;
  this->m_Alpha2 = 2.0;
}


/*
 * Destructor
 */
template <unsigned int NDimension>
SatoVesselnessFeatureGenerator<NDimension>
::~SatoVesselnessFeatureGenerator()
{
}

template <unsigned int NDimension>
void
SatoVesselnessFeatureGenerator<NDimension>
::SetInput( const SpatialObjectType * spatialObject )
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<SpatialObjectType *>( spatialObject ));
}

template <unsigned int NDimension>
const typename SatoVesselnessFeatureGenerator<NDimension>::SpatialObjectType *
SatoVesselnessFeatureGenerator<NDimension>
::GetFeature() const
{
  if (this->GetNumberOfOutputs() < 1)
    {
    return 0;
    }

  return static_cast<const SpatialObjectType*>(this->ProcessObject::GetOutput(0));

}


/*
 * PrintSelf
 */
template <unsigned int NDimension>
void
SatoVesselnessFeatureGenerator<NDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}


/*
 * Generate Data
 */
template <unsigned int NDimension>
void
SatoVesselnessFeatureGenerator<NDimension>
::GenerateData()
{
  typename InputImageSpatialObjectType::ConstPointer inputObject = 
    dynamic_cast<const InputImageSpatialObjectType * >( this->ProcessObject::GetInput(0) );

  if( !inputObject )
    {
    itkExceptionMacro("Missing input spatial object");
    }

  const InputImageType * inputImage = inputObject->GetImage();

  if( !inputImage )
    {
    itkExceptionMacro("Missing input image");
    }

  this->m_HessianFilter->SetInput( inputImage );
  this->m_VesselnessFilter->SetInput( this->m_HessianFilter->GetOutput() );

  this->m_HessianFilter->SetSigma( this->m_Sigma );
  this->m_VesselnessFilter->SetAlpha1( this->m_Alpha1 );
  this->m_VesselnessFilter->SetAlpha2( this->m_Alpha2 );

  this->m_VesselnessFilter->Update();

  typename OutputImageType::Pointer outputImage = this->m_VesselnessFilter->GetOutput();

  outputImage->DisconnectPipeline();

  typedef itk::ImageSpatialObject< Dimension, OutputPixelType > OutputImageSpatialObjectType;

  OutputImageSpatialObjectType * outputObject =
    dynamic_cast< OutputImageSpatialObjectType * >(this->ProcessObject::GetOutput(0));

  outputObject->SetImage( outputImage );
}

} // end namespace itk

#endif
