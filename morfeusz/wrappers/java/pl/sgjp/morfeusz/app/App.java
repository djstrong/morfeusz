package pl.sgjp.morfeusz.app;

import pl.sgjp.morfeusz.Morfeusz;
import pl.sgjp.morfeusz.MorfeuszUsage;
import pl.sgjp.morfeusz.ResultsIterator;

/**
 * Hello world!
 *
 */
public class App {

    public static void main(String[] args) {
        System.err.println("Working Directory=" +
              System.getProperty("user.dir"));
        System.err.println("java.library.path="+System.getProperty("java.library.path"));
        Morfeusz morfeusz = Morfeusz.createInstance(MorfeuszUsage.ANALYSE_ONLY);
        ResultsIterator it = morfeusz.analyseAsIterator("Ala ma kota i żółć.");
        while (it.hasNext()) {
            System.out.println(MorfeuszUtils.getInterpretationString(it.next(), morfeusz));
        }
    }
}
