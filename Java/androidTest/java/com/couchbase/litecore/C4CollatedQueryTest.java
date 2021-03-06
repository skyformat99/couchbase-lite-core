package com.couchbase.litecore;

import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;

public class C4CollatedQueryTest extends C4QueryBaseTest {
    static final String TAG = C4QueryTest.class.getSimpleName();

    //-------------------------------------------------------------------------
    // public methods
    //-------------------------------------------------------------------------

    @Override
    public void setUp() throws Exception {
        super.setUp();
        importJSONLines("iTunesMusicLibrary.json");
    }

    @Override
    public void tearDown() throws Exception {
        if (query != null) {
            query.free();
            query = null;
        }
        super.tearDown();
    }

    protected List<String> run() throws LiteCoreException {
        C4QueryEnumerator e = query.run(new C4QueryOptions(), null);
        try {
            List<String> results = new ArrayList<>();
            while (e.next())
                results.add(e.getColumns().getValueAt(0).asString());
            return results;
        } finally {
            e.free();
        }
    }

    //-------------------------------------------------------------------------
    // tests
    //-------------------------------------------------------------------------

    // - DB Query collated
    @Test
    public void testDBQueryCollated() throws LiteCoreException {
        compileSelect(json5("{WHAT: [ ['.Name'] ], "
                + "WHERE: ['COLLATE', {'unicode': true, 'case': false, 'diacritic': false}, ['=', ['.Artist'], 'Benoît Pioulard']],"
                + "ORDER_BY: [ ['COLLATE', {'unicode': true, 'case': false, 'diacritic': false}, ['.Name']] ]}"));
        List<String> tracks = run();
        assertEquals(2, tracks.size());
    }

    // - DB Query aggregate collated
    @Test
    public void testDBQueryAggregateCollated() throws LiteCoreException {
        compileSelect(json5("{WHAT: [ ['COLLATE', {'unicode': true, 'case': false, 'diacritic': false}, ['.Artist']] ], "
                + "DISTINCT: true, "
                + "ORDER_BY: [ ['COLLATE', {'unicode': true, 'case': false, 'diacritic': false}, ['.Artist']] ]}"));
        List<String> artists = run();
        assertEquals(2097, artists.size());

        // Benoît Pioulard appears twice in the database, once miscapitalized as BenoÎt Pioulard.
        // Check that these got coalesced by the DISTINCT operator:
        assertEquals("Benny Goodman", artists.get(214));
        assertEquals("Benoît Pioulard", artists.get(215));
        assertEquals("Bernhard Weiss", artists.get(216));

        // Make sure "Zoë Keating" sorts correctly:
        assertEquals("ZENИTH (feat. saåad)", artists.get(2082));
        assertEquals("Zoë Keating", artists.get(2083));
        assertEquals("Zola Jesus", artists.get(2084));
    }
}
