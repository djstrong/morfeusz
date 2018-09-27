
package pl.sgjp.morfeusz;

/**
 * Iterates through morphological analysis and synthesis results.
 * 
 */

public class ResultsIterator implements java.util.Iterator<MorphInterpretation> {
    
    private _ResultsIterator realIt;
    private MorphInterpretation peek = null;

    protected ResultsIterator(_ResultsIterator realIt) {
        this.realIt = realIt;
    }
    
    /**
     * Removing of elements from this iterator is not supported.
     */
    public void remove() {
        throw new java.lang.UnsupportedOperationException();
    }

    public boolean hasNext() {
        return this.peek != null || this.realIt.hasNext();
    }

    public MorphInterpretation peek() {
        if (peek == null) {
            this.peek = new MorphInterpretation(realIt.next());
        }
        return this.peek;
    }

    public MorphInterpretation next() throws java.util.NoSuchElementException {
        MorphInterpretation res = this.peek();
        this.peek = null;
        return res;
    }
}
