
#include "IdResolverImpl.hpp"
#include "fsa/const.hpp"
#include "utils.hpp"
#include "const.hpp"
#include "deserialization/deserializationUtils.hpp"
#include "morfeusz2.h"

using namespace std;

namespace morfeusz {

    inline static void readTags(const unsigned char*& currPtr, std::vector<std::string>& tags) {
        tags.clear();
        tags.resize(65536);
        uint16_t tagsNum = readInt16(currPtr);
        for (unsigned int i = 0; i < tagsNum; i++) {
            unsigned int tagNum = readInt16(currPtr);
            tags[tagNum] = readString(currPtr);
        }
    }

    inline static void createReverseMapping(IdResolverImpl::IdStringMapping& mapping) {
        mapping.string2Id.clear();
        for (unsigned int i = 0; i < mapping.id2String.size(); i++) {
            mapping.string2Id[mapping.id2String[i]] = i;
        }
    }

    template <class T>
    inline static const T& getFromMap(map<string, T> string2T, const string& key, const char* errMsg) {
        if (string2T.count(key) != 0) {
            return string2T.find(key)->second;
        }
        else {
            throw MorfeuszException(string(errMsg) + ": " + key);
        }
    }
    
    inline static void convertCharset(const CharsetConverter* charsetConverter, IdResolverImpl::IdStringMapping& mapping) {
        for (unsigned int i = 0; i < mapping.id2String.size(); i++) {
            mapping.id2String[i] = charsetConverter->fromUTF8(
                    charsetConverter->toUTF8(mapping.id2String[i]));
        }
        createReverseMapping(mapping);
    }
    
    IdResolverImpl::IdResolverImpl()
    : tagsetId(),
    tags(),
    names(),
    labels(),
    labelsAsSets(),
    charsetConverter(&UTF8CharsetConverter::getInstance()) {
        
    }

    IdResolverImpl::IdResolverImpl(const unsigned char* ptr, const CharsetConverter* charsetConverter)
    : tagsetId(),
    tags(),
    names(),
    labels(),
    labelsAsSets(),
    charsetConverter(charsetConverter) {
//        uint32_t fsaSize = readInt32Const(ptr + FSA_DATA_SIZE_OFFSET);
//        const unsigned char* currPtr = ptr + FSA_DATA_OFFSET + fsaSize;
        const unsigned char* currPtr = getEpiloguePtr(ptr) + 4;
        
        readString(currPtr); // skip dictId
        readString(currPtr); // skip copyright
        
        this->tagsetId = readString(currPtr);
        readTags(currPtr, this->tags.id2String);
        createReverseMapping(this->tags);

        readTags(currPtr, this->names.id2String);
        createReverseMapping(this->names);

        readTags(currPtr, this->labels.id2String);
        createReverseMapping(this->labels);
        for (unsigned int i = 0; i < this->labels.id2String.size(); i++) {
            vector<string> labelsVector = split(this->labels.id2String[i], LABELS_SEPARATOR);
            this->labelsAsSets.push_back(set<string>(labelsVector.begin(), labelsVector.end()));
        }

        setCharsetConverter(charsetConverter);
    }
    
    // FIXME - probably should not convert whole tagset on every setCharsetConverter method invocation.

    void IdResolverImpl::setCharsetConverter(const CharsetConverter* charsetConverter) {
        convertCharset(charsetConverter, this->tags);
        convertCharset(charsetConverter, this->names);
        convertCharset(charsetConverter, this->labels);

        this->charsetConverter = charsetConverter;
    }
    
    const string IdResolverImpl::getTagsetId() const {
        return this->tagsetId;
    }

    const string& IdResolverImpl::getTag(const int tagId) const {
        return this->tags.id2String.at(tagId);
    }

    int IdResolverImpl::getTagId(const std::string& tag) const {
        return getFromMap(this->tags.string2Id, tag, "Invalid tag");
    }

    const string& IdResolverImpl::getName(const int nameId) const {
        return this->names.id2String.at(nameId);
    }

    int IdResolverImpl::getNameId(const std::string& name) const {
        return getFromMap(this->names.string2Id, name, "Invalid name");
    }

    const string& IdResolverImpl::getLabelsAsString(int labelsId) const {
        return this->labels.id2String.at(labelsId);
    }

    const set<string>& IdResolverImpl::getLabels(int labelsId) const {
        return this->labelsAsSets.at(labelsId);
    }

    int IdResolverImpl::getLabelsId(const string& labelsStr) const {
        return getFromMap(this->labels.string2Id, labelsStr, "Invalid labels string");
    }

    size_t IdResolverImpl::getTagsCount() const {
        return this->tags.id2String.size();
    }

    size_t IdResolverImpl::getNamesCount() const {
        return this->names.id2String.size();
    }

    size_t IdResolverImpl::getLabelsCount() const {
        return this->labels.id2String.size();
    }
    
    bool IdResolverImpl::isCompatibleWith(const IdResolverImpl& other) const {
        return this->tagsetId == other.tagsetId
                && this->tags.id2String == other.tags.id2String 
                && this->names.id2String == other.names.id2String
                && this->labels.id2String == other.labels.id2String;
    }
}
