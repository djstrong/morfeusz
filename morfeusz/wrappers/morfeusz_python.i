
%exception {
    try{
        $action
    }
    catch(const std::ios_base::failure& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const morfeusz::FileFormatException& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const std::invalid_argument& e) {
        SWIG_exception(SWIG_ValueError, const_cast<char*>(e.what()));
    }
    catch(const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.what()));
    }
    catch(const std::string& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.c_str()));
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

%exception morfeusz::Morfeusz::setDictionary {
    try{
        $action
    }
    catch(const std::ios_base::failure& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const morfeusz::MorfeuszException& e) {
        SWIG_exception(SWIG_IOError, const_cast<char*>(e.what()));
    }
    catch(const std::invalid_argument& e) {
        SWIG_exception(SWIG_ValueError, const_cast<char*>(e.what()));
    }
    catch(const std::string& e) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(e.c_str()));
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }
}

%ignore morfeusz::MorfeuszException;
%ignore morfeusz::FileFormatException;

%rename (_generateByTagId) morfeusz::Morfeusz::generate(const std::string&, int, std::vector<std::string>&);

%extend morfeusz::ResultsIterator {
    morfeusz::ResultsIterator& morfeusz::ResultsIterator::__iter__() {
        return *($self);
    }
};

%rename (_Morfeusz) morfeusz::Morfeusz;

//%ignore morfeusz::Morfeusz::createInstance(morfeusz::MorfeuszUsage);
%extend morfeusz::Morfeusz {
    std::vector<morfeusz::MorphInterpretation> morfeusz::Morfeusz::_generateByTagId(const std::string& lemma, int tagId) const {
        std::vector<morfeusz::MorphInterpretation> res;
        $self->generate(lemma, tagId, res);
        return res;
    }
};

%feature("shadow") morfeusz::ResultsIterator::next %{
def next(self):
    if self.hasNext():
        return $action(self)
    else:
        raise StopIteration
%}

%feature("shadow") morfeusz::Morfeusz::_analyseAsIterator %{

def analyse_iter(self, text):
    """
    Analyse given text and return an iterator over MorphInterpretation objects as a result.
    """
    return $action(self, text.encode('utf-8'))
%}

%feature("shadow") morfeusz::Morfeusz::analyse %{
def analyse(self, text):
    """
    Analyse given text and return a list of MorphInterpretation objects.
    """
    res = InterpsList()
    $action(self, text.encode('utf-8'), res)
    return res
%}

%feature("shadow") morfeusz::Morfeusz::setAggl %{
def setAggl(self, optionString):
    """
    Select agglutination rules option
    """
    $action(self, optionString.encode('utf8'))
%}

%feature("shadow") morfeusz::Morfeusz::setPraet %{
def setPraet(self, optionString):
    """
    Select past tense segmentation
    """
    $action(self, optionString.encode('utf8'))
%}

%feature("shadow") morfeusz::Morfeusz::setCaseHandling %{
def setCaseHandling(self, option):
    """
    Set case handling option (valid options are CONDITIONALLY_CASE_SENSITIVE, STRICTLY_CASE_SENSITIVE, IGNORE_CASE)
    """
    $action(self, option)
%}

%feature("shadow") morfeusz::Morfeusz::setTokenNumbering %{
def setTokenNumbering(self, option):
    """
    Set token numbering option (valid options are SEPARATE_NUMBERING, CONTINUOUS_NUMBERING)
    """
    $action(self, option)
%}

%feature("shadow") morfeusz::Morfeusz::setWhitespaceHandling %{
def setWhitespaceHandling(self, option):
    """
    Set whitespace handling handling option (valid options are SKIP_WHITESPACES, KEEP_WHITESPACES, APPEND_WHITESPACES)
    """
    $action(self, option)
%}

%feature("shadow") morfeusz::Morfeusz::setDictionary %{
def setDictionary(self, dictName):
    """
    Set dictionary to be used by this instance (by name)
    """
    $action(self, dictName.encode('utf8'))
%}

%feature("shadow") morfeusz::Morfeusz::createInstance(morfeusz::MorfeuszUsage) %{
@staticmethod
def _createInstance(usage):
    return $action(usage)
%}

%feature("shadow") morfeusz::Morfeusz::createInstance(const std::string&, morfeusz::MorfeuszUsage) %{
@staticmethod
def createInstance(dictName=None, usage=BOTH_ANALYSE_AND_GENERATE):
    """
    Creates new instance of Morfeusz class. Usage may be BOTH_ANALYZE_AND_GENERATE (default), ONLY_ANALYSE and ONLY_GENERATE.
    """
    if dictName is None:
        return Morfeusz._createInstance(usage)
    else:
        return $action(dictName.encode('utf8'), usage)
%}

%feature("shadow") morfeusz::Morfeusz::_generateByTagId %{
def _generateByTagId(self, lemma, tagId):
    res = InterpsList()
    $action(self, lemma, tagId, res)
    return res
%}

%feature("shadow") morfeusz::Morfeusz::generate %{
def generate(self, lemma, tagId=None):
    """
    Perform morphological synthesis on given text and return a list of MorphInterpretation objects.
    """
    lemma = lemma.encode('utf-8')
    if tagId is not None:
        return self._generateByTagId(lemma, tagId)
    else:
        res = InterpsList()
        $action(self, lemma, res)
        return res
%}

%rename (_orth) morfeusz::MorphInterpretation::orth;

%extend morfeusz::MorphInterpretation {
    %pythoncode %{
        @property
        def orth(self):
            return self._orth.decode('utf8')
        
        @orth.setter
        def orth(self, val):
            self._orth = val.encode('utf8')
    %}
};

%rename (_lemma) morfeusz::MorphInterpretation::lemma;

%extend morfeusz::MorphInterpretation {
    %pythoncode %{
        @property
        def lemma(self):
            return self._lemma.decode('utf8')
        
        @lemma.setter
        def lemma(self, val):
            self._lemma = val.encode('utf8')
    %}
};

%feature("shadow") morfeusz::MorphInterpretation::getTag %{
def getTag(self, morfeusz):
    """
    Returns tag as string.
    """
    return $action(self, morfeusz)
%}

%feature("shadow") morfeusz::MorphInterpretation::getName %{
def getName(self, morfeusz):
    """
    Returns this interpretation named entity as unicode
    """
    name = $action(self, morfeusz).decode('utf8')
    return name.split('|') if name else []
%}

%feature("shadow") morfeusz::MorphInterpretation::getLabelsAsString %{
def getLabelsAsUnicode(self, morfeusz):
    """
    Returns this interpretation labels as string
    """
    return $action(self, morfeusz).decode('utf8')
%}

%feature("shadow") morfeusz::MorphInterpretation::getLabels %{
def getLabels(self, morfeusz):
    """
    Returns this interpretation labels as a list of strings
    """
    return [l.decode('utf8') for l in $action(self, morfeusz)]
%}

%feature("shadow") morfeusz::MorphInterpretation::createIgn %{
@staticmethod
def createIgn(startNode, endNode, orth, lemma):
    """
    Creates unknown interpretation
    """
    return $action(self, startNode, endNode, orth.encode('utf8'), lemma.encode('utf8'))
%}

%feature("shadow") morfeusz::MorphInterpretation::createWhitespace %{
@staticmethod
def createWhitespace(startNode, endNode, orth):
    """
    Creates whitespace interpretation
    """
    return $action(self, startNode, endNode, orth.encode('utf8'))
%}

%feature("shadow") morfeusz::IdResolver::getTag %{
def getTag(self, tagId):
    return $action(self, tagId).decode('utf8')
%}

%feature("shadow") morfeusz::IdResolver::getTagId %{
def getTagId(self, tag):
    return $action(self, tag.encode('utf8'))
%}

%feature("shadow") morfeusz::IdResolver::getName %{
def getName(self, nameId):
    return $action(self, nameId).decode('utf8')
%}

%feature("shadow") morfeusz::IdResolver::getNameId %{
def getNameId(self, name):
    return $action(self, name.encode('utf8'))
%}

%feature("shadow") morfeusz::IdResolver::getLabelsAsString %{
def getLabelsAsUnicode(self, labelsId):
    return $action(self, labelsId).decode('utf8')
%}

%feature("shadow") morfeusz::IdResolver::getLabels %{
def getLabels(self, labelsId):
    return [l.decode('utf8') for l in $action(self, labelsId)]
%}

%feature("shadow") morfeusz::IdResolver::getLabelsId %{
def getLabelsId(self, labelsStr):
    return $action(self, labelsStr.encode('utf8'))
%}

%pythoncode %{
import collections
import os.path

# skopiowane, bo kod sie wkleja na poczatku zamiast na koncu
CONTINUOUS_NUMBERING = _morfeusz2.CONTINUOUS_NUMBERING
CONDITIONALLY_CASE_SENSITIVE = _morfeusz2.CONDITIONALLY_CASE_SENSITIVE
SKIP_WHITESPACES = _morfeusz2.SKIP_WHITESPACES
ANALYSE_ONLY = _morfeusz2.ANALYSE_ONLY
GENERATE_ONLY = _morfeusz2.GENERATE_ONLY
BOTH_ANALYSE_AND_GENERATE = _morfeusz2.BOTH_ANALYSE_AND_GENERATE

__version__ = _morfeusz2._Morfeusz_getVersion()

__copyright__ = _morfeusz2._Morfeusz_getCopyright().decode('utf-8')

GENDERS = ['m1', 'm2', 'm3', 'f', 'n']


class Morfeusz(_object):
    def __init__(self, dict_name=None, dict_path=None,
                 analyse=True, generate=True, expand_dag=False,
                 expand_tags=False, expand_dot=True, expand_underscore=True,
                 aggl=None, praet=None, separate_numbering=True,
                 case_handling=CONDITIONALLY_CASE_SENSITIVE,
                 whitespace=SKIP_WHITESPACES):
        """
        case_handling options:
            CONDITIONALLY_CASE_SENSITIVE, STRICTLY_CASE_SENSITIVE, IGNORE_CASE
        whitespace options:
            SKIP_WHITESPACES, KEEP_WHITESPACES, APPEND_WHITESPACES
        """
        if analyse and generate:
            usage = BOTH_ANALYSE_AND_GENERATE
        elif analyse:
            usage = ANALYSE_ONLY
        elif generate:
            usage = GENERATE_ONLY
        else:
            raise ValueError(
                'At least one of "analyse" and "generate" must be True')
        self.expand_dag = expand_dag
        self.expand_tags = expand_tags
        self.expand_dot = expand_dot
        self.expand_underscore = expand_underscore
        if dict_path:
            self.add_dictionary_path(dict_path)
        if dict_name:
            m = _Morfeusz.createInstance(dict_name, usage)
        else:
            m = _Morfeusz.createInstance(usage)
        self._morfeusz_obj = m
        if aggl:
            m.setAggl(aggl)
        if praet:
            m.setPraet(praet)
        if not separate_numbering:
            m.setTokenNumbering(CONTINUOUS_NUMBERING)
        m.setCaseHandling(case_handling)
        m.setWhitespaceHandling(whitespace)

    def add_dictionary_path(self, dict_path):
        dict_paths = _morfeusz2._Morfeusz_dictionarySearchPaths_get()
        if dict_path not in dict_paths:
            _morfeusz2._Morfeusz_dictionarySearchPaths_set(
                (dict_path,) + dict_paths)

    def _expand_tag(self, tag):
        chunks = [
            GENDERS if chunk == '_' and self.expand_underscore
            else chunk.split('.')
            for chunk in tag.split(':')
        ]

        if not self.expand_dot:
            yield ':'.join('.'.join(values) for values in chunks)
            return

        def expand_chunks(i):
            if i >= len(chunks):
                yield ()
            else:
                tail = tuple(expand_chunks(i + 1))
                for chunk_variant in chunks[i]:
                    for tail_variant in tail:
                        yield (chunk_variant,) + tail_variant

        for x in expand_chunks(0):
            yield ':'.join(x)

    def _expand_interp(self, interp):
        tags = self._expand_tag(interp[2])
        for tag in tags:
            yield (interp[0], interp[1], tag, interp[3], interp[4])

    @staticmethod
    def _dag_to_list(interps):
        dag = collections.defaultdict(list)
        for start, end, interp in interps:
            dag[start].append((interp, end))
        def expand_dag(start):
            nexts = dag[start]
            if not nexts:
                yield []
            else:
                for head, end in nexts:
                    for tail in expand_dag(end):
                        yield [head] + tail
        return list(expand_dag(0))

    def _interp2tuple(self, i):
        m = self._morfeusz_obj
        return i.orth, i.lemma, i.getTag(m), i.getName(m), i.getLabels(m)

    def analyse(self, text):
        m = self._morfeusz_obj
        interps = m.analyse(text)
        interp_tuples = [
            (i.startNode, i.endNode, self._interp2tuple(i)) for i in interps]

        def expand_interps():
            for start, end, interp in interp_tuples:
                for exp_interp in self._expand_interp(interp):
                    yield start, end, exp_interp

        if self.expand_tags:
            interp_tuples = list(expand_interps())
        if self.expand_dag:
            interp_tuples = self._dag_to_list(interp_tuples)
        return interp_tuples

    def generate(self, lemma, tag_id=None):
        m = self._morfeusz_obj
        interps = m.generate(lemma, tag_id)
        interp_tuples = [self._interp2tuple(i) for i in interps]

        def expand_interps():
            for interp in interp_tuples:
                for exp_interp in self._expand_interp(interp):
                    yield exp_interp

        if self.expand_tags:
            interp_tuples = list(expand_interps())
        return interp_tuples

    def dict_id(self):
        return self._morfeusz_obj.getDictID().decode('utf-8')

    def dict_copyright(self):
        return self._morfeusz_obj.getDictCopyright().decode('utf-8')
%}

%include "std_vector.i"
%include "std_string.i"
%include "std_list.i"
%include "std_set.i"
%include "std_except.i"
%include "exception.i"
%include "typemaps.i"
