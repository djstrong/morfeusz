
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.List;
import java.util.NoSuchElementException;
import org.junit.After;
import org.junit.AfterClass;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import pl.sgjp.morfeusz.Morfeusz;
import pl.sgjp.morfeusz.MorfeuszUsage;
import pl.sgjp.morfeusz.MorfeuszException;
import pl.sgjp.morfeusz.MorphInterpretation;
import pl.sgjp.morfeusz.ResultsIterator;

/**
 *
 * @author mlenart
 */
public class JMorfeuszTest {

    private Morfeusz morfeusz;

    public JMorfeuszTest() {
    }

    @BeforeClass
    public static void setUpClass() {
    }

    @AfterClass
    public static void tearDownClass() {
    }

    @Before
    public void setUp() {
        morfeusz = Morfeusz.createInstance(MorfeuszUsage.BOTH_ANALYSE_AND_GENERATE);
    }

    @After
    public void tearDown() {
    }

    @Test
    public void testAnalyzeAsList() {
        List<MorphInterpretation> res = morfeusz.analyseAsList("Aaaa żżżż");
        assertEquals(2, res.size());
        assertEquals("Aaaa", res.get(0).getOrth());
        assertEquals("żżżż", res.get(1).getOrth());
        try {
            res.get(2);
            fail();
        } catch (IndexOutOfBoundsException ex) {

        }
    }

    @Test
    public void testAnalyzeAsIterator() {
        ResultsIterator it = morfeusz.analyseAsIterator("Aaaa żżżż");
        assertTrue(it.hasNext());
        assertEquals("Aaaa", it.next().getOrth());
        assertTrue(it.hasNext());
        assertEquals("żżżż", it.next().getOrth());
        try {
            it.next();
            fail();
        } catch (NoSuchElementException ex) {

        }
    }

    @Test(expected = MorfeuszException.class)
    public void testInvalidAgglOption() {
        morfeusz.setAggl("XXXXYYYYZZZZ");
    }

    @Test(expected = MorfeuszException.class)
    public void testInvalidPraetOption() {
        morfeusz.setPraet("XXXXYYYYZZZZ");
    }

    @Test(expected = MorfeuszException.class)
    public void testInvalidGenerate() {
        morfeusz.generate("AAAA BBBB");
    }

    @Test(expected = NullPointerException.class)
    public void testNullCaseHandling() {
        morfeusz.setCaseHandling(null);
    }

    @Test(expected = MorfeuszException.class)
    public void testNonExistingDictionary() throws IOException {
        morfeusz.setDictionary("ee2rmtsq");
    }

    @Test(expected = IOException.class)
    public void testInvalidDictionary() throws Exception {
        String dictName = "6J1vMiqY";
        File tmpFile = new File(dictName + "-a.dict");
        assertTrue(tmpFile.createNewFile());
        tmpFile.deleteOnExit();
        try (PrintStream out = new PrintStream(tmpFile)) {
            out.print("IzEne9FXuc");
        }
        morfeusz.getDictionarySearchPaths().add(0, ".");
        morfeusz.setDictionary(dictName);
    }
}
