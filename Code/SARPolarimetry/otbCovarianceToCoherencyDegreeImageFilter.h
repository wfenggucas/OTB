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

#ifndef __CovarianceToCoherencyDegreeImageFilter_h
#define __CovarianceToCoherencyDegreeImageFilter_h

#include "otbUnaryFunctorImageFilter.h"

namespace otb
 {

namespace Functor {

/** \class otbCovarianceToCoherencyDegreeFunctor
 * \brief Evaluate the Coherency Degree coefficient from from the MLC image
 *
 *   Output value are:
 *   channel #0 : \f$ abs(S_{hh}*S_{vv}}^{*}) / sqrt(S_{hh}*S_{hh}}^{*}) / sqrt(S_{vv}*S_{vv}}^{*}) \f$
 *   channel #1 : \f$ abs(S_{hv}*S_{vv}}^{*}) / sqrt(S_{hv}*S_{hv}}^{*}) / sqrt(S_{vv}*S_{vv}}^{*}) \f$
 *   channel #2 : \f$ abs(S_{hh}*S_{hv}}^{*}) / sqrt(S_{hh}*S_{hh}}^{*}) / sqrt(S_{hv}*S_{hv}}^{*}) \f$
 *
 *
 * \infgroup Functor
 * \ingroup SARPolarimetry
 *
 * \sa MLCToCircularCoherencyDegreeImageFilter
 * \sa CovarianceToCoherencyImageFilter
 */
template< class TInput, class TOutput>
class CovarianceToCoherencyDegreeFunctor
{
public:
  typedef double                                   RealType;
  typedef typename std::complex <double>           ComplexType;
  typedef typename TOutput::ValueType              OutputValueType;

  inline TOutput operator()( const TInput & Covariance ) const
    {
    TOutput result;
    result.SetSize(m_NumberOfComponentsPerPixel);
    result.Fill(0.0);

    /* Using the convention  
     * \f$ C_{11} = S_{hh}*S_{hh}^* \f$
     * \f$ C_{12} = S_{hh}*S_{hv}^* \f$
     * \f$ C_{13} = S_{hh}*S_{vv}^* \f$
     * \f$ C_{22} = S_{hv}*S_{hv}^* \f$
     * \f$ C_{23} = S_{hv}*S_{vv}^* \f$
     * \f$ C_{33} = S_{vv}*S_{vv}^* \f$
     */
    const RealType    C11 =  static_cast<RealType>(Covariance[0].real());
    const ComplexType C12 =  static_cast<ComplexType>(Covariance[1]);
    const ComplexType C13 =  static_cast<ComplexType>(Covariance[2]);
    const RealType    C22 =  static_cast<RealType>(Covariance[3].real());
    const ComplexType C23 =  static_cast<ComplexType>(Covariance[4]);
    const RealType    C33 =  static_cast<RealType>(Covariance[5].real());

    if ((C11 >m_Epsilon) && (C33 > m_Epsilon))
      {
      result[0] = vcl_abs(C13) / vcl_sqrt(C11 * C33); // |<hh.vv*|/sqrt(<hh.hh*><vv.vv*>)
      }

    if ((C22 > m_Epsilon) && (C33 > m_Epsilon))
      {
      result[1] = vcl_abs(C23) / vcl_sqrt(C22 * C33);  // |<hv.vv*|/sqrt(<hv.hv*><vv.vv*>)
      }

    if ((C11 > m_Epsilon) && (C22 > m_Epsilon) )
      {
      result[2] = vcl_abs(C12) / vcl_sqrt(C11 * C22);  // |<hh.hv*|/sqrt(<hh.hh*><hv.hv*>)
      }

    return result;
    }

   unsigned int GetOutputSize()
   {
     return m_NumberOfComponentsPerPixel;
   }

   /** Constructor */
   CovarianceToCoherencyDegreeFunctor() : m_NumberOfComponentsPerPixel(3), m_Epsilon(1e-6) {}

   /** Destructor */
   virtual ~CovarianceToCoherencyDegreeFunctor() {}

private:
    unsigned int m_NumberOfComponentsPerPixel;
    const double m_Epsilon;
};
}


/** \class otbCovarianceToCoherencyDegreeImageFilter
 * \brief Compute the Coherency Degree coefficient
 * from the MLC image (6 complex channels)
 */
template <class TInputImage, class TOutputImage, class TFunction = Functor::CovarianceToCoherencyDegreeFunctor<
    ITK_TYPENAME TInputImage::PixelType, ITK_TYPENAME TOutputImage::PixelType> >
class ITK_EXPORT CovarianceToCoherencyDegreeImageFilter :
   public UnaryFunctorImageFilter<TInputImage, TOutputImage, TFunction>
{
public:
   /** Standard class typedefs. */
   typedef CovarianceToCoherencyDegreeImageFilter  Self;
   typedef UnaryFunctorImageFilter<TInputImage, TOutputImage, TFunction> Superclass;
   typedef itk::SmartPointer<Self>        Pointer;
   typedef itk::SmartPointer<const Self>  ConstPointer;

   /** Method for creation through the object factory. */
   itkNewMacro(Self);

   /** Runtime information support. */
   itkTypeMacro(CovarianceToCoherencyDegreeImageFilter, UnaryFunctorImageFilter);


protected:
  CovarianceToCoherencyDegreeImageFilter() {}
  virtual ~CovarianceToCoherencyDegreeImageFilter() {}

private:
  CovarianceToCoherencyDegreeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&);                  //purposely not implemented

 
 

};
  
} // end namespace otb

#endif
