#pragma once
#include <functional>
#include <vector>
namespace ss {

	template<typename T>
	class  SSHasher : public std::hash<T>{
	};

	template<typename T>
	class SSHasher<std::vector<T>> {
	public:
    		size_t operator()(const std::vector<T>& vec) const {
        		size_t seed = 0;
        		std::hash<T> hasher;
        		for (auto& v : vec) {
            			seed ^= hasher(v);
        		}
        		return seed;
    		}
	};
};
