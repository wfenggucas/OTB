/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbHistogramOfOrientedGradientCovariantImageFunction_txx
#define __otbHistogramOfOrientedGradientCovariantImageFunction_txx

#include "otbHistogramOfOrientedGradientCovariantImageFunction.h"
#include "itkNumericTraits.h"
#include "itkMacro.h"

namespace otb
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputPrecision, class TCoordRep>
HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision, TCoordRep>
::HistogramOfOrientedGradientCovariantImageFunction() : m_NeighborhoodRadius(1),
       m_NumberOfOrientationBins(18)
{}

template <class TInputImage, class TOutputPrecision, class TCoordRep>
void
HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision, TCoordRep>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << " Neighborhood radius value   : "  << m_NeighborhoodRadius << std::endl;
}

template <class TInputImage, class TOutputPrecision, class TCoordRep>
typename HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision,TCoordRep>::OutputType
HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision,TCoordRep>
::EvaluateAtIndex(const IndexType& index) const
{
  // Build output vector
  OutputType hog;
  
  // Check for input image
  if( !this->GetInputImage() )
    {
    return hog;
    }
  
  // Check for out of buffer
  if ( !this->IsInsideBuffer( index ) )
    {
    return hog;
    }

  // Create an N-d neighborhood kernel, using a zeroflux boundary condition
  typename InputImageType::SizeType kernelSize;
  kernelSize.Fill( m_NeighborhoodRadius );
  
  itk::ConstNeighborhoodIterator<InputImageType>
    it(kernelSize, this->GetInputImage(), this->GetInputImage()->GetBufferedRegion());
  
  // Set the iterator at the desired location
  it.SetLocation(index);
  
  // Offset to be used in the loops
  typename InputImageType::OffsetType offset;

  // Compute the center bin radius
  unsigned int centerBinRadius = vcl_floor(vcl_log((double)m_NeighborhoodRadius)/vcl_log(2.0));

  // Define a gaussian kernel around the center location
  double squaredRadius = m_NeighborhoodRadius * m_NeighborhoodRadius;
  double squaredCenterBinRadius = centerBinRadius * centerBinRadius;
  double squaredSigma = 0.25 * squaredRadius;

  // Build a global orientation histogram
  std::vector<TOutputPrecision> globalOrientationHistogram(m_NumberOfOrientationBins,0.);

  // Compute the orientation bin width
  double orientationBinWidth = 2 * M_PI / m_NumberOfOrientationBins;

  // Build the global orientation histogram
  for(int i = -(int)m_NeighborhoodRadius;i< (int)m_NeighborhoodRadius;++i)
    {
    for(int j = -(int)m_NeighborhoodRadius;j< (int)m_NeighborhoodRadius;++j)
      {
      // Check if the current pixel lies within a disc of radius m_NeighborhoodRadius
      double currentSquaredRadius = i*i+j*j;
      if(currentSquaredRadius < squaredRadius)
 {
 // If so, compute the gaussian weighting (this could be
 // computed once for all for the sake of optimisation)
 double gWeight = (1/vcl_sqrt(2*M_PI*squaredSigma)) * vcl_exp(- currentSquaredRadius/(2*squaredSigma));

 // Compute pixel location
 offset[0]=i;
 offset[1]=j;
 
 // Get the current gradient covariant value
 InputPixelType gradient = it.GetPixel(offset);

 // Then, compute the gradient orientation
 double angle = vcl_atan2(gradient[1],gradient[0]);

 // Also compute its magnitude
 TOutputPrecision magnitude = vcl_sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]);

 // Determine the bin index (shift of M_PI since atan2 values
 // lies in [-pi,pi]
 unsigned int binIndex = vcl_floor((M_PI + angle)/orientationBinWidth);
 
 // Cumulate values
 globalOrientationHistogram[binIndex]+= magnitude * gWeight;
 }
      }
    }

  // Compute principal orientation
  // TODO: Replace this with a stl algorithm
  double maxOrientationHistogramValue = globalOrientationHistogram[0];
  unsigned int maxOrientationHistogramBin = 0;

  for(unsigned int i = 1; i < m_NumberOfOrientationBins;++i)
    {
    // Retrieve current value
    double currentValue = globalOrientationHistogram[i];

    // Look for new maximum
    if(maxOrientationHistogramValue<currentValue)
      {
      maxOrientationHistogramValue = currentValue;
      maxOrientationHistogramBin = i;
      }
    }

  // Derive principal orientation
  double principalOrientation = maxOrientationHistogramBin * orientationBinWidth;

  // Initialize the five spatial bins
  std::vector<TOutputPrecision> centerHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> upperLeftHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> upperRightHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> lowerLeftHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> lowerRightHistogram(m_NumberOfOrientationBins,0.);

  // Walk the image once more to fill the spatial bins
  for(int i = -(int)m_NeighborhoodRadius;i< (int)m_NeighborhoodRadius;++i)
    {
    for(int j = -(int)m_NeighborhoodRadius;j< (int)m_NeighborhoodRadius;++j)
      {
      // Check if the current pixel lies within a disc of radius m_NeighborhoodRadius
      double currentSquaredRadius = i*i+j*j;
      if(currentSquaredRadius < squaredRadius)
 {
 // If so, compute the gaussian weighting (this could be
 // computed once for all for the sake of optimisation)
 double gWeight = (1/vcl_sqrt(2*M_PI*squaredSigma)) * vcl_exp(- currentSquaredRadius/(2*squaredSigma));

 // Compute pixel location
 offset[0]=i;
 offset[1]=j;
 
 // Get the current gradient covariant value
 InputPixelType gradient = it.GetPixel(offset);

 // Then, compute the compensated gradient orientation
 double angle = vcl_atan2(gradient[1],gradient[0]) - principalOrientation;

 // Angle is supposed to lie with [-pi,pi], so we ensure that
 // compenstation did not introduce out-of-range values
 if(angle > M_PI)
   {
   angle -= 2*M_PI;
   }
 else if(angle < -M_PI)
   {
   angle += 2*M_PI;
   }

 // Also compute its magnitude
 TOutputPrecision magnitude = vcl_sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]);

 // Determine the bin index (shift of M_PI since atan2 values
 // lies in [-pi,pi]
 unsigned int binIndex = vcl_floor((M_PI + angle)/orientationBinWidth);

 // Compute the angular position
 double angularPosition = vcl_atan2((double)j,(double)i) - principalOrientation;

 // Angle is supposed to lie within [-pi,pi], so we ensure that
 // the compensation did not introduce out-of-range values
 if(angularPosition > M_PI)
   {
   angularPosition -= 2*M_PI;
   }
 else if(angularPosition < -M_PI)
   {
   angularPosition += 2*M_PI;
   }

 // Check if we lie in center bin
 if(currentSquaredRadius < squaredCenterBinRadius)
   {
   centerHistogram[binIndex]+= magnitude * gWeight;
   }
 else if(angularPosition > 0)
   {
   if(angularPosition < M_PI/2)
     {
     upperRightHistogram[binIndex]+= magnitude * gWeight;
     }
   else
     {
     upperLeftHistogram[binIndex]+= magnitude * gWeight;
     }
   }
 else
   {
   if(angularPosition > -M_PI/2)
     {
     lowerRightHistogram[binIndex]+= magnitude * gWeight;
     }
   else
     {
     lowerLeftHistogram[binIndex]+= magnitude * gWeight;
     }
   }
 
 // Cumulate values
 globalOrientationHistogram[binIndex]+= magnitude * gWeight;
 }
      }
    }

  // Build the final output
  hog.push_back(centerHistogram);
  hog.push_back(upperLeftHistogram);
  hog.push_back(upperRightHistogram);
  hog.push_back(lowerRightHistogram);
  hog.push_back(lowerLeftHistogram);

  // Normalize each histogram
  for(typename OutputType::iterator oIt = hog.begin(); oIt!=hog.end();++oIt)
    {
    // Compute L2 norm
    double squaredCumul = 1e-10;
    for(typename std::vector<TOutputPrecision>::const_iterator vIt = oIt->begin();
 vIt!=oIt->end(); ++vIt)
      {
      squaredCumul += (*vIt)*(*vIt);
      }
    double scale = 1/vcl_sqrt(squaredCumul);
    // Apply normalisation factor
    for(typename std::vector<TOutputPrecision>::iterator vIt = oIt->begin();
 vIt!=oIt->end(); ++vIt)
      {
      (*vIt)*=scale;
      }
    }


  // Return result
  return hog;
}

} // namespace otb

#endif
