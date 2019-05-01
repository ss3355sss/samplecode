#pragma once
#include "base/pch.h"


#include "base/attributes.h"
#include "base/points.h"

#include <thrust/execution_policy.h>
#include <thrust/for_each.h>
#include <thrust/system/omp/execution_policy.h>
NAMESPACE_BASE_BEGIN
struct Partition final
{
	int baseInstance = 0;
	int	primCount 	 = 0;
};

class InstancingItem final
{
public:
	typedef int                          ElemIndex;
	typedef std::map<int, Partition> PartitionMap;

private:
	Int32Array   sourceIDs_;
	PartitionMap partitionMap_;

	int             npoints_;
	Int32Array      pidArray_;
	Int32Array      sidArray_;
	Int32Array      renderableArray_;
	Mat4x4fArray    instancingMtxArray_;
	TexCoord3fArray uvArray_;
	RGBColorArray   colorArray_;        //r, g, b
	RGBColorArray   wireColorArray_;    //r, g, b
	FloatArray      radiusArray_;
	
public:
	static const std::shared_ptr<InstancingItem> Create(const Int32Array& sourceIDs = Array1<int>(1, 0),
	                                                    const std::shared_ptr<Points>& pPoints = Points::Create(1)
	)
	{
		return std::make_shared<InstancingItem>(sourceIDs, pPoints);
	}
public:
	InstancingItem(const Array1<int32_t>& sourceIDs, const std::shared_ptr<Points>& pPoints)
	{
		reset2(sourceIDs, pPoints);
	}
	~InstancingItem()
	{
	}

public:
	const Partition partition(int instanceID) const
	{
		if(partitionMap_.empty())
			return Partition();
		
		if(partitionMap_.find(instanceID) == partitionMap_.end())
			return Partition();
		
		return partitionMap_.at(instanceID);
		
	}
public:
	size_t nsources() const
	{
		return partitionMap_.size();
	}
	size_t npoints() const
	{
		return npoints_;
	}
public:
	const Int32Array&       getPidArray() const             { return pidArray_;             }
	const Int32Array&       getSidArray() const             { return sidArray_;             }
	const Int32Array&       getRenderableArray() const      { return renderableArray_;      }
	const Mat4x4fArray&     getInstancingMtxArray() const   { return instancingMtxArray_;   }
	const TexCoord3fArray&  getUVArray() const              { return uvArray_;              }
	const FloatArray&       getRadiusArray() const          { return radiusArray_;          }
	
	Int32Array&             getPidArray()                   { return pidArray_;             }
	Int32Array&             getSidArray()                   { return sidArray_;             }
	Int32Array&             getRenderableArray()            { return renderableArray_;      }
	Mat4x4fArray&           getInstancingMtxArray()         { return instancingMtxArray_;   }
	TexCoord3fArray&        getUVArray()                    { return uvArray_;              }
	FloatArray&             getRadiusArray()                { return radiusArray_;          }


public:
	void reset2(const Array1<int>& sourceIDs, const std::shared_ptr<Points>& pPoints)
	{
		DECL_LOGGER(base::InstanceItem);
		assign_(pPoints);
		sorting_();
		partitioning_(sourceIDs);
	}
	void clear()
	{
		sourceIDs_.clear();
		partitionMap_.clear();
		
		npoints_ = 0;
		pidArray_.clear();
		sidArray_.clear();
		
		renderableArray_.clear();
		
		instancingMtxArray_.clear();
		
		uvArray_.clear();
		
		radiusArray_.clear();
	}
	void resize(size_t sz)
	{
		npoints_ = sz;
		pidArray_.resize(sz);
		sidArray_.resize(sz);
		renderableArray_.resize(sz);
		instancingMtxArray_.resize(sz);
		uvArray_.resize(sz);
		colorArray_.resize(sz);        //r, g, b
		wireColorArray_.resize(sz);    //r, g, b
		radiusArray_.resize(sz);
	}

private:
	void assign_(const std::shared_ptr<Points> pPoints)
	{
		DECL_LOGGER(base::InstanceItem);
		
		/* --------------------------------------------- assign data */
		npoints_            = pPoints->size();
		pidArray_           = pPoints->get<int>(sementic::attribute::pid);
		sidArray_           = pPoints->get<int>(sementic::attribute::sid);
		renderableArray_    = pPoints->get<int>(sementic::attribute::renderable);
		
		LOGGER_DEBUG("npoints = %d", npoints_);

		
		instancingMtxArray_.resize(npoints_);
		{
			const Point3fArray& positionArray   = pPoints->get();
			const Vec3fArray  & eulerAngleArray = pPoints->get<Vec3f>(sementic::attribute::eulerAngle);
			const Vec3fArray  & scaleArray      = pPoints->get<Vec3f>(sementic::attribute::scale);
			
			LOGGER_DEBUG("positionArray.size()   = %d", positionArray.size());
			LOGGER_DEBUG("eulerAngleArray.size() = %d", eulerAngleArray.size());
			LOGGER_DEBUG("scaleArray.size()      = %d", scaleArray.size());
			
			thrust::for_each(thrust::system::omp::par,
			                 thrust::counting_iterator<size_t>(0),
			                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints_)),
			                 [&](const size_t idx)
			{
				const Mat4x4f T = {
					1.0f, 0.0f, 0.0f, positionArray[idx][0],
					0.0f, 1.0f, 0.0f, positionArray[idx][1],
					0.0f, 0.0f, 1.0f, positionArray[idx][2],
					0.0f, 0.0f, 0.0f, 1.0f,
				};
				
				float c, s;
				c = std::cos(eulerAngleArray[idx][0]);
				s = std::sin(eulerAngleArray[idx][0]);
				
				const Mat4x4f Rx = {
					1.0f, 0.0f, 0.0f,  0.0f,
					0.0f,   +c,   -s,  0.0f,
					0.0f,   +s,   +c,  0.0f,
					0.0f, 0.0f, 0.0f,  1.0f,
				};
				c = std::cos(eulerAngleArray[idx][1]);
				s = std::sin(eulerAngleArray[idx][1]);
				const Mat4x4f Ry = {
					  +c, 0.0f,   +s,  0.0f,
					0.0f, 1.0f, 0.0f,  0.0f,
					  -s, 0.0f,   +c,  0.0f,
					0.0f, 0.0f, 0.0f,  1.0f,
				};
				c = std::cos(eulerAngleArray[idx][2]);
				s = std::sin(eulerAngleArray[idx][2]);
				const Mat4x4f Rz = {
					  +c,   -s, 0.0f, 0.0f,
					  +s,   +c, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f,  0.0f,
					0.0f, 0.0f, 0.0f,  1.0f,
				};
				
				const Mat4x4f S = {
					scaleArray[idx][0], 0.0f              , 0.0f              , 0.0f,
					0.0f              , scaleArray[idx][1], 0.0f              , 0.0f,
					0.0f              , 0.0f              , scaleArray[idx][2], 0.0f,
					0.0f              , 0.0f              , 0.0f              , 1.0f,
				};
				instancingMtxArray_[idx] = T * Rz * Ry * Rx * S;
				
			});
		}
		uvArray_            = pPoints->get<TexCoord3f>(sementic::attribute::uvCoord);
		radiusArray_        = pPoints->get<float>(sementic::attribute::radius);
		
	}
	void sorting_()
	{
		DECL_LOGGER(base::InstanceItem);
		
		/* --------------------------------------------- sort data by sid */
		typedef thrust::tuple<Int32Array::iterator,      /// pid
		                      Int32Array::iterator,      /// renderable
		                      Mat4x4fArray::iterator,    /// instancingMtx
		                      TexCoord3fArray::iterator, /// uvCoord
		                      FloatArray::iterator       /// radius
		> TupleIter;
		typedef thrust::zip_iterator<TupleIter> ZipIter;
		
		ZipIter beginIter = thrust::make_tuple(pidArray_.begin(),
		                                       renderableArray_.begin(),
		                                       instancingMtxArray_.begin(),
		                                       uvArray_.begin(),
		                                       radiusArray_.begin()
		);
		
		ZipIter endIter = thrust::make_tuple(pidArray_.end(),
		                                     renderableArray_.end(),
		                                     instancingMtxArray_.end(),
		                                     uvArray_.end(),
		                                     radiusArray_.end()
		);
		
		thrust::sort_by_key(thrust::host,      //policy
		                    sidArray_.begin(), //key begin
		                    sidArray_.end(),   //key end
		                    beginIter          //output
		);
	}
	void partitioning_(const Int32Array& sourceIDs)
	{
		DECL_LOGGER(base::InstanceItem);
		
		/* --------------------------------------------- make partition map*/
		partitionMap_.clear();

		int pid         = 0;
		int offset      = 0;
		for (Int32Array::const_iterator iter = sourceIDs.cbegin(); iter != sourceIDs.cend(); ++iter)
		{
			const int& searchID = *iter;
			int cnt = std::count(sidArray_.begin(), sidArray_.end(), searchID);
			
			partitionMap_[searchID].baseInstance = offset;
			partitionMap_[searchID].primCount    = cnt;
			offset += cnt;
		}
	}

};

NAMESPACE_BASE_END


