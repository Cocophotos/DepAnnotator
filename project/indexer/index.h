#ifndef INDEX_H
#define INDEX_H

#include <map>
#include <stdexcept>

/**
 * This template is completely work in progress
 * and clearly not usable at this point
 */
template<class K, class V> class Index
{
public:
    Index();

    const V& value(K key)const{
        typename std::map<K, V>::const_iterator it = mIndex.find(key);
        if(it != mIndex.end())
            return it->second;
        else
            throw std::out_of_range("Key out of range");
    }

    void add(std::pair<K,V> keyValue){
        mIndex.insert(keyValue);
    }

private:
    std::map<K, V> mIndex;
};

#endif // INDEX_H
