%typemap(javaimports) morfeusz::Morfeusz %{
import java.io.IOException;
import java.lang.RuntimeException;
import java.util.List;
import java.util.ArrayList;

/**
 * Performs morphological analysis (analyse methods) and syntesis (generate methods).
 * 
 * It is NOT thread-safe
 * but it is possible to use separate Morfeusz instance for each concurrent thread.
 */
%}

%typemap(javaimports) morfeusz::ResultsIterator %{
/**
 * Iterates through morphological analysis and synthesis results.
 * 
 */
%}

%typemap(javaimports) morfeusz::MorphInterpretation %{
/**
 * <p>The result of morphological analysis or synthesis</p>
 * 
 * <p>
 * The result of analysis is  a directed acyclic graph with numbered
 * nodes representing positions  in text (points _between_ segments)
 * and edges representing interpretations of segments that span from
 * one node to another.  E.g.,
 * <pre>
 * {0,1,"ja","ja","ppron12:sg:nom:m1.m2.m3.f.n1.n2:pri"}
 *       |
 *       |      {1,2,"został","zostać","praet:sg:m1.m2.m3:perf"}
 *       |      |
 *     __|  ____|   __{2,3,"em","być","aglt:sg:pri:imperf:wok"}
 *    /  \ /     \ / \
 *   * Ja * został*em *
 *   0    1       2   3
 * </pre>
 * </p>
 * 
 * <p>
 * Note that the word 'zostałem' got broken into 2 separate segments.
 * </p>

 * <p>One MorphInterpretation instance describes one edge of this DAG.</p>
 */
%}

%typemap(javaimports) morfeusz::IdResolver %{
/**
 * Represents mappings for tags, names and labels.
 */
%}

%typemap(javaimports) morfeusz::WhitespaceHandling %{
/**
 * Represents whitespace handling strategies (default is SKIP_WHITESPACES).
 */
%}

%typemap(javaimports) morfeusz::CaseHandling %{
/**
 * Represents case handling strategies (default is CONDITIONALLY_CASE_SENSITIVE).
 */
%}

%typemap(javaimports) morfeusz::TokenNumbering %{
/**
 * Represents token numbering strategies (default is SEPARATE_NUMBERING).
 */
%}

%javamethodmodifiers morfeusz::Morfeusz::createInstance "
    /**
     * Creates actual instance of Morfeusz class.
     * 
     * @return Morfeusz instance
     */
    public synchronized";

%javamethodmodifiers morfeusz::Morfeusz::getVersion() "
    /**
     * Returns a string containing library version.
     * @return library version string
     */
    public";

%javamethodmodifiers morfeusz::Morfeusz::analyseAsIterator(const char*) const "
    /**
     * Analyze given text and return the results as iterator.
     * It does not store results for whole text at once, so may be less memory-consuming for analysis of big texts.
     * 
     * NOT THREAD-SAFE (must have exclusive access to this instance).
     * 
     * @param text text for morphological analysis.
     * @return iterator over morphological analysis results
     */
    public";

%javamethodmodifiers morfeusz::Morfeusz::setAggl(const std::string&) "
    /**
     * Set aggl segmentation option value.
     * 
     * NOT THREAD-SAFE (must have exclusive access to this instance).
     * 
     * @param aggl aggl value
     * @throws MorfeuszException when invalid aggl parameter provided
     */
    public";

%javamethodmodifiers morfeusz::Morfeusz::setPraet(const std::string&) "
    /**
     * Set praet segmentation option value.
     * 
     * NOT THREAD-SAFE (must have exclusive access to this instance).
     * 
     * @param praet praet value
     * @throws MorfeuszException when invalid praet parameter provided
     */
    public";

%javamethodmodifiers morfeusz::Morfeusz::setCaseHandling(CaseHandling) "
    /**
     * Set case handling.
     * 
     * NOT THREAD-SAFE (must have exclusive access to this instance).
     * 
     * @param caseHandling case handling policy
     */
    public";

%javamethodmodifiers morfeusz::Morfeusz::setTokenNumbering(TokenNumbering) "
    /**
     * Set token numbering policy.
     * 
     * NOT THREAD-SAFE (must have exclusive access to this instance).
     * 
     * @param numbering token numbering policy
     */
    public";

%javamethodmodifiers morfeusz::Morfeusz::setWhitespaceHandling(WhitespaceHandling) "
    /**
     * Set whitespace handling.
     * 
     * NOT THREAD-SAFE (must have exclusive access to this instance).
     * 
     * @param whitespaceHandling whitespace handling policy
     */
    public";