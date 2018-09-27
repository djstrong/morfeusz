
namespace morfeusz {
    
    %ignore Morfeusz::analyse(const char*) const;
    %ignore Morfeusz::analyse(const std::string&) const;
    %ignore Morfeusz::setCharset(Charset);
    %ignore Morfeusz::getCharset() const;
    %ignore Morfeusz::setDebug(bool);
    %ignore Charset;
    
    %newobject Morfeusz::createInstance();
    %newobject Morfeusz::_analyseAsIterator(const char*) const;
}

%extend morfeusz::Morfeusz {
    morfeusz::ResultsIterator* morfeusz::Morfeusz::_analyseAsIterator(const char* text) const {
        return dynamic_cast<const morfeusz::MorfeuszImpl*>($self)->analyseWithCopy(text);
    }
}

%template(InterpsList) std::vector<morfeusz::MorphInterpretation>;
%template(StringsList) std::vector<std::string>;
%template(StringsLinkedList) std::list<std::string>;

#ifndef SWIGPERL
%template(StringsSet) std::set<std::string>;
#endif
