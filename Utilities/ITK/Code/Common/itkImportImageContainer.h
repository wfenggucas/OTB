/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImportImageContainer.h,v $
  Language:  C++
  Date:      $Date: 2009-04-25 12:24:09 $
  Version:   $Revision: 1.24 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImportImageContainer_h
#define __itkImportImageContainer_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include <utility>

namespace itk
{

/** \class ImportImageContainer
 * Defines an itk::Image front-end to a standard C-array. This container
 * conforms to the ImageContainerInterface. This is a full-fleged Object,
 * so there is modification time, debug, and reference count information.
 *
 * Template parameters for ImportImageContainer:
 *
 * TElementIdentifier =
 *     An INTEGRAL type for use in indexing the imported buffer.
 *
 * TElement =
 *    The element type stored in the container.
 *
 * \ingroup ImageObjects
 * \ingroup IOFilters
 */
  
template <typename TElementIdentifier, typename TElement>
class ImportImageContainer:  public Object
{
public:
  /** Standard class typedefs. */
  typedef ImportImageContainer      Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
    
  /** Save the template parameters. */
  typedef TElementIdentifier  ElementIdentifier;
  typedef TElement            Element;
    
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Standard part of every itk Object. */
  itkTypeMacro(ImportImageContainer, Object);

  /** Get the pointer from which the image data is imported. */
  TElement *GetImportPointer() {return m_ImportPointer;};

  /** Set the pointer from which the image data is imported.  "num" is
   * the number of pixels in the block of memory. If
   * "LetContainerManageMemory" is false, then the application retains
   * the responsibility of freeing the memory for this image data.  If
   * "LetContainerManageMemory" is true, then this class will free the
   * memory when this object is destroyed. */
  void SetImportPointer(TElement *ptr, TElementIdentifier num,
                        bool LetContainerManageMemory = false);

  /** Index operator. This version can be an lvalue. */
  TElement & operator[](const ElementIdentifier id)
    { return m_ImportPointer[id]; }

  /** Index operator. This version can only be an rvalue */
  const TElement & operator[](const ElementIdentifier id) const
    { return m_ImportPointer[id]; }

  /** Return a pointer to the beginning of the buffer.  This is used by
   * the image iterator class. */
  TElement *GetBufferPointer()
    { return m_ImportPointer; }
  
  /** Get the capacity of the container. */
  unsigned long Capacity(void) const
    { return (unsigned long) m_Capacity; }

  /** Get the number of elements currently stored in the container. */
  unsigned long Size(void) const
    { return (unsigned long) m_Size; }

  /** Tell the container to allocate enough memory to allow at least
   * as many elements as the size given to be stored.  If new memory
   * needs to be allocated, the contents of the old buffer are copied
   * to the new area.  The old buffer is deleted if the original pointer
   * was passed in using "LetContainerManageMemory"=true. The new buffer's
   * memory management will be handled by the container from that point on.
   *
   * In general, Reserve should not change the usable elements of the
   * container. However, in this particular case, Reserve as a Resize
   * semantics that is kept for backward compatibility reasons.
   *
   * \sa SetImportPointer() */
  void Reserve(ElementIdentifier num);
  
  /** Tell the container to try to minimize its memory usage for
   * storage of the current number of elements.  If new memory is
   * allocated, the contents of old buffer are copied to the new area.
   * The previous buffer is deleted if the original pointer was in
   * using "LetContainerManageMemory"=true.  The new buffer's memory
   * management will be handled by the container from that point on. */
  void Squeeze(void);
  
  /** Tell the container to release any of its allocated memory. */
  void Initialize(void);


  /** These methods allow to define whether upon destruction of this class
   *  the memory buffer should be released or not.  Setting it to true
   *  (or ON) makes that this class will take care of memory release.
   *  Setting it to false (or OFF) will prevent the destructor from
   *  deleting the memory buffer. This is desirable only when the data
   *  is intended to be used by external applications.
   *  Note that the normal logic of this class set the value of the boolean
   *  flag. This may override your setting if you call this methods prematurely.
   *  \warning Improper use of these methods will result in memory leaks */
  itkSetMacro(ContainerManageMemory,bool);
  itkGetConstMacro(ContainerManageMemory,bool);
  itkBooleanMacro(ContainerManageMemory);


protected:
  ImportImageContainer();
  virtual ~ImportImageContainer();

  /** PrintSelf routine. Normally this is a protected internal method. It is
   * made public here so that Image can call this method.  Users should not
   * call this method but should call Print() instead. */
  void PrintSelf(std::ostream& os, Indent indent) const;

  virtual TElement* AllocateElements(ElementIdentifier size) const;
  virtual void DeallocateManagedMemory();

  /* Set the m_Size member that represents the number of elements
   * currently stored in the container. Use this function with great
   * care since it only changes the m_Size member and not the actual size
   * of the import pointer m_ImportPointer. It should typically
   * be used only to override AllocateElements and
   * DeallocateManagedMemory. */
  itkSetMacro(Size,TElementIdentifier);
   
  /* Set the m_Capacity member that represents the capacity of
   * the current container. Use this function with great care
   * since it only changes the m_Capacity member and not the actual
   * capacity of the import pointer m_ImportPointer. It should typically
   * be used only to override AllocateElements and
   * DeallocateManagedMemory. */
  itkSetMacro(Capacity,TElementIdentifier);

  
  /* Set the m_ImportPointer member. Use this function with great care
   * since it only changes the m_ImportPointer member but not the m_Size
   * and m_Capacity members. It should typically be used only to override
   * AllocateElements and DeallocateManagedMemory. */
  void SetImportPointer(TElement *ptr){m_ImportPointer=ptr;}

private:
  ImportImageContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  TElement            *m_ImportPointer;
  TElementIdentifier   m_Size;
  TElementIdentifier   m_Capacity;
  bool                 m_ContainerManageMemory;
};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_ImportImageContainer(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT ImportImageContainer< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef ImportImageContainer< ITK_TEMPLATE_2 x > ImportImageContainer##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkImportImageContainer+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkImportImageContainer.txx"
#endif

#endif