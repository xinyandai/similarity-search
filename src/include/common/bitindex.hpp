#include "index.hpp"
namespace ss {

	template<class DataType>
	class BinaryIndex : public Index<DataType> {
		
		BinaryIndex() {}

		virtual void preprocess_train(lshbox::Matrix<DataType > ) {}
		virtual void preprocess_base (lshbox::Matrix<DataType > ) {}
		virtual void preprocess_query(lshbox::Matrix<DataType > ) {}
		
		virtual void train(lshbox::Matrix<DataType >) {} 
		virtual void add  (lshbox::Matrix<DataType >) {}

		virtual ~BinaryIndex() {}
	};
	
}
