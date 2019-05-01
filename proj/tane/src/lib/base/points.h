#pragma once

#include "base/pch.h"

#include "base/logger.h"

#include "base/vector.h"
#include "base/matrix.h"

#include "base/uniforms.h"
#include "base/attributes.h"

#include <thrust/remove.h>
#include <thrust/execution_policy.h>
#include <thrust/system/omp/execution_policy.h>

NAMESPACE_BASE_BEGIN
class Points final
{
public:
	/*------------------------------------------------------ enum*/
	enum MaskType
	{
		kVISIBLE_MASK,
		kDELETE_MASK,
	};

public:
	/*------------------------------------------------------ typedefs*/
	typedef typename Point3fArray::value_type      value_type;
	typedef typename Point3fArray::pointer         pointer;
	typedef typename Point3fArray::const_pointer   const_pointer;
	typedef typename Point3fArray::reference       reference;
	typedef typename Point3fArray::const_reference const_reference;
	typedef typename Point3fArray::iterator        iterator;
	typedef typename Point3fArray::const_iterator  const_iterator;
	typedef typename Point3fArray::size_type       size_type;
	typedef typename Point3fArray::difference_type difference_type;

public:
	/*------------------------------------------------------ static variables */

public:
	/*------------------------------------------------------ static public functions */

private:
	/*------------------------------------------------------ private variables */
	size_type    npoints_;
	Point3fArray points_;
	Uniforms     uniforms_;
	Attributes   attributes_;

public:
	/*------------------------------------------------------ creator */
	static const std::shared_ptr<Points> Create(const size_t npoints = 0);
	static const std::shared_ptr<Points> Create(const size_t& sz,
	                                            const std::initializer_list<Uniforms::value_type>& ulst,
	                                            const std::initializer_list<Attributes::value_type>& alst);


public:
	Points(const size_t& sz,
	       const std::initializer_list<Uniforms::value_type>& ulst,
	       const std::initializer_list<Attributes::value_type>& alst);
	Points(const Point3fArray& array,
	       const std::initializer_list<Uniforms::value_type>& ulst = std::initializer_list<Uniforms::value_type>(),
	       const std::initializer_list<Attributes::value_type>& alst = std::initializer_list<Attributes::value_type>());
	~Points();

public:
	/*------------------------------------------------------ public functions */
	///------------------------------------------------------------ operator overloading
	reference operator[](size_type idx);
	const_reference operator[](size_type idx) const;
public:
	///--------------------------------------------------------------------------------------- position query
	size_type                     size() const;
	Point3fArray&                 get();
	const Point3fArray&           get() const;
	Point3fArray::iterator        begin();
	Point3fArray::iterator        end();
	Point3fArray::const_iterator  cbegin() const;
	Point3fArray::const_iterator  cend() const;
	pointer                       data() ;
	const_pointer                 data() const;

	///--------------------------------------------------------------------------------------- attribute query
	bool exists(const Attributes::key_type& key) const;
	
	template<typename AttrT> Array1 <AttrT>& get(const Attributes::key_type& key);
	template<typename AttrT> const Array1 <AttrT>& get(const Attributes::key_type& key) const;
	template<typename AttrT> typename Array1<AttrT>::iterator begin(const Attributes::key_type& key);
	template<typename AttrT> typename Array1<AttrT>::iterator end(const Attributes::key_type& key);
	template<typename AttrT> typename Array1<AttrT>::const_iterator cbegin(const Attributes::key_type& key) const;
	template<typename AttrT> typename Array1<AttrT>::const_iterator cend(const Attributes::key_type& key) const;
	template<typename AttrT> typename Array1<AttrT>::pointer data(const Attributes::key_type& key);
	template<typename AttrT> typename Array1<AttrT>::const_pointer data(const Attributes::key_type& key) const;
	void append(Points& other);
	void remove(const Array1<size_t>& deleteIDs);
	void remove(const Array1<bool>& maskArray, const MaskType& maskType = kVISIBLE_MASK);
	void clear();
	void swap(Points& other);

private:
	/*------------------------------------------------------ private functions */
	void set_(const std::initializer_list<Uniforms::value_type>& ulst);
	void set_(const std::initializer_list<Attributes::value_type>& alst);
	void valid_() const;

private:
	/*------------------------------------------------------ archive */
	friend class boost::serialization::access;
	template<typename ARCHIVE> void serialize(ARCHIVE& ar, const unsigned int ver);
private:
};


NAMESPACE_BASE_END
