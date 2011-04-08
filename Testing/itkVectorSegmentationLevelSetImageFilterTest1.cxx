/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkVectorSegmentationLevelSetImageFilterTest1.cxx

  Copyright (c) Kitware Inc. 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkVectorSegmentationLevelSetImageFilter.h"
#include "itkVectorSegmentationLevelSetFunction.h"
#include "itkImage.h"
#include "itkVector.h"

namespace itk {

template <class TInputImage, class TFeatureImage, class TOutputImage >
class HelperVectorSegmentationLevelSetImageFilter
  : public VectorSegmentationLevelSetImageFilter<TInputImage, TFeatureImage, TOutputImage >
{
public:
  typedef HelperVectorSegmentationLevelSetImageFilter    Self;
  typedef VectorSegmentationLevelSetImageFilter<
    TInputImage, TFeatureImage, TOutputImage>            Superclass;
  typedef SmartPointer<Self>                             Pointer;
  typedef SmartPointer<const Self>                       ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(HelperVectorSegmentationLevelSetImageFilter, 
     VectorSegmentationLevelSetImageFilter);
};

template <class TInputImage, class TFeatureImage>
class HelperVectorSegmentationLevelSetFunction
  : public VectorSegmentationLevelSetFunction<TInputImage, TFeatureImage>
{
public:
  typedef HelperVectorSegmentationLevelSetFunction    Self;
  typedef VectorSegmentationLevelSetFunction<
    TInputImage, TFeatureImage >                      Superclass;
  typedef SmartPointer<Self>                          Pointer;
  typedef SmartPointer<const Self>                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(HelperVectorSegmentationLevelSetFunction, 
     VectorSegmentationLevelSetFunction);

  typedef typename Superclass::PixelType              PixelType;
  typedef typename Superclass::NeighborhoodType       NeighborhoodType;
  typedef typename Superclass::FloatOffsetType        FloatOffsetType;
  typedef typename Superclass::ScalarValueType        ScalarValueType;
  typedef typename Superclass::VectorType             VectorType;
  typedef typename Superclass::GlobalDataStruct       GlobalDataStruct;

  virtual PixelType  ComputeUpdate(
    const NeighborhoodType &neighborhood, void *globalData,
    const FloatOffsetType &offset = FloatOffsetType(0.0))
      {
      PixelType output;
      const unsigned int component = 0;
      GlobalDataStruct * data = static_cast<GlobalDataStruct *>( globalData );
      ScalarValueType propagationSpeed = this->PropagationSpeed( neighborhood, offset, component, data );
      ScalarValueType curvatureSpeed   = this->CurvatureSpeed(   neighborhood, offset, component, data );
      VectorType      advectionField   = this->AdvectionField(   neighborhood, offset, component, data );
      std::cout << propagationSpeed << " " << curvatureSpeed << std::endl;
      return output;
      }
};


} // end namespace itk


int main( int argc, char * argv [] )
{

  const unsigned int Dimension = 2;
  const unsigned int NumberOfPhases = 2;
  const unsigned int NumberOfComponents = 1;

  typedef itk::Vector< float, NumberOfPhases >           LevelSetPixelType;
  typedef itk::Vector< float, NumberOfComponents >           FeaturePixelType;

  typedef itk::Image< LevelSetPixelType, Dimension >     LevelSetImageType;
  typedef itk::Image< FeaturePixelType, Dimension >      FeatureImageType;

  typedef itk::HelperVectorSegmentationLevelSetImageFilter< 
    LevelSetImageType, FeatureImageType, LevelSetImageType >      FilterType;

  typedef itk::HelperVectorSegmentationLevelSetFunction<
    LevelSetImageType, FeatureImageType >                         FunctionType;

  FilterType::Pointer filter = FilterType::New();

  FunctionType::Pointer differenceFunction = FunctionType::New();

  FunctionType::MatrixValueType curvatureWeights( NumberOfPhases, NumberOfPhases );
  FunctionType::MatrixValueType propagationWeights( NumberOfPhases, NumberOfComponents );
  FunctionType::MatrixValueType advectionWeights( NumberOfPhases, NumberOfComponents );
  FunctionType::MatrixValueType laplacianSmoothingWeights( NumberOfPhases, NumberOfComponents );
  curvatureWeights.SetIdentity();
  propagationWeights.Fill(10.0);
  advectionWeights.Fill(10.0);
  laplacianSmoothingWeights.Fill(10.0);
  differenceFunction->SetCurvatureWeights( curvatureWeights );
  differenceFunction->SetPropagationWeights( propagationWeights );
  differenceFunction->SetAdvectionWeights( advectionWeights );
  differenceFunction->SetLaplacianSmoothingWeights( laplacianSmoothingWeights );

  std::cout << filter->GetNameOfClass() << std::endl;

  LevelSetImageType::Pointer inputLevelSet = LevelSetImageType::New();

  LevelSetImageType::RegionType region;
  LevelSetImageType::SizeType   size;
  LevelSetImageType::IndexType  start;

  start.Fill( 0 );
  size.Fill( 10 );

  region.SetSize( size );
  region.SetIndex( start );

  inputLevelSet->SetRegions( region );
  inputLevelSet->Allocate();

  filter->SetInput( inputLevelSet );
  filter->SetNumberOfIterations( 5 );
  filter->SetSegmentationFunction( differenceFunction.GetPointer() );

  // Exercise the Print method 
  filter->Print( std::cout );

  std::cout << "Name of Class = " << filter->GetNameOfClass() << std::endl;
  std::cout << "Name of Superclass = " << filter->Superclass::GetNameOfClass() << std::endl;

  FeatureImageType::Pointer featureImage = FeatureImageType::New();

  featureImage->SetRegions( region );
  featureImage->Allocate();

  filter->SetFeatureImage( featureImage );

  const FeatureImageType * featureImageBack = filter->GetFeatureImage();

  if( featureImageBack != featureImage.GetPointer() )
    {
    std::cerr << "Error in Set/GetFeatureImage() " << std::endl;
    return EXIT_FAILURE;
    }

  if( featureImageBack != differenceFunction->GetFeatureImage() )
    {
    std::cerr << "Error in Set/GetFeatureImage() in finite difference function" << std::endl;
    return EXIT_FAILURE;
    }


  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Number of elapsed iterations = " << filter->GetElapsedIterations() << std::endl;

  filter->GenerateAdvectionImage();

  typedef FunctionType::ImageType      SpeedImageType;
  SpeedImageType::Pointer speedImage = SpeedImageType::New();

  speedImage->SetRegions( region );
  speedImage->Allocate();

  differenceFunction->SetSpeedImage( speedImage );

  const SpeedImageType * speedImageBack = differenceFunction->GetSpeedImage();

  if( speedImageBack != speedImage.GetPointer() )
    {
    std::cerr << "Error in function Set/GetSpeedImage() " << std::endl;
    return EXIT_FAILURE;
    }

  speedImageBack = filter->GetSpeedImage();

  if( speedImageBack != speedImage.GetPointer() )
    {
    std::cerr << "Error in ImageFilter Set/GetSpeedImage() " << std::endl;
    return EXIT_FAILURE;
    }

  typedef FunctionType::VectorImageType      AdvectionImageType;
  AdvectionImageType::Pointer advectionImage = AdvectionImageType::New();

  advectionImage->SetRegions( region );
  advectionImage->Allocate();

  unsigned int component = 0;
  differenceFunction->SetAdvectionImage( component, advectionImage );

  const AdvectionImageType * advectionImageBack = 
    differenceFunction->GetAdvectionImage( component );

  if( advectionImageBack != advectionImage.GetPointer() )
    {
    std::cerr << "Error in function Set/GetAdvectionImage() " << std::endl;
    return EXIT_FAILURE;
    }

  advectionImageBack = filter->GetAdvectionImage( component );

  if( advectionImageBack != advectionImage.GetPointer() )
    {
    std::cerr << "Error in function Set/GetAdvectionImage() " << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Exercise 'SetInitialImage()'
  //
  filter->SetInitialImage( inputLevelSet );

  //
  // Exercise 'Generate' methods
  //
  filter->GenerateSpeedImage();
  filter->GenerateAdvectionImage();

  //
  // Exercise the Calculate Methods
  //
  differenceFunction->CalculateSpeedImage();
  differenceFunction->CalculateAdvectionImage();


  //
  //  Exercise ReverseExpansionDirection methods
  //
  filter->SetReverseExpansionDirection( true );
  if( !filter->GetReverseExpansionDirection() )
    {
    std::cerr << "Error in function Set/GetReverseExpansionDirection() " << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetReverseExpansionDirection( false );
  if( filter->GetReverseExpansionDirection() )
    {
    std::cerr << "Error in function Set/GetReverseExpansionDirection() " << std::endl;
    return EXIT_FAILURE;
    }

  filter->ReverseExpansionDirectionOn();
  if( !filter->GetReverseExpansionDirection() )
    {
    std::cerr << "Error in function Set/GetReverseExpansionDirection() " << std::endl;
    return EXIT_FAILURE;
    }

  filter->ReverseExpansionDirectionOff();
  if( filter->GetReverseExpansionDirection() )
    {
    std::cerr << "Error in function Set/GetReverseExpansionDirection() " << std::endl;
    return EXIT_FAILURE;
    }

  //
  //  Exercise AutoGenerateSpeedAdvection methods
  //
  filter->SetAutoGenerateSpeedAdvection( true );
  if( !filter->GetAutoGenerateSpeedAdvection() )
    {
    std::cerr << "Error in function Set/GetAutoGenerateSpeedAdvection() " << std::endl;
    return EXIT_FAILURE;
    }

  filter->SetAutoGenerateSpeedAdvection( false );
  if( filter->GetAutoGenerateSpeedAdvection() )
    {
    std::cerr << "Error in function Set/GetAutoGenerateSpeedAdvection() " << std::endl;
    return EXIT_FAILURE;
    }

  filter->AutoGenerateSpeedAdvectionOn();
  if( !filter->GetAutoGenerateSpeedAdvection() )
    {
    std::cerr << "Error in function Set/GetAutoGenerateSpeedAdvection() " << std::endl;
    return EXIT_FAILURE;
    }

  filter->AutoGenerateSpeedAdvectionOff();
  if( filter->GetAutoGenerateSpeedAdvection() )
    {
    std::cerr << "Error in function Set/GetAutoGenerateSpeedAdvection() " << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Set the flags to ON and run the filter
  //
  filter->ReverseExpansionDirectionOn();
  filter->AutoGenerateSpeedAdvectionOn();

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // Force the filter to run again
  inputLevelSet->Modified();

  try
    {
    filter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}