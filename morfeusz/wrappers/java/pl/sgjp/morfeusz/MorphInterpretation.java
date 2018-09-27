
package pl.sgjp.morfeusz;

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

public class MorphInterpretation {
    
    private int startNode;
    private int endNode;
    
    private String orth;
    private String lemma;
    
    private int tagId;
    private int nameId;
    private int labelsId;
    
    protected MorphInterpretation(_MorphInterpretation mi) {
        this.startNode = mi.getStartNode();
        this.endNode = mi.getEndNode();
        this.orth = mi.getOrth();
        this.lemma = mi.getLemma();
        this.tagId = mi.getTagId();
        this.nameId = mi.getNameId();
        this.labelsId = mi.getLabelsId();
    }
    
static java.util.List<MorphInterpretation> createList(InterpsList interpsList) {
	java.util.List<MorphInterpretation> res = new java.util.ArrayList<MorphInterpretation>();
        for (_MorphInterpretation mi: interpsList) {
            res.add(new MorphInterpretation(mi));
        }
        return res;
    }
    
    public java.util.Collection<java.lang.String> getLabels(Morfeusz morfeusz) {
        return morfeusz.getIdResolver().getLabels(labelsId);
    }

    public MorphInterpretation() {
        this.startNode = 0;
        this.endNode = 0;
        this.orth = "";
        this.lemma = "";
        this.tagId = 0;
        this.nameId = 0;
        this.labelsId = 0;
    }

    public static MorphInterpretation createIgn(int startNode, int endNode, String orth, String lemma) {
        return new MorphInterpretation(new _MorphInterpretation(MorfeuszWrapperJNI._MorphInterpretation_createIgn(startNode, endNode, orth, lemma), true));
    }

    public static MorphInterpretation createWhitespace(int startNode, int endNode, String orth) {
        return new MorphInterpretation(new _MorphInterpretation(MorfeuszWrapperJNI._MorphInterpretation_createWhitespace(startNode, endNode, orth), true));
    }

    public boolean isIgn() {
        return tagId == 0;
    }

    public boolean isWhitespace() {
        return tagId == 1;
    }

    public String getTag(Morfeusz morfeusz) {
        return morfeusz.getIdResolver().getTag(tagId);
    }

    public String getName(Morfeusz morfeusz) {
        return morfeusz.getIdResolver().getName(nameId);
    }

    public String getLabelsAsString(Morfeusz morfeusz) {
        return morfeusz.getIdResolver().getLabelsAsString(labelsId);
    }

    public void setStartNode(int value) {
        this.startNode = value;
    }

    public int getStartNode() {
        return this.startNode;
    }

    public void setEndNode(int value) {
        this.endNode = value;
    }

    public int getEndNode() {
        return this.endNode;
    }

    public void setOrth(String value) {
        this.orth = value;
    }

    public String getOrth() {
        return this.orth;
    }

    public void setLemma(String value) {
        this.lemma = value;
    }

    public String getLemma() {
        return this.lemma;
    }

    public void setTagId(int value) {
        this.tagId = value;
    }

    public int getTagId() {
        return this.tagId;
    }

    public void setNameId(int value) {
        this.nameId = value;
    }

    public int getNameId() {
        return this.nameId;
    }

    public void setLabelsId(int value) {
        this.labelsId = value;
    }

    public int getLabelsId() {
        return this.labelsId;
    }
}
