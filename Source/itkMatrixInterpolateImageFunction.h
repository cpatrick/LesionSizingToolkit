/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMatrixInterpolateImageFunction.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMatrixInterpolateImageFunction_h
#define __itkMatrixInterpolateImageFunction_h

#include "itkImageFunction.h"
#include "itkFixedArray.h"
#include "itkNumericTraitsMatrixPixel.h"

namespace itk
{

/**
 * Due to a bug in MSVC, an enum value cannot be accessed out of a template
 * parameter until the template class opens.  In order for templated classes
 * to access the dimension of a template parameter in defining their
 * own dimension, this class is needed as a work-around.
 */
template <typename T>
struct GetDimension
{
  itkStaticConstMacro(Dimension, int, T::Dimension);
};


/** \class MatrixInterpolateImageFunction
 * \brief Base class for all vector image interpolaters.
 *
 * MatrixInterpolateImageFunction is the base for all ImageFunctions that
 * interpolates image with vector pixel types. This function outputs
 * a return value of type Matrix<double,RowDimensions,ColumnDimensions>.
 *
 * This class is templated input image type and the coordinate
 * representation type.
 *
 * \warning This hierarchy of functions work only for images
 * with Vector-based pixel types. For scalar images use
 * InterpolateImageFunction.
 *
 * \sa InterpolateImageFunction
 * \ingroup ImageFunctions ImageInterpolators
 */
template <class TInputImage, class TCoordRep = double>
class ITK_EXPORT MatrixInterpolateImageFunction :
  public ImageFunction<
    TInputImage,
    ITK_TYPENAME NumericTraits<typename TInputImage::PixelType>::RealType,
    TCoordRep >
{
public:
  /** Extract the vector dimension from the pixel template parameter. */
  itkStaticConstMacro(RowDimensions, unsigned int,
                      TInputImage::PixelType::RowDimensions);
  itkStaticConstMacro(ColumnDimensions, unsigned int,
                      TInputImage::PixelType::ColumnDimensions);

  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Standard class typedefs. */
  typedef MatrixInterpolateImageFunction                          Self;
  typedef ImageFunction<TInputImage,
    ITK_TYPENAME NumericTraits<
      typename TInputImage::PixelType>::RealType, TCoordRep >     Superclass;
  typedef SmartPointer<Self>                                      Pointer;
  typedef SmartPointer<const Self>                                ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(MatrixInterpolateImageFunction, ImageFunction);

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType               InputImageType;
  typedef typename InputImageType::PixelType                PixelType;
  typedef typename PixelType::ValueType                     ValueType;
  typedef typename NumericTraits<ValueType>::RealType       RealType;

  /** Point typedef support. */
  typedef typename Superclass::PointType PointType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** Output type is RealType of TInputImage::PixelType. */
  typedef typename Superclass::OutputType OutputType;

  /** CoordRep typedef support. */
  typedef TCoordRep CoordRepType;

  /** Returns the interpolated image intensity at a
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType Evaluate( const PointType& point ) const
    {
    ContinuousIndexType index;
    this->GetInputImage()->TransformPhysicalPointToContinuousIndex( point, index );
    return ( this->EvaluateAtContinuousIndex( index ) );
    }

  /** Interpolate the image at a continuous index position
   *
   * Returns the interpolated image intensity at a
   * specified index position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * Subclasses must override this method.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType EvaluateAtContinuousIndex(
    const ContinuousIndexType & index ) const = 0;

  /** Interpolate the image at an index position.
   * Simply returns the image value at the
   * specified index position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType EvaluateAtIndex( const IndexType & index ) const
    {
    OutputType output;
    PixelType input = this->GetInputImage()->GetPixel( index );
    for( unsigned int r = 0; r < RowDimensions; r++ )
      {
      for( unsigned int c = 0; c < ColumnDimensions; c++ )
        {
        output(r,c) = static_cast<double>( input(r,c) );
        }
      }
    return ( output );
    }

protected:
  MatrixInterpolateImageFunction() {}
  ~MatrixInterpolateImageFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const
    { Superclass::PrintSelf( os, indent ); }

private:
  MatrixInterpolateImageFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#endif