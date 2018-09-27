/* 
 * File:   tagset.hpp
 * Author: mlenart
 *
 * Created on 12 listopad 2013, 14:09
 */

#ifndef DEFAULTTAGSET_HPP
#define	DEFAULTTAGSET_HPP

#include <string>
#include <vector>
#include <map>
#include "morfeusz2.h"
#include "charset/CharsetConverter.hpp"

namespace morfeusz {

    class IdResolverImpl : public IdResolver {
    public:
        
        IdResolverImpl(const unsigned char* ptr, const CharsetConverter* charsetConverter);
        
        IdResolverImpl();
        
        void setCharsetConverter(const CharsetConverter* charsetConverter);
        
        const std::string getTagsetId() const;

        const std::string& getTag(const int tagId) const;
        int getTagId(const std::string& tag) const;

        const std::string& getName(const int nameId) const;
        int getNameId(const std::string& name) const;

        const std::string& getLabelsAsString(int labelsId) const;
        const std::set<std::string>& getLabels(int labelsId) const;
        int getLabelsId(const std::string& labelsStr) const;

        size_t getTagsCount() const;
        size_t getNamesCount() const;
        size_t getLabelsCount() const;

        virtual ~IdResolverImpl() {
        }
        
        struct IdStringMapping {
            std::vector<std::string> id2String;
            std::map<std::string, int> string2Id;
        };
        
        bool isCompatibleWith(const IdResolverImpl& other) const;
        
    private:
        std::string tagsetId;
        IdStringMapping tags;
        IdStringMapping names;
        IdStringMapping labels;
        std::vector< std::set<std::string> > labelsAsSets;
        
        const CharsetConverter* charsetConverter;
    };

}

#endif	/* DEFAULTTAGSET_HPP */

