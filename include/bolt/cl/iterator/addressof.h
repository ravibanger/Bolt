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
#if 0
    /*******************Acquire OpenCL cl_mem Buffers **********************/
    //template <typename InputIterator>
    //void acquireBuffers(typename bolt::cl::permutation_iterator_tag, 
    //                    typename InputIterator::tuple first_pointer, userFunctors)
    //{
    //    
    //}
    /*******************Create device side Iterators **********************/
    template <typename ElementIterator, typename IndexIterator>
    bolt::cl::permutation_iterator<typename bolt::cl::device_vector<typename ElementIterator::value_type>::iterator, 
                                   typename bolt::cl::device_vector<typename IndexIterator::value_type>::iterator>
    create_device_buffers(bolt::cl::permutation_iterator_tag, 
                          bolt::cl::permutation_iterator<ElementIterator, IndexIterator> &begin, 
                          bolt::cl::permutation_iterator<ElementIterator, IndexIterator> &end, 
                          bolt::cl::control &ctl)
    {

        typedef typename bolt::cl::permutation_iterator<ElementIterator, IndexIterator>::index_type *index_type_ptr;
        typedef typename bolt::cl::permutation_iterator<ElementIterator, IndexIterator>::value_type *value_type_ptr;
        index_type_ptr index_ptr_begin = begin.getIndex_pointer();
        value_type_ptr value_ptr_begin = begin.getElement_pointer();

        size_t values_size = end.m_elt_iter - begin.m_elt_iter;
        size_t index_size = end - begin;
        ::cl::Buffer *value_buffer = new ::cl::Buffer( ctl.getContext(), CL_MEM_USE_HOST_PTR|CL_MEM_READ_ONLY, 
                                                       values_size * sizeof( typename ElementIterator::value_type ),
                                                       value_ptr_begin);
        
        ::cl::Buffer *index_buffer = new ::cl::Buffer( ctl.getContext(), CL_MEM_USE_HOST_PTR|CL_MEM_READ_ONLY, 
                                                       index_size * sizeof( typename IndexIterator::value_type ),
                                                       index_ptr_begin);
        bolt::cl::device_vector<typename ElementIterator::value_type> *dv_values_vector = new bolt::cl::device_vector<typename ElementIterator::value_type>(*value_buffer, ctl);
        bolt::cl::device_vector<typename IndexIterator::value_type> *dv_index_vector = new bolt::cl::device_vector<typename IndexIterator::value_type>(*index_buffer, ctl);
        return bolt::cl::make_permutation_iterator((*dv_values_vector).begin(), (*dv_index_vector).begin());
        //bolt::cl::device_vector<typename ElementIterator::value_type> dv_values_vector (*value_buffer, ctl);
        //bolt::cl::device_vector<typename IndexIterator::value_type> dv_index_vector (*index_buffer, ctl);

        //bolt::cl::device_vector<typename IndexIterator::value_type>::iterator element_itr(dv_index_vector, 0);
        //bolt::cl::device_vector<typename IndexIterator::value_type>::iterator indx_itr(dv_index_vector, 0);

        //return bolt::cl::make_permutation_iterator( element_itr, indx_itr);
    }



    /*******************Create device side Iterators **********************/
    template <typename ElementIterator, typename IndexIterator>
    const bolt::cl::permutation_iterator<ElementIterator, IndexIterator>
    create_device_itr(bolt::cl::permutation_iterator_tag, 
                      bolt::cl::permutation_iterator<ElementIterator, IndexIterator> itr, 
                      ElementIterator element_itr,
                      IndexIterator   index_itr)
    {
        return permutation_iterator<ElementIterator, IndexIterator> (element_itr, index_itr);
    } 
#endif
    template <typename Iterator, typename DeviceIterator>
    const transform_iterator<typename Iterator::unary_func, DeviceIterator>
    create_device_itr(bolt::cl::transform_iterator_tag, Iterator itr, DeviceIterator dev_itr_1)
    {
        typedef typename Iterator::unary_func unary_func;
        return transform_iterator<unary_func, DeviceIterator> (dev_itr_1, itr.functor());
    }   

    template <typename Iterator, typename DeviceIterator>
    const typename bolt::cl::device_vector<typename Iterator::value_type>::iterator 
    create_device_itr(std::random_access_iterator_tag, Iterator itr, DeviceIterator dev_itr_1)
    {
        return dev_itr_1;
    }

    template <typename T, typename DeviceIterator>
    const typename bolt::cl::device_vector<T>::iterator 
    create_device_itr(std::random_access_iterator_tag, T* ptr, DeviceIterator dev_itr_1)
    {
        return dev_itr_1;
    }

    template <typename Iterator, typename DeviceIterator>
    const constant_iterator<typename Iterator::value_type> 
    create_device_itr(bolt::cl::constant_iterator_tag, Iterator itr, DeviceIterator dev_itr_1)
    {
        return itr;
    }

    template <typename Iterator, typename DeviceIterator>
    const counting_iterator<typename Iterator::value_type> 
    create_device_itr(bolt::cl::counting_iterator_tag, Iterator itr, DeviceIterator dev_itr_1)
    {
        return itr;
    }
    
    /******************* Create Mapped Iterators **********************/

    template <typename Iterator, typename T>
    transform_iterator<typename Iterator::unary_func, T*>
    create_mapped_iterator(bolt::cl::transform_iterator_tag, bolt::cl::control &ctl, Iterator &itr, T* ptr)
    {
        typedef typename Iterator::unary_func unary_func;
        return transform_iterator<unary_func, T*> (ptr, itr.functor());
    }   

    template <typename Iterator, typename T>
    T*
    create_mapped_iterator(bolt::cl::device_vector_tag, bolt::cl::control &ctl, Iterator &itr, T* ptr)
    {
        return ptr + itr.m_Index;
    }

    /*TODO - The current constant and counting iterator implementations are buggy. 
      They create an OpenCL device buffer even if the iterator is to be used on host only.
   */
    template <typename Iterator, typename T>
    const constant_iterator<typename Iterator::value_type> &
    create_mapped_iterator(bolt::cl::constant_iterator_tag, bolt::cl::control &ctl, Iterator &itr, T* ptr)
    {
        return itr;
    }
    
    template <typename Iterator, typename T>
    const counting_iterator<typename Iterator::value_type> &
    create_mapped_iterator(bolt::cl::counting_iterator_tag, bolt::cl::control &ctl, Iterator &itr, T* ptr)
    {
        return itr;
    }

    template <typename Iterator, typename T>
    const permutation_iterator<typename Iterator::value_type*, typename Iterator::index_type*>
    create_mapped_iterator(bolt::cl::permutation_iterator_tag, bolt::cl::control &ctl,  Iterator &itr, typename T* ptr)
    {
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::index_type index_type;
        index_type *i_ptr = ptr;
        ::cl::Buffer elementBuffer  = itr.m_elt_iter.getContainer( ).getBuffer( );
        size_t elementBuffer_sz  = elementBuffer.getInfo<CL_MEM_SIZE>();
        cl_int map_err;
        value_type *mapped_elementPtr = (value_type*)ctl.getCommandQueue().enqueueMapBuffer(elementBuffer, true, 
                                                                                       CL_MAP_WRITE, 0, 
                                                                                       elementBuffer_sz, 
                                                                                       NULL, NULL, &map_err);
        
        return bolt::cl::make_permutation_iterator (mapped_elementPtr, i_ptr);
    }

    template <typename Iterator, typename T>
    void release_mapped_iterator(bolt::cl::permutation_iterator_tag, bolt::cl::control &ctl, Iterator &itr)
    {
        typedef typename Iterator::value_type value_type;
        ::cl::Buffer elementBuffer  = itr.m_elt_iter.getContainer( ).getBuffer( );
        value_type * element_ptr = elementBuffer.getInfo<CL_MEM_HOST_PTR>();
        ctl.getCommandQueue().enqueueUnmapMemObject(elementBuffer, element_ptr, NULL, );
        return;
    }

}} //namespace bolt::cl

#endif
