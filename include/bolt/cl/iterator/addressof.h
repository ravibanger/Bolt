#ifndef BOLT_ADDRESSOF_H
#define BOLT_ADDRESSOF_H
#include <bolt/cl/device_vector.h>
#include <bolt/cl/iterator/permutation_iterator.h>
#include <bolt/cl/iterator/transform_iterator.h>
#include <bolt/cl/iterator/counting_iterator.h>
#include <bolt/cl/iterator/constant_iterator.h>

namespace bolt{
namespace cl{

    template <typename Iterator>
    typename Iterator::value_type * addressof(Iterator itr)
    {
        return std::addressof(*itr);
    }

    template <typename T>
    T * addressof(T* itr)
    {
        return itr;
    }

    template <typename ElementIterator, typename IndexIterator>
    //typename bolt::cl::permutation_iterator<ElementIterator, IndexIterator>::pointer 
    std::tuple<typename ElementIterator::value_type*, typename IndexIterator::value_type*>
        addressof(typename bolt::cl::permutation_iterator<ElementIterator, IndexIterator> itr)
    {
        /*Note that the pointer in permutation iterator is pointer to the element iterator value_type. 
          But here we will need a pointer to IndexIterator::value_type. 
          This is done because the distance between two permutation iterator is equal to the 
          distance between the corresponding IndexIterator*/
        typedef typename bolt::cl::permutation_iterator<ElementIterator, IndexIterator>::index_type *index_type_ptr;
        typedef typename bolt::cl::permutation_iterator<ElementIterator, IndexIterator>::value_type *value_type_ptr;
        index_type_ptr index_ptr = itr;
        value_type_ptr value_ptr = itr;
        return std::make_tuple(value_ptr, index_ptr);
    }

    template <typename UnaryFunction, typename Iterator>
    typename bolt::cl::transform_iterator<UnaryFunction, Iterator>::pointer 
        addressof(typename bolt::cl::transform_iterator<UnaryFunction, Iterator> itr)
    {
        typedef typename bolt::cl::transform_iterator<UnaryFunction, Iterator>::pointer pointer;
        pointer ptr = itr;
        return ptr;
    }

    template <typename value_type>
    typename bolt::cl::counting_iterator<value_type>::pointer 
        addressof(typename bolt::cl::counting_iterator<value_type> itr)
    {
        typedef typename bolt::cl::counting_iterator<value_type>::pointer pointer;
        pointer ptr = itr;
        return ptr;
    }

    template <typename value_type>
    typename bolt::cl::constant_iterator<value_type>::pointer 
        addressof(typename bolt::cl::constant_iterator<value_type> itr)
    {
        typedef typename bolt::cl::constant_iterator<value_type>::pointer pointer;
        pointer ptr = itr;
        return ptr;
    }

    
    /*******************Create device side Iterators **********************/

    template <typename Iterator, typename DeviceIterator>
    const transform_iterator<typename Iterator::unary_func, DeviceIterator>
    create_device_itr(bolt::cl::permutation_iterator_tag, Iterator itr, DeviceIterator dev_itr)
    {
        typedef typename Iterator::unary_func unary_func;
        return transform_iterator<unary_func, DeviceIterator> (dev_itr, itr.functor());
    } 

    template <typename Iterator, typename DeviceIterator>
    const transform_iterator<typename Iterator::unary_func, DeviceIterator>
    create_device_itr(bolt::cl::transform_iterator_tag, Iterator itr, DeviceIterator dev_itr)
    {
        typedef typename Iterator::unary_func unary_func;
        return transform_iterator<unary_func, DeviceIterator> (dev_itr, itr.functor());
    }   

    template <typename Iterator, typename DeviceIterator>
    const typename bolt::cl::device_vector<typename Iterator::value_type>::iterator 
    create_device_itr(std::random_access_iterator_tag, Iterator itr, DeviceIterator dev_itr)
    {
        return dev_itr;
    }

    template <typename T, typename DeviceIterator>
    const typename bolt::cl::device_vector<T>::iterator 
    create_device_itr(std::random_access_iterator_tag, T* ptr, DeviceIterator dev_itr)
    {
        return dev_itr;
    }

    template <typename Iterator, typename DeviceIterator>
    const constant_iterator<typename Iterator::value_type> 
    create_device_itr(bolt::cl::constant_iterator_tag, Iterator itr, DeviceIterator dev_itr)
    {
        return itr;
    }

    template <typename Iterator, typename DeviceIterator>
    const counting_iterator<typename Iterator::value_type> 
    create_device_itr(bolt::cl::counting_iterator_tag, Iterator itr, DeviceIterator dev_itr)
    {
        return itr;
    }
    
    /******************* Create Mapped Iterators **********************/

    template <typename Iterator, typename T>
    transform_iterator<typename Iterator::unary_func, T*>
    create_mapped_iterator(bolt::cl::transform_iterator_tag, Iterator &itr, T* ptr)
    {
        typedef typename Iterator::unary_func unary_func;
        return transform_iterator<unary_func, T*> (ptr, itr.functor());
    }   

    template <typename Iterator, typename T>
    T*
    create_mapped_iterator(bolt::cl::device_vector_tag, Iterator &itr, T* ptr)
    {
        return ptr + itr.m_Index;
    }

    /*TODO - The current constant and counting iterator implementations are buggy. 
      They create an OpenCL device buffer even if the iterator is to be used on host only.
   */
    template <typename Iterator, typename T>
    const constant_iterator<typename Iterator::value_type> &
    create_mapped_iterator(bolt::cl::constant_iterator_tag, Iterator &itr, T* ptr)
    {
        return itr;
    }
    
    template <typename Iterator, typename T>
    const counting_iterator<typename Iterator::value_type> &
    create_mapped_iterator(bolt::cl::counting_iterator_tag, Iterator &itr, T* ptr)
    {
        return itr;
    }


}} //namespace bolt::cl

#endif
